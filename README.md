# Clarity VM

## Prerequisities

- GCC
- Make
- Go

## Building

To build the vm with all its tools you can run the following commands in the cloned repository

```sh
make
cd compiler
go build -o clarityc src/main.go
```

This should make a `dist/clarity` executable which is the vm and `compiler/clarityc` executable which is the compiler for the vm

## Running

To run any `clarity` complient assembly you can run the `clarityc` executable with the file as argument, this will produce `<file>.bin` file which you can then use as argument for the `clarity` executable

## Assembly

### Meaning
- `reg` -- general purpose register, vm has 8: `r0`-`r7`
- `imm` -- immediate value represented by a number
- `addr` -- memory address represented by a number
- `lbl` -- label, address calculated at compile time

### Instruction Set
- `add <reg1>, <reg2>` -- add two values and store the result in `reg1`
- `sub <reg1>, <reg2>` -- subtract two values and store the result in `reg1`
- `mul <reg1>, <reg2>` -- multiply two values and store the result in `reg1`
- `div <reg1>, <reg2>` -- divide two values and store the result in `reg1`
- `mod <reg1>, <reg2>` -- modulo two values and store the result in `reg1`
- `inc <reg>` -- increase the value in `reg` by 1
- `dec <reg>` -- decrease the value in `reg` by 1

- `and <reg1>, <reg2>` -- bitwise AND two values and store the result in `reg1`
- `or <reg1>, <reg2>` -- bitwise OR two values and store the result in `reg1`
- `xor <reg1>, <reg2>` -- bitwise XOR two values and store the result in `reg1`
- `not <reg>` -- bitwise NOT a valueand store the result in `reg`
- `shl <reg>, <imm>` -- shift left value in `reg` by `imm` bits
- `shr <reg>, <imm>` -- shift right value in `reg` by `imm` bits

- `mov <reg>, <imm>` -- move `imm` into `reg`
- `load <reg>, <addr>` -- load value from `addr` into `reg`
- `store <reg>, <addr>` -- store value from `reg` into `addr`
- `push <reg>` -- push value from `reg` onto the stack
- `pop <reg>` -- pop value from stack into `reg`

- `jmp <lbl>` -- jump to label
- `jz <lbl>` -- jump to label if last comparison was zero (equal)
- `jnz <lbl>` -- jump to label if last comparison wasn't zero (not equal)
- `jg <lbl>` -- jump to label if last comparison was greater
- `jl <lbl>` -- jump to label if last comparicon was less
- `call <lbl>` -- call a function
- `ret` -- return from a function

- `cmp <reg1>, <reg2>` -- compare two values in `reg1` and `reg2`
- `test <reg1>, <reg2>` -- test two values in `reg1` and `reg2` (use only for `==`)
- `nop` -- no operator (does nothing)
- `hlt` -- halt, stop the vm
