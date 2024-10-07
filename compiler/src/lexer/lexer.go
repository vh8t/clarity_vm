package lexer

import (
	"fmt"
	"os"
)

type TokenKind string

type Token struct {
	Value string
	Kind  TokenKind
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

	for i := 0; i < len(source); {
		char := source[i]
		var buf string

		switch {
		case isalpha(char) || char == '_':
			for isalnum(char) || char == '_' {
				buf += string(char)
				i++
				if i >= len(source) {
					break
				}
				char = source[i]
			}

			if char == ':' {
				i++
				tokens = append(tokens, Token{
					Value: buf,
					Kind:  LABEL,
				})
			} else if isInstruction(buf) {
				tokens = append(tokens, Token{
					Value: buf,
					Kind:  INSTR,
				})
			} else if isRegister(buf) {
				tokens = append(tokens, Token{
					Value: buf,
					Kind:  REG,
				})
			} else {
				tokens = append(tokens, Token{
					Value: buf,
					Kind:  IDENT,
				})
			}
		case isdigit(char):
			for isdigit(char) {
				buf += string(char)
				i++
				if i >= len(source) {
					break
				}
				char = source[i]
			}

			tokens = append(tokens, Token{
				Value: buf,
				Kind:  IMM,
			})
		case char == '.':
			i++
			if i >= len(source) {
				continue
			}
			char = source[i]
			for isalpha(char) {
				buf += string(char)
				i++
				if i >= len(source) {
					break
				}
				char = source[i]
			}

			tokens = append(tokens, Token{
				Value: buf,
				Kind:  DIRECTIVE,
			})
		case char == ',':
			tokens = append(tokens, Token{
				Value: "",
				Kind:  COMMA,
			})
			i++
		case char == '\n':
			tokens = append(tokens, Token{
				Value: "",
				Kind:  NEWLINE,
			})
			i++
		case char == ' ' || char == '\t' || char == '\r':
			i++
		default:
			fmt.Fprintf(os.Stderr, "unknown character while lexing: `%c` (0x%02x)\n", char, char)
			err = true
		}
	}

	if err {
		os.Exit(1)
	}

	return
}
