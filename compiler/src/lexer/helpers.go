package lexer

import "strings"

func isInstruction(s string) bool {
	inst := []string{
		"add", "sub", "mul", "div", "mod", "inc", "dec",
		"and", "or", "xor", "not", "shl", "shr",
		"mov", "load", "store", "push", "pop",
		"jmp", "jz", "jnz", "jg", "jl",
		"call", "ret", "cmp", "test",
		"nop", "hlt",
	}

	for _, i := range inst {
		if i == strings.ToLower(s) {
			return true
		}
	}
	return false
}

func isDefinition(s string) bool {
	defs := []string{
		"byte", "bytes", "string",
	}

	for _, i := range defs {
		if i == strings.ToLower(s) {
			return true
		}
	}
	return false
}

func isRegister(s string) bool {
	if len(s) != 2 {
		return false
	}

	if s[0] != 'r' && s[0] != 'R' {
		return false
	}

	if '0' > s[1] || s[1] > '7' {
		return false
	}

	return true
}

func isalpha(char byte) bool {
	return 'a' <= char && char <= 'z' || 'A' <= char && char <= 'Z'
}

func isdigit(char byte) bool {
	return '0' <= char && char <= '9'
}

func isalnum(char byte) bool {
	return isalpha(char) || isdigit(char)
}
