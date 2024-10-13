.section text
    .global _start

_start:
    mov r0, 0xa
    mov r1, 0x14
    add r0, r1
    hlt
