.section data
    string msg "Hello, World!\n"

.section text
    .global _start

; r0 - counter
; r1 - arg1 string
; r2 - char
; r3 - string terminator
strlen:
    xor r0, r0
    xor r3, r3
strlen_start:
    mov r2, [r1]
    cmp r2, r3
    jz strlen_end
    inc r0
    inc r1
    jmp strlen_start
strlen_end:
    ret

_start:
    mov r1, msg
    call strlen
    hlt
