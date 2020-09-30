#include<iostream>

typedef unsigned char byte;
typedef unsigned int uns;

struct MIPS32_VM;

typedef void(*OP)(MIPS32_VM&);
typedef void(*OP_R_TYPE)(MIPS32_VM&,uns,uns,uns,uns,uns);
typedef void(*OP_I_TYPE)(MIPS32_VM&,uns,uns,uns);
typedef void(*OP_I_OFF21)(MIPS32_VM&,uns,uns);
typedef void(*OP_I_OFF26)(MIPS32_VM&,uns);
typedef void(*OP_I_OFF11)(MIPS32_VM&,uns,uns,uns,uns);
typedef void(*OP_I_OFF9)(MIPS32_VM&,uns,uns,uns,uns);
typedef void(*OP_J_TYPE)(MIPS32_VM&,uns);

enum OP_TYPE {
   UNIMPLEMENTED,
   R_Type,
   I_Type, // Imm16
   I_Off21,
   I_Off26,
   I_Off11,
   I_Off9,
   J_Type
};

//-----------------------//
// Core VM implmentation //
//-----------------------//

struct MIPS32_VM {
protected:
   static const OP op_handlers[64];
   static const OP_TYPE op_types[64];

public:
   void execute(uns inst) {
      // Masks (static constexer means they're only defined once, and not re-defined
      // every time the funciton is called)
      static constexpr uns num_bits = 32;
      static constexpr uns opcode_mask = 0b11111100000000000000000000000000;
      static constexpr uns opcode_shamt = num_bits - 6;
      // R_Type masks
      static constexpr uns rs_mask = 0b00000011111000000000000000000000;
      static constexpr uns rs_shamt = opcode_shamt-5;
      static constexpr uns rt_mask = 0b00000000000111110000000000000000;
      static constexpr uns rt_shamt = rs_shamt-5;
      static constexpr uns rd_mask = 0b00000000000000001111100000000000;
      static constexpr uns rd_shamt = rt_shamt-5;
      static constexpr uns sa_mask = 0b00000000000000000000011111000000;
      static constexpr uns sa_shamt = rd_shamt-5;
      static constexpr uns function_mask = 0b00000000000000000000000000111111;
      static constexpr uns function_shamt = sa_shamt-6;
      // I_Type masks
      static constexpr uns immediate_mask = rd_mask | sa_mask | function_mask;
      static constexpr uns immediate_shamt = function_shamt;
      // I_Off21 masks
      static constexpr uns offset21_mask = rt_mask | rd_mask | sa_mask | function_mask;
      static constexpr uns offset21_shamt = function_shamt;
      // I_Off26 masks
      static constexpr uns offset26_mask = ~opcode_mask;
      static constexpr uns offset26_shamt = function_shamt;
      // I_Off11 masks
      static constexpr uns offset11_mask = sa_mask | function_mask;
      static constexpr uns offset11_shamt = function_shamt;
      // I_Off9 masks
      static constexpr uns offset9_mask = rd_mask | sa_mask;
      static constexpr uns offset9_shamt = sa_shamt + 1;
      // J_Type masks
      static constexpr uns instr_index_mask = ~opcode_mask;
      static constexpr uns instr_index_shamt = function_shamt;

      // Opcode dispatcher
      uns opcode = (inst & opcode_mask) >> opcode_shamt;
      switch (op_types[opcode]) {
         case UNIMPLEMENTED:
            break;
         case R_Type: {
            uns rs = (inst & rs_mask) >> rs_shamt;
            uns rt = (inst & rt_mask) >> rt_shamt;
            uns rd = (inst & rd_mask) >> rd_shamt;
            uns sa = (inst & sa_mask) >> sa_shamt;
            uns function = (inst & function_mask) >> function_shamt;
            auto r_handler = reinterpret_cast<OP_R_TYPE>(op_handlers[opcode]);
            r_handler(*this, rs, rt, rd, sa, function);
            break;
         }
         case I_Type: {
            uns rs = (inst & rs_mask) >> rs_shamt;
            uns rt = (inst & rt_mask) >> rt_shamt;
            uns immediate = (inst & immediate_mask) >> immediate_shamt;
            auto i1_handler = reinterpret_cast<OP_I_TYPE>(op_handlers[opcode]);
            i1_handler(*this, rs, rt, immediate);
            break;
         }
         case I_Off21: {
            uns rd = (inst & rs_mask) >> rs_shamt;
            uns offset = (inst & offset21_mask) >> offset21_shamt;
            auto i2_handler = reinterpret_cast<OP_I_OFF21>(op_handlers[opcode]);
            i2_handler(*this, rd, offset);
            break;
         }
         case I_Off26: {
            uns offset = (inst & offset26_mask) >> offset26_shamt;
            auto i3_handler = reinterpret_cast<OP_I_OFF26>(op_handlers[opcode]);
            i3_handler(*this, offset);
            break;
         }
         case I_Off11: {
            uns rs = (inst & rs_mask) >> rs_shamt;
            uns rt = (inst & rt_mask) >> rt_shamt;
            uns rd = (inst & rd_mask) >> rd_shamt;
            uns offset = (inst & offset11_mask) >> offset11_shamt;
            auto i4_handler = reinterpret_cast<OP_I_OFF11>(op_handlers[opcode]);
            i4_handler(*this, rs, rt, rd, offset);
            break;
         }
         case I_Off9: {
            uns base = (inst & rs_mask) >> rs_shamt;
            uns rt = (inst & rt_mask) >> rt_shamt;
            uns offset = (inst & offset9_mask) >> offset9_shamt;
            uns function = (inst & function_mask) >> function_shamt;
            auto i5_handler = reinterpret_cast<OP_I_OFF9>(op_handlers[opcode]);
            i5_handler(*this, base, rt, offset, function);
            break;
         }
         case J_Type: {
            uns instr_index = (inst & instr_index_mask) >> instr_index_shamt;
            auto j_handler = reinterpret_cast<OP_J_TYPE>(op_handlers[opcode]);
            j_handler(*this, instr_index);
            break;
         }
      }
   }

