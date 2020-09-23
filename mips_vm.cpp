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
   R_type,
   I_type, // Imm16
   I_Off21,
   I_Off26,
   I_Off11,
   I_Off9,
   J_type
};

struct MIPS32_VM {
protected:
   static OP op_handlers[64];
   static OP_TYPE op_types[64];

public:
   void execute(uns inst) {
      static constexpr uns num_bits = 32;
      static constexpr uns opcode_mask = 0b11111100000000000000000000000000;
      static constexpr uns opcode_shamt = num_bits - 6;
      // R_type masks
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
      // I_type masks
      static constexpr uns immediate_mask = rd_mask | sa_mask | function_mask;
      static constexpr uns immediate_shamt = function_shamt;
      // I_Off21 masks

      uns opcode = (inst & opcode_mask) >> opcode_shamt;
      switch (op_types[opcode]) {
         case UNIMPLEMENTED:
            break;
         case R_type:
            uns rs = (inst & rs_mask) >> rs_shamt;
            uns rt = (inst & rt_mask) >> rt_shamt;
            uns rd = (inst & rd_mask) >> rd_shamt;
            uns sa = (inst & sa_mask) >> sa_shamt;
            uns function = (inst & function_mask) >> function_shamt;
            auto handler = reinterpret_cast<OP_R_TYPE>(op_handlers[opcode]);
            handler(*this,rs,rt,rd,sa,function);
            break;
      }
   }

   int GPR[32];
   int PC;
   byte* MEM;
   uns memsize;

   MIPS32_VM(uns memsize = 0) {
   }
};

// Add functions here

void op_special(MIPS32_VM& vm, uns rs, uns rt, uns rd, uns sa, uns function) {
   std::cout << rs << " " << rt << " " << rd << " " << sa << " " << function << std::endl;
}

void op_addiu(MIPS32_VM& vm, uns rs, uns rt, uns immediate) {
   vm.GPR[rt] = vm.GPR[rs] + immediate;   
}

void op_andi(MIPS32_VM& vm, uns rs, uns rt, uns immediate) {
   vm.GRP[rt] = vm.GRP[rs] & immediate;
}

OP MIPS32_VM::op_handlers[64] = {
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
   reinterpret_cast<OP>(op_andi), // 001100
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

OP_TYPE MIPS32_VM::op_types[64] = {
   R_type, // 000000
   UNIMPLEMENTED, // 000001
   UNIMPLEMENTED, // 000010
   UNIMPLEMENTED, // 000011
   UNIMPLEMENTED, // 000100
   UNIMPLEMENTED, // 000101
   UNIMPLEMENTED, // 000110
   UNIMPLEMENTED, // 000111
   UNIMPLEMENTED, // 001000
   I_type, // 001001
   UNIMPLEMENTED, // 001010
   UNIMPLEMENTED, // 001011
   I_type, // 001100
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
