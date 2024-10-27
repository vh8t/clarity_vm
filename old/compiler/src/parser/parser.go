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
	MOV_ADDR
	MOV_REG
	MOV_FROM_ADDR
	MOV_FROM_REG
	MOV_INT
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

var MAJOR byte = 1
var MINOR byte = 1

type Def struct {
	Addr int
	Type string
}

func Parse(tokens []lexer.Token) (bytecode []byte) {
	var err bool
	var bcOffset, pcOffset int
	var dataOffset int

	directives := make(map[string]string)
	unknown := make(map[int]string)
	labels := make(map[string]int)

	unknownDefinition := make(map[int]string)
	definitions := make(map[string]Def)

	data := []byte{}

	var section string

	for i := 0; i < len(tokens); {
		token := tokens[i]

		switch token.Kind {
		case lexer.DIRECTIVE:
			directive := token.Value
			switch strings.ToLower(directive) {
			case "global":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d global directive missing label\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.IDENT {
					fmt.Fprintf(os.Stderr, "%d:%d invalid argument for global directive\n", tokens[i+1].Row, tokens[i+1].Col)
					err = true
					i = len(tokens)
					continue
				}
				directives[directive] = tokens[i+1].Value
				i += 2
			case "section":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d section directive missing lable\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.IDENT {
					fmt.Fprintf(os.Stderr, "%d:%d invalid argument for section directive\n", tokens[i+1].Row, tokens[i+1].Col)
					err = true
					i = len(tokens)
					continue
				}
				switch tokens[i+1].Value {
				case "text", "data":
				default:
					fmt.Fprintf(os.Stderr, "%d:%d unknown section %s\n", tokens[i+1].Row, tokens[i+1].Col, tokens[i+1].Value)
					err = true
					i = len(tokens)
					continue
				}
				section = tokens[i+1].Value
				i += 2
			}
		case lexer.LABEL:
			if _, ok := labels[token.Value]; ok {
				fmt.Fprintf(os.Stderr, "%d:%d duplicate label: `%s`\n", token.Row, token.Col, token.Value)
				err = true
				i = len(tokens)
				continue
			} else {
				labels[token.Value] = pcOffset
			}
			i++
		case lexer.DEFINITION:
			if section != "data" {
				fmt.Fprintf(os.Stderr, "%d:%d illegal definition in section %s\n", token.Row, token.Col, section)
				err = true
				i = len(tokens)
				continue
			}
			if i+2 >= len(tokens) {
				fmt.Fprintf(os.Stderr, "%d:%d missing arguments for definition\n", token.Row, token.Col)
				err = true
				i = len(tokens)
				continue
			}
			if tokens[i+1].Kind != lexer.IDENT {
				fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for definition\n", token.Row, token.Col)
				err = true
				i = len(tokens)
				continue
			}
			name := tokens[i+1].Value
			switch token.Value {
			case "byte":
				if tokens[i+2].Kind != lexer.IMM {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for definition\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				n := strToInt32(tokens[i+2].Value)
				if 0 > n || n > 255 {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for definition\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				data = append(data, byte(n))
				if _, ok := definitions[name]; ok {
					fmt.Fprintf(os.Stderr, "%d:%d duplicate definition: `%s`\n", token.Row, token.Col, token.Value)
					err = true
					i = len(tokens)
					continue
				} else {
					definitions[name] = Def{dataOffset, "byte"}
				}
				dataOffset++
				i += 3
			case "bytes":
				var bytes []byte
				if tokens[i+2].Kind != lexer.IMM {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for definition\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				n := strToInt32(tokens[i+2].Value)
				if 0 > n || n > 255 {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for definition\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytes = append(bytes, byte(n))
				i += 3
				for i+1 < len(tokens) && tokens[i].Kind == lexer.COMMA {
					if tokens[i+1].Kind != lexer.IMM {
						fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for definition\n", token.Row, token.Col)
						err = true
						i = len(tokens)
						continue
					}
					n := strToInt32(tokens[i+1].Value)
					if 0 > n || n > 255 {
						fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for definition\n", token.Row, token.Col)
						err = true
						i = len(tokens)
						continue
					}
					i += 2
					bytes = append(bytes, byte(n))
				}
				i++
				data = append(data, bytes...)
				if _, ok := definitions[name]; ok {
					fmt.Fprintf(os.Stderr, "%d:%d duplicate definition: `%s`\n", token.Row, token.Col, token.Value)
					err = true
					i = len(tokens)
					continue
				} else {
					definitions[name] = Def{dataOffset, "bytes"}
				}
				dataOffset += len(bytes)
			case "string":
				if tokens[i+2].Kind != lexer.STRING {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for definition\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytes := []byte(tokens[i+2].Value)
				bytes = append(bytes, 0)
				data = append(data, bytes...)
				if _, ok := definitions[name]; ok {
					fmt.Fprintf(os.Stderr, "%d:%d duplicate definition: `%s`\n", token.Row, token.Col, token.Value)
					err = true
					i = len(tokens)
					continue
				} else {
					definitions[name] = Def{dataOffset, "string"}
				}
				dataOffset += len(bytes)
				i += 3
			case "int":
				if tokens[i+2].Kind != lexer.IMM {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for definition\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytes := int32ToBytes(strToInt32(tokens[i+2].Value))
				data = append(data, bytes...)
				if _, ok := definitions[name]; ok {
					fmt.Fprintf(os.Stderr, "%d:%d duplicate definition: `%s`\n", token.Row, token.Col, token.Value)
					err = true
					i = len(tokens)
					continue
				} else {
					definitions[name] = Def{dataOffset, "int"}
				}
				dataOffset += len(bytes)
				i += 3
			default:
				fmt.Fprintf(os.Stderr, "%d:%d unknown data type `%s`\n", token.Row, token.Col, token.Value)
				err = true
				i = len(tokens)
				continue
			}
		case lexer.INSTR:
			if section != "text" {
				fmt.Fprintf(os.Stderr, "%d:%d illegal instruction in section %s\n", token.Row, token.Col, section)
				err = true
				i = len(tokens)
				continue
			}
			switch strings.ToLower(token.Value) {
			case "add":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for add instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for add instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, ADD, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				bcOffset += 3
				i += 4
			case "sub":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for sub instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for sub instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, SUB, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				bcOffset += 3
				i += 4
			case "mul":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for mul instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for mul instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, MUL, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				bcOffset += 3
				i += 4
			case "div":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for div instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for div instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, DIV, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				bcOffset += 3
				i += 4
			case "mod":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for mod instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for mod instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, MOD, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				bcOffset += 3
				i += 4
			case "inc":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for inc instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for inc instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, INC, regToByte(tokens[i+1].Value))
				bcOffset += 2
				i += 2
			case "dec":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for dec instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for dec instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, DEC, regToByte(tokens[i+1].Value))
				bcOffset += 2
				i += 2
			case "and":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for and instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for and instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, AND, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				bcOffset += 3
				i += 4
			case "or":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for or instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for or instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, OR, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				bcOffset += 3
				i += 4
			case "xor":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for xor instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for xor instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, XOR, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				bcOffset += 3
				i += 4
			case "not":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for not instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for not instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, NOT, regToByte(tokens[i+1].Value))
				bcOffset += 2
				i += 2
			case "shl":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for shl instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.IMM {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for shl instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				n := strToInt32(tokens[i+3].Value)
				b := int32ToBytes(n)
				bytecode = append(bytecode, SHL, regToByte(tokens[i+1].Value))
				bytecode = append(bytecode, b...)
				bcOffset += 6
				i += 4
			case "shr":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for shr instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.IMM {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for shr instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				n := strToInt32(tokens[i+3].Value)
				b := int32ToBytes(n)
				bytecode = append(bytecode, SHR, regToByte(tokens[i+1].Value))
				bytecode = append(bytecode, b...)
				bcOffset += 6
				i += 4
			case "mov":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for mov instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for mov instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				switch tokens[i+3].Kind {
				case lexer.IMM:
					n := strToInt32(tokens[i+3].Value)
					b := int32ToBytes(n)
					bytecode = append(bytecode, MOV, regToByte(tokens[i+1].Value))
					bytecode = append(bytecode, b...)
					bcOffset += 6
					i += 4
				case lexer.REG:
					bytecode = append(bytecode, MOV_REG, regToByte(tokens[i+1].Value), regToByte(tokens[i+2].Value))
					bcOffset += 3
					i += 4
				case lexer.IDENT:
					if addr, ok := definitions[tokens[i+3].Value]; ok {
						b := int32ToBytes(int32(addr.Addr))
						bytecode = append(bytecode, MOV_ADDR, regToByte(tokens[i+1].Value))
						bytecode = append(bytecode, b...)
					} else {
						bytecode = append(bytecode, MOV_ADDR, regToByte(tokens[i+1].Value), 0, 0, 0, 0)
						unknownDefinition[bcOffset+2] = tokens[i+3].Value
					}
					bcOffset += 6
					i += 4
				case lexer.LBRACKET:
					if i+5 >= len(tokens) {
						fmt.Fprintf(os.Stderr, "%d:%d missing arguments for mov instruction\n", token.Row, token.Col)
						err = true
						i = len(tokens)
						continue
					}
					if tokens[i+5].Kind != lexer.RBRACKET {
						fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for mov instruction\n", token.Row, token.Col)
						err = true
						i = len(tokens)
						continue
					}
					switch tokens[i+4].Kind {
					case lexer.REG:
						bytecode = append(bytecode, MOV_FROM_REG, regToByte(tokens[i+1].Value), regToByte(tokens[i+4].Value))
						bcOffset += 3
					case lexer.IDENT:
						if addr, ok := definitions[tokens[i+4].Value]; ok {
							b := int32ToBytes(int32(addr.Addr))
							if addr.Type == "int" {
								bytecode = append(bytecode, MOV_INT, regToByte(tokens[i+1].Value))
							} else {
								bytecode = append(bytecode, MOV_FROM_ADDR, regToByte(tokens[i+1].Value))
							}
							bytecode = append(bytecode, b...)
						} else {
							bytecode = append(bytecode, MOV_FROM_ADDR, regToByte(tokens[i+1].Value), 0, 0, 0, 0)
							unknownDefinition[bcOffset+2] = tokens[i+3].Value
						}
						bcOffset += 6
					default:
						fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for mov instruction\n", token.Row, token.Col)
						err = true
						i = len(tokens)
						continue
					}
					i += 6
				default:
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for mov instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
			case "load":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for load instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.IMM {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for load instruction\n", token.Row, token.Col)
					err = true
				}
				n := strToInt32(tokens[i+3].Value)
				b := int32ToBytes(n)
				bytecode = append(bytecode, LOAD, regToByte(tokens[i+1].Value))
				bytecode = append(bytecode, b...)
				bcOffset += 6
				i += 4
			case "store":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for store instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.IMM {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for store instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				n := strToInt32(tokens[i+3].Value)
				b := int32ToBytes(n)
				bytecode = append(bytecode, STORE, regToByte(tokens[i+1].Value))
				bytecode = append(bytecode, b...)
				bcOffset += 6
				i += 4
			case "push":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for push instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for push instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, PUSH, regToByte(tokens[i+1].Value))
				bcOffset += 2
				i += 2
			case "pop":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for pop instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for pop instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, POP, regToByte(tokens[i+1].Value))
				bcOffset += 2
				i += 2
			case "jmp":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for jmp instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.IDENT {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for jmp instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				n, ok := labels[tokens[i+1].Value]
				var b []byte
				if ok {
					b = int32ToBytes(int32(n))
				} else {
					b = []byte{0, 0, 0, 0}
					unknown[bcOffset+1] = tokens[i+1].Value
				}
				bytecode = append(bytecode, JMP)
				bytecode = append(bytecode, b...)
				bcOffset += 5
				i += 2
			case "jz":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for jz instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.IDENT {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for jz instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				n, ok := labels[tokens[i+1].Value]
				var b []byte
				if ok {
					b = int32ToBytes(int32(n))
				} else {
					b = []byte{0, 0, 0, 0}
					unknown[bcOffset+1] = tokens[i+1].Value
				}
				bytecode = append(bytecode, JZ)
				bytecode = append(bytecode, b...)
				bcOffset += 5
				i += 2
			case "jnz":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for jnz instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.IDENT {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for jnz instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				n, ok := labels[tokens[i+1].Value]
				var b []byte
				if ok {
					b = int32ToBytes(int32(n))
				} else {
					b = []byte{0, 0, 0, 0}
					unknown[bcOffset+1] = tokens[i+1].Value
				}
				bytecode = append(bytecode, JNZ)
				bytecode = append(bytecode, b...)
				bcOffset += 5
				i += 2
			case "jg":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for jg instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.IDENT {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for jg instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				n, ok := labels[tokens[i+1].Value]
				var b []byte
				if ok {
					b = int32ToBytes(int32(n))
				} else {
					b = []byte{0, 0, 0, 0}
					unknown[bcOffset+1] = tokens[i+1].Value
				}
				bytecode = append(bytecode, JG)
				bytecode = append(bytecode, b...)
				bcOffset += 5
				i += 2
			case "jl":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for jl instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.IDENT {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for jl instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				n, ok := labels[tokens[i+1].Value]
				var b []byte
				if ok {
					b = int32ToBytes(int32(n))
				} else {
					b = []byte{0, 0, 0, 0}
					unknown[bcOffset+1] = tokens[i+1].Value
				}
				bytecode = append(bytecode, JL)
				bytecode = append(bytecode, b...)
				bcOffset += 5
				i += 2
			case "call":
				if i+1 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for call instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.IDENT {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for call instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				n, ok := labels[tokens[i+1].Value]
				var b []byte
				if ok {
					b = int32ToBytes(int32(n))
				} else {
					b = []byte{0, 0, 0, 0}
					unknown[bcOffset+1] = tokens[i+1].Value
				}
				bytecode = append(bytecode, CALL)
				bytecode = append(bytecode, b...)
				bcOffset += 5
				i += 2
			case "ret":
				bytecode = append(bytecode, RET)
				bcOffset++
				i++
			case "cmp":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for cmp instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for cmp instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, CMP, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				bcOffset += 3
				i += 4
			case "test":
				if i+3 >= len(tokens) {
					fmt.Fprintf(os.Stderr, "%d:%d missing arguments for test instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				if tokens[i+1].Kind != lexer.REG || tokens[i+2].Kind != lexer.COMMA || tokens[i+3].Kind != lexer.REG {
					fmt.Fprintf(os.Stderr, "%d:%d invalid arguments for test instruction\n", token.Row, token.Col)
					err = true
					i = len(tokens)
					continue
				}
				bytecode = append(bytecode, TEST, regToByte(tokens[i+1].Value), regToByte(tokens[i+3].Value))
				bcOffset += 3
				i += 4
			case "nop":
				bytecode = append(bytecode, NOP)
				bcOffset++
				i++
			case "hlt":
				bytecode = append(bytecode, HLT)
				bcOffset++
				i++
			}
			pcOffset++
		}

		if tokens[i].Kind == lexer.NEWLINE {
			i++
		} else {
			fmt.Fprintf(os.Stderr, "%s %s %s\n", tokens[i-1].Kind, tokens[i].Kind, tokens[i+1].Kind)
			fmt.Fprintf(os.Stderr, "%d:%d missing new line after instruction\n", token.Row, token.Col)
			err = true
			i = len(tokens)
			continue
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

	for ref, label := range unknownDefinition {
		if addr, ok := definitions[label]; ok {
			if bytecode[ref-2] == MOV_FROM_ADDR && addr.Type == "int" {
				bytecode[ref-2] = MOV_INT
			}
			b := int32ToBytes(int32(addr.Addr))
			bytecode[ref] = b[0]
			bytecode[ref+1] = b[1]
			bytecode[ref+2] = b[2]
			bytecode[ref+3] = b[3]
		} else {
			fmt.Fprintf(os.Stderr, "undefined definition `%s`\n", label)
			err = true
		}
	}

	if err {
		os.Exit(1)
	}

	start := 0
	if label, ok := directives["global"]; ok {
		if addr, ok := labels[label]; ok {
			start = addr
		}
	}

	pcStart := int32ToBytes(int32(start))
	bcStart := int32ToBytes(int32(len(data)))

	header := []byte{0xc1, 0xa0, MAJOR, MINOR}
	header = append(header, bcStart...)
	header = append(header, pcStart...)
	header = append(header, data...)
	bytecode = append(header, bytecode...)

	return
}