   int GPR[32];
   int PC;
   byte* MEM;
   uns memsize;

   MIPS32_VM(uns memsize = 0) {

   }
};

//------------------------//
// Special Opcode Section //
//------------------------//

typedef void(*SPECIAL_FUNC)(MIPS32_VM&, uns, uns, uns, uns);

struct Special_Registry {
   static const SPECIAL_FUNC func_handlers[64];

   SPECIAL_FUNC get(uns num) {
      return func_handlers[num];
   }
};

Special_Registry special_reg;

void op_special(MIPS32_VM& vm, uns rs, uns rt, uns rd, uns sa, uns function) {
   special_reg.get(function)(vm,rs,rt,rd,sa);
}

// Special function implementation

// For any instruction with the SPECIAL opcode (000000), implement the function
// as follows (as an example is the ADD instruction. sa is not used, so it is
// ignored). The arguments for any special function should be as below.
void special_add(MIPS32_VM& vm, uns rs, uns rt, uns rd, uns sa) {
   vm.GPR[rd] = vm.GPR[rs] + vm.GPR[rd];
}

// Special function registration

// Put the name of the function implemented on the line corresponding to the
// binary value of the LAST section in the instruction below. ADD has been
// placed already
const SPECIAL_FUNC Special_Registry::func_handlers[] = {
   nullptr, // 000000
   nullptr, // 000001
   nullptr, // 000010
   nullptr, // 000011
   nullptr, // 000100
   nullptr, // 000101
   nullptr, // 000110
   nullptr, // 000111
   nullptr, // 001000
   nullptr, // 001001
   nullptr, // 001010
   nullptr, // 001011
   nullptr, // 001100
   nullptr, // 001101
   nullptr, // 001110
   nullptr, // 001111
   nullptr, // 010000
   nullptr, // 010001
   nullptr, // 010010
   nullptr, // 010011
   nullptr, // 010100
   nullptr, // 010101
   nullptr, // 010110
   nullptr, // 010111
   nullptr, // 011000
   nullptr, // 011001
   nullptr, // 011010
   nullptr, // 011011
   nullptr, // 011100
   nullptr, // 011101
   nullptr, // 011110
   nullptr, // 011111
   special_add, // 100000
   nullptr, // 100001
   nullptr, // 100010
   nullptr, // 100011
   nullptr, // 100100
   nullptr, // 100101
   nullptr, // 100110
   nullptr, // 100111
   nullptr, // 101000
   nullptr, // 101001
   nullptr, // 101010
   nullptr, // 101011
   nullptr, // 101100
   nullptr, // 101101
   nullptr, // 101110
   nullptr, // 101111
   nullptr, // 110000
   nullptr, // 110001
   nullptr, // 110010
   nullptr, // 110011
   nullptr, // 110100
   nullptr, // 110101
   nullptr, // 110110
   nullptr, // 110111
   nullptr, // 111000
   nullptr, // 111001
   nullptr, // 111010
   nullptr, // 111011
   nullptr, // 111100
   nullptr, // 111101
   nullptr, // 111110
   nullptr, // 111111
};

//----------------------//
// Basic Opcode Section //
//----------------------//

void op_addiu(MIPS32_VM& vm, uns rs, uns rt, uns immediate) {
   vm.GPR[rt] = vm.GPR[rs] + immediate;
}

// Opcode registration

