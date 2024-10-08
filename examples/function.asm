.global _start

loop:
    xor r2, r2
loop_start:
    cmp r1, r2
    jz loop_end
    inc r2
    jmp loop_start
loop_end:
    ret

_start:
    mov r1, 5  ;; amout of loops
    call loop
    hlt
