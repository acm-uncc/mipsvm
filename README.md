# mipsvm

## To get started

Run this command on the command line clone the repo.
- `git clone https://github.com/acm-uncc/mipsvm.git`

## To add a new feature

- git branch (your name)
- git checkout (your name)
- Find an instruction [here](https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00086-2B-MIPS32BIS-AFP-6.06.pdf) that is not already implemented (or pick one from our list below)
- Identify instruction layout
- Create function with op_ + function name
- First argument to function will be MIPS32_VM& vm
- Other arguments will be instruction layout (except for the first section)
- Implement function according to what the page says
- Go to op_handlers array, find the opcode (first number) that corresponds to a number on the list
- Change nullptr with the name of the function you implemented like reinterpret_cast<OP>(name)
- Go to [here](https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00082-2B-MIPS32INT-AFP-06.01.pdf)
- Figure out type of instruction based on page 73
- Look through enum of OP_TYPE
- Go to op_types array and find the opcode of your function
- Replace UNIMPLEMENTED with OP_TYPE
- git push origin master
- Go to the repository then the Pull requests tab
- Hit new pull request
- Select your branch
- Create pull request and mention which instruction you did in the title

## Simple New Instructions
- MULT
- DIV
- AND
- OR
- SLL
- SRL

## Questions?
Please ask in the ACM Discord if you have any questions.
