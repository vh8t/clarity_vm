package lexer

import (
	"fmt"
	"os"
)

type TokenKind string

type Token struct {
	Value    string
	Kind     TokenKind
	Row, Col int
}

const (
	DIRECTIVE TokenKind = "DIRECTIVE"
	NEWLINE   TokenKind = "NEWLINE"
	LABEL     TokenKind = "LABEL"
	INSTR     TokenKind = "INSTR"
	IDENT     TokenKind = "IDENT"
	COMMA     TokenKind = "COMMA"
	REG       TokenKind = "REG"
	IMM       TokenKind = "IMM"
)

func Tokenize(source []byte) (tokens []Token) {
	var err bool
	row, col := 1, 1

	for i := 0; i < len(source); {
		char := source[i]
		var buf string

		sRow, sCol := row, col
		switch {
		case isalpha(char) || char == '_':
			for isalnum(char) || char == '_' {
				buf += string(char)
				i++
				col++
				if i >= len(source) {
					break
				}
				char = source[i]
			}

			if char == ':' {
				i++
				col++
				tokens = append(tokens, Token{
					Value: buf,
					Kind:  LABEL,
					Row:   sRow,
					Col:   sCol,
				})
			} else if isInstruction(buf) {
				tokens = append(tokens, Token{
					Value: buf,
					Kind:  INSTR,
					Row:   sRow,
					Col:   sCol,
				})
			} else if isRegister(buf) {
				tokens = append(tokens, Token{
					Value: buf,
					Kind:  REG,
					Row:   sRow,
					Col:   sCol,
				})
			} else {
				tokens = append(tokens, Token{
					Value: buf,
					Kind:  IDENT,
					Row:   sRow,
					Col:   sCol,
				})
			}
		case isdigit(char):
			if char == '0' && i+2 < len(source) && source[i+1] == 'x' {
				buf += "0x"
				i += 2
				col += 2
				char = source[i]
				for isdigit(char) || ('a' <= char && char <= 'f') {
					buf += string(char)
					i++
					col++
					if i >= len(source) {
						break
					}
					char = source[i]
				}
			} else {
				for isdigit(char) {
					buf += string(char)
					i++
					col++
					if i >= len(source) {
						break
					}
					char = source[i]
				}
			}

			tokens = append(tokens, Token{
				Value: buf,
				Kind:  IMM,
				Row:   sRow,
				Col:   sCol,
			})
		case char == '.':
			i++
			col++
			if i >= len(source) {
				continue
			}
			char = source[i]
			for isalpha(char) {
				buf += string(char)
				i++
				col++
				if i >= len(source) {
					break
				}
				char = source[i]
			}

			tokens = append(tokens, Token{
				Value: buf,
				Kind:  DIRECTIVE,
				Row:   sRow,
				Col:   sCol,
			})
		case char == ',':
			tokens = append(tokens, Token{
				Value: "",
				Kind:  COMMA,
				Row:   sRow,
				Col:   sCol,
			})
			i++
			col++
		case char == '\n':
			tokens = append(tokens, Token{
				Value: "",
				Kind:  NEWLINE,
				Row:   sRow,
				Col:   sCol,
			})
			i++
			col = 1
			row++
		case char == ';':
			for char != '\n' {
				i++
				col++
				if i >= len(source) {
					break
				}
				char = source[i]
			}
		case char == ' ' || char == '\t' || char == '\r':
			i++
			col++
		default:
			i++
			fmt.Fprintf(os.Stderr, "unknown character while lexing: `%c` (0x%02x)\n", char, char)
			err = true
		}
	}

	if err {
		os.Exit(1)
	}

	return
}
