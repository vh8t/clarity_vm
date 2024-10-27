.section data
  int number 65535
  string message "Hello, World!\n"
  byte b 0x01
  bytes bs 0x01, 2, 0b11

.section text
  .global _start

_start:
  mov r0, [number]
  mov r1, bs
  inc r1
  mov r1, [r1]
  mov r2, [message]
  hlt
