package parser

import (
	"encoding/binary"
	"strconv"
	"strings"
)

func regToByte(reg string) byte {
	switch strings.ToLower(reg) {
	case "r0":
		return 0
	case "r1":
		return 1
	case "r2":
		return 2
	case "r3":
		return 3
	case "r4":
		return 4
	case "r5":
		return 5
	case "r6":
		return 6
	case "r7":
		return 7
	}
	return 0
}

func strToInt32(s string) int32 {
	i64, err := strconv.ParseInt(s, 10, 32)
	if err != nil {
		panic(err)
	}
	return int32(i64)
}

func int32ToBytes(n int32) []byte {
	bytes := make([]byte, 4)
	binary.LittleEndian.PutUint32(bytes, uint32(n))
	return bytes
}