const OP MIPS32_VM::op_handlers[] = {
   reinterpret_cast<OP>(op_special), // 000000
   nullptr, // 000001
   nullptr, // 000010
   nullptr, // 000011
   nullptr, // 000100
   nullptr, // 000101
   nullptr, // 000110
   nullptr, // 000111
   nullptr, // 001000
   reinterpret_cast<OP>(op_addiu), // 001001
   nullptr, // 001010
   nullptr, // 001011
   nullptr, // 001100
   nullptr, // 001101
   nullptr, // 001110
   nullptr, // 001111
   nullptr, // 010000
   nullptr, // 010001
   nullptr, // 010010
   nullptr, // 010011
   nullptr, // 010100
   nullptr, // 010101
   nullptr, // 010110
   nullptr, // 010111
   nullptr, // 011000
   nullptr, // 011001
   nullptr, // 011010
   nullptr, // 011011
   nullptr, // 011100
   nullptr, // 011101
   nullptr, // 011110
   nullptr, // 011111
   nullptr, // 100000
   nullptr, // 100001
   nullptr, // 100010
   nullptr, // 100011
   nullptr, // 100100
   nullptr, // 100101
   nullptr, // 100110
   nullptr, // 100111
   nullptr, // 101000
   nullptr, // 101001
   nullptr, // 101010
   nullptr, // 101011
   nullptr, // 101100
   nullptr, // 101101
   nullptr, // 101110
   nullptr, // 101111
   nullptr, // 110000
   nullptr, // 110001
   nullptr, // 110010
   nullptr, // 110011
   nullptr, // 110100
   nullptr, // 110101
   nullptr, // 110110
   nullptr, // 110111
   nullptr, // 111000
   nullptr, // 111001
   nullptr, // 111010
   nullptr, // 111011
   nullptr, // 111100
   nullptr, // 111101
   nullptr, // 111110
   nullptr, // 111111
};

const OP_TYPE MIPS32_VM::op_types[] = {
   R_Type, // 000000
   UNIMPLEMENTED, // 000001
   UNIMPLEMENTED, // 000010
   UNIMPLEMENTED, // 000011
   UNIMPLEMENTED, // 000100
   UNIMPLEMENTED, // 000101
   UNIMPLEMENTED, // 000110
   UNIMPLEMENTED, // 000111
   UNIMPLEMENTED, // 001000
   I_Type, // 001001
   UNIMPLEMENTED, // 001010
   UNIMPLEMENTED, // 001011
   UNIMPLEMENTED, // 001100
   UNIMPLEMENTED, // 001101
   UNIMPLEMENTED, // 001110
   UNIMPLEMENTED, // 001111
   UNIMPLEMENTED, // 010000
   UNIMPLEMENTED, // 010001
   UNIMPLEMENTED, // 010010
   UNIMPLEMENTED, // 010011
   UNIMPLEMENTED, // 010100
   UNIMPLEMENTED, // 010101
   UNIMPLEMENTED, // 010110
   UNIMPLEMENTED, // 010111
   UNIMPLEMENTED, // 011000
   UNIMPLEMENTED, // 011001
   UNIMPLEMENTED, // 011010
   UNIMPLEMENTED, // 011011
   UNIMPLEMENTED, // 011100
   UNIMPLEMENTED, // 011101
   UNIMPLEMENTED, // 011110
   UNIMPLEMENTED, // 011111
   UNIMPLEMENTED, // 100000
   UNIMPLEMENTED, // 100001
   UNIMPLEMENTED, // 100010
   UNIMPLEMENTED, // 100011
   UNIMPLEMENTED, // 100100
   UNIMPLEMENTED, // 100101
   UNIMPLEMENTED, // 100110
   UNIMPLEMENTED, // 100111
   UNIMPLEMENTED, // 101000
   UNIMPLEMENTED, // 101001
   UNIMPLEMENTED, // 101010
   UNIMPLEMENTED, // 101011
   UNIMPLEMENTED, // 101100
   UNIMPLEMENTED, // 101101
   UNIMPLEMENTED, // 101110
   UNIMPLEMENTED, // 101111
   UNIMPLEMENTED, // 110000
   UNIMPLEMENTED, // 110001
   UNIMPLEMENTED, // 110010
   UNIMPLEMENTED, // 110011
   UNIMPLEMENTED, // 110100
   UNIMPLEMENTED, // 110101
   UNIMPLEMENTED, // 110110
   UNIMPLEMENTED, // 110111
   UNIMPLEMENTED, // 111000
   UNIMPLEMENTED, // 111001
   UNIMPLEMENTED, // 111010
   UNIMPLEMENTED, // 111011
   UNIMPLEMENTED, // 111100
   UNIMPLEMENTED, // 111101
   UNIMPLEMENTED, // 111110
   UNIMPLEMENTED, // 111111
};

int main() {
   MIPS32_VM vm;
   vm.execute(4);
}
