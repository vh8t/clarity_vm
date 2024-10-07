package parser

import (
	"compiler/src/lexer"
	"fmt"
	"os"
	"strings"
)

const (
	ADD = byte(iota)
	SUB
	MUL
	DIV
	MOD
	INC
	DEC
	AND
	OR
	XOR
	NOT
	SHL
	SHR
	MOV
	LOAD
	STORE
	PUSH
	POP
	JMP
	JZ
	JNZ
	JG
	JL
	CALL
	RET
	CMP
	TEST
	NOP
	HLT
)

func Parse(tokens []lexer.Token) (bytecode []byte) {
	var err bool
	var offset int

	unknown := make(map[int]string)
	labels := make(map[string]int)

	for i := 0; i < len(tokens); {
		token := tokens[i]

		switch token.Kind {
		case lexer.LABEL:
			if _, ok := labels[token.Value]; ok {
				fmt.Fprintf(os.Stderr, "duplicate label: `%s`\n", token.Value)
				err = true
			} else {
				labels[token.Value] = offset
			}
			i++
		case lexer.INSTR:
			switch strings.ToLower(token.Value) {
			case "add":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for add instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for add instruction\n")
					err = true
				}
				bytecode = append(bytecode, ADD, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				offset += 3
				i += 4
			case "sub":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for sub instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for sub instruction\n")
					err = true
				}
				bytecode = append(bytecode, SUB, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				offset += 3
				i += 4
			case "mul":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for mul instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for mul instruction\n")
					err = true
				}
				bytecode = append(bytecode, MUL, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				offset += 3
				i += 4
			case "div":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for div instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for div instruction\n")
					err = true
				}
				bytecode = append(bytecode, DIV, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				offset += 3
				i += 4
			case "mod":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for mod instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for mod instruction\n")
					err = true
				}
				bytecode = append(bytecode, MOD, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				offset += 3
				i += 4
			case "inc":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for inc instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for inc instruction\n")
					err = true
				}
				bytecode = append(bytecode, INC, regToByte(tokens[i+1].Value))
				offset += 2
				i += 2
			case "dec":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for dec instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for dec instruction\n")
					err = true
				}
				bytecode = append(bytecode, DEC, regToByte(tokens[i+1].Value))
				offset += 2
				i += 2
			case "and":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for and instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for and instruction\n")
					err = true
				}
				bytecode = append(bytecode, AND, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				offset += 3
				i += 4
			case "or":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for or instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for or instruction\n")
					err = true
				}
				bytecode = append(bytecode, OR, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				offset += 3
				i += 4
			case "xor":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for xor instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for xor instruction\n")
					err = true
				}
				bytecode = append(bytecode, XOR, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				offset += 3
				i += 4
			case "not":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for not instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for not instruction\n")
					err = true
				}
				bytecode = append(bytecode, NOT, regToByte(tokens[i+1].Value))
				offset += 2
				i += 2
			case "shl":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for shl instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.IMM {
					fmt.Fprintf(os.Stderr, "invalid arguments for shl instruction\n")
					err = true
				}
				n := strToInt32(tokens[i+3].Value)
				b := int32ToBytes(n)
				bytecode = append(bytecode, SHL, regToByte(tokens[i+1].Value))
				bytecode = append(bytecode, b...)
				offset += 6
				i += 4
			case "shr":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for shr instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.IMM {
					fmt.Fprintf(os.Stderr, "invalid arguments for shr instruction\n")
					err = true
				}
				n := strToInt32(tokens[i+3].Value)
				b := int32ToBytes(n)
				bytecode = append(bytecode, SHR, regToByte(tokens[i+1].Value))
				bytecode = append(bytecode, b...)
				offset += 6
				i += 4
			case "mov":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for mov instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.IMM {
					fmt.Fprintf(os.Stderr, "invalid arguments for mov instruction\n")
					err = true
				}
				n := strToInt32(tokens[i+3].Value)
				b := int32ToBytes(n)
				bytecode = append(bytecode, MOV, regToByte(tokens[i+1].Value))
				bytecode = append(bytecode, b...)
				offset += 6
				i += 4
			case "load":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for load instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.IMM {
					fmt.Fprintf(os.Stderr, "invalid arguments for load instruction\n")
					err = true
				}
				n := strToInt32(tokens[i+3].Value)
				b := int32ToBytes(n)
				bytecode = append(bytecode, LOAD, regToByte(tokens[i+1].Value))
				bytecode = append(bytecode, b...)
				offset += 6
				i += 4
			case "store":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for store instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.IMM {
					fmt.Fprintf(os.Stderr, "invalid arguments for store instruction\n")
					err = true
				}
				n := strToInt32(tokens[i+3].Value)
				b := int32ToBytes(n)
				bytecode = append(bytecode, STORE, regToByte(tokens[i+1].Value))
				bytecode = append(bytecode, b...)
				offset += 6
				i += 4
			case "push":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for push instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for push instruction\n")
					err = true
				}
				bytecode = append(bytecode, PUSH, regToByte(tokens[i+1].Value))
				offset += 2
				i += 2
			case "pop":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for pop instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for pop instruction\n")
					err = true
				}
				bytecode = append(bytecode, POP, regToByte(tokens[i+1].Value))
				offset += 2
				i += 2
			case "jmp":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for jmp instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.IDENT {
					fmt.Fprintf(os.Stderr, "invalid arguments for jmp instruction\n")
					err = true
				}
				n, ok := labels[tokens[i+1].Value]
				var b []byte
				if ok {
					b = int32ToBytes(int32(n))
				} else {
					b = []byte{0, 0, 0, 0}
					unknown[offset+1] = tokens[i+1].Value
				}
				bytecode = append(bytecode, JMP)
				bytecode = append(bytecode, b...)
				offset += 5
				i += 2
			case "jz":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for jz instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.IDENT {
					fmt.Fprintf(os.Stderr, "invalid arguments for jz instruction\n")
					err = true
				}
				n, ok := labels[tokens[i+1].Value]
				var b []byte
				if ok {
					b = int32ToBytes(int32(n))
				} else {
					b = []byte{0, 0, 0, 0}
					unknown[offset+1] = tokens[i+1].Value
				}
				bytecode = append(bytecode, JZ)
				bytecode = append(bytecode, b...)
				offset += 5
				i += 2
			case "jnz":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for jnz instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.IDENT {
					fmt.Fprintf(os.Stderr, "invalid arguments for jnz instruction\n")
					err = true
				}
				n, ok := labels[tokens[i+1].Value]
				var b []byte
				if ok {
					b = int32ToBytes(int32(n))
				} else {
					b = []byte{0, 0, 0, 0}
					unknown[offset+1] = tokens[i+1].Value
				}
				bytecode = append(bytecode, JNZ)
				bytecode = append(bytecode, b...)
				offset += 5
				i += 2
			case "jg":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for jg instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.IDENT {
					fmt.Fprintf(os.Stderr, "invalid arguments for jg instruction\n")
					err = true
				}
				n, ok := labels[tokens[i+1].Value]
				var b []byte
				if ok {
					b = int32ToBytes(int32(n))
				} else {
					b = []byte{0, 0, 0, 0}
					unknown[offset+1] = tokens[i+1].Value
				}
				bytecode = append(bytecode, JG)
				bytecode = append(bytecode, b...)
				offset += 5
				i += 2
			case "jl":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for jl instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.IDENT {
					fmt.Fprintf(os.Stderr, "invalid arguments for jl instruction\n")
					err = true
				}
				n, ok := labels[tokens[i+1].Value]
				var b []byte
				if ok {
					b = int32ToBytes(int32(n))
				} else {
					b = []byte{0, 0, 0, 0}
					unknown[offset+1] = tokens[i+1].Value
				}
				bytecode = append(bytecode, JL)
				bytecode = append(bytecode, b...)
				offset += 5
				i += 2
			case "call":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for call instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.IDENT {
					fmt.Fprintf(os.Stderr, "invalid arguments for call instruction\n")
					err = true
				}
				n, ok := labels[tokens[i+1].Value]
				var b []byte
				if ok {
					b = int32ToBytes(int32(n))
				} else {
					b = []byte{0, 0, 0, 0}
					unknown[offset+1] = tokens[i+1].Value
				}
				bytecode = append(bytecode, CALL)
				bytecode = append(bytecode, b...)
				offset += 5
				i += 2
			case "ret":
				bytecode = append(bytecode, RET)
				offset++
				i++
			case "cmp":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for cmp instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for cmp instruction\n")
					err = true
				}
				bytecode = append(bytecode, CMP, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				offset += 3
				i += 4
			case "test":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "missing arguments for test instruction\n")
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "invalid arguments for test instruction\n")
					err = true
				}
				bytecode = append(bytecode, TEST, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				offset += 3
				i += 4
			case "nop":
				bytecode = append(bytecode, NOP)
				offset++
				i++
			case "hlt":
				bytecode = append(bytecode, HLT)
				offset++
				i++
			}
		}

		if i < len(tokens) && tokens[i].Kind == lexer.NEWLINE {
			i++
		} else {
			fmt.Fprintf(os.Stderr, "missing new line after instruction\n")
			err = true
		}
	}

	for ref, label := range unknown {
		if addr, ok := labels[label]; ok {
			b := int32ToBytes(int32(addr))
			bytecode[ref] = b[0]
			bytecode[ref+1] = b[1]
			bytecode[ref+2] = b[2]
			bytecode[ref+3] = b[3]
		} else {
			fmt.Fprintf(os.Stderr, "undefined label `%s`\n", label)
			err = true
		}
	}

	if err {
		os.Exit(1)
	}

	return
}
