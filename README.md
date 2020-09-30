# mipsvm

## To get started

Run this command on the command line clone the repo.
- `git clone https://github.com/acm-uncc/mipsvm.git`

## To add a new feature

1. Find an instruction [here](https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00086-2B-MIPS32BIS-AFP-6.06.pdf) that is not already implemented (or pick one from our list below)
1. git branch (instruction name)
1. git checkout (instruction name)
1. Identify instruction layout
1. Create function with op_ + function name
1. First argument to function will be MIPS32_VM& vm
1. Other arguments will be instruction layout (except for the first section)
1. Implement function according to what the page says
1. Go to op_handlers array, find the opcode (first number) that corresponds to a number on the list
1. Change nullptr with the name of the function you implemented like reinterpret_cast<OP>(name)
1. Go to [here](https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00082-2B-MIPS32INT-AFP-06.01.pdf)
1. Figure out type of instruction based on page 73
1. Look through enum of OP_TYPE
1. Go to op_types array and find the opcode of your function
1. Replace UNIMPLEMENTED with OP_TYPE
1. git add .
1. git commit -m "Add (instructin name)"
1. git push origin master
1. Go to the repository then the Pull requests tab
1. Hit new pull request
1. Select your branch
1. Create pull request and mention which instruction you did in the title

## Simple New Instructions
- MULT
- DIV
- AND
- OR
- SLL
- SRL
- XORI

## Questions?
Please ask in the ACM Discord if you have any questions.
