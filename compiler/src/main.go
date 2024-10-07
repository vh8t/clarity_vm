package main

import (
	"compiler/src/lexer"
	"compiler/src/parser"
	"fmt"
	"os"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintf(os.Stderr, "Usage: %s <file>\n", os.Args[0])
		os.Exit(1)
	}

	contents, err := os.ReadFile(os.Args[1])
	if err != nil {
		fmt.Fprintf(os.Stderr, "could not read file %s: %v\n", os.Args[1], err)
		os.Exit(1)
	}

	tokens := lexer.Tokenize(contents)
	bytecode := parser.Parse(tokens)

	os.WriteFile("out.bin", bytecode, 0644)
}
