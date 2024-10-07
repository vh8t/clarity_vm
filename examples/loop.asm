    xor r0, r0
    mov r1, 10
loop:
    cmp r0, r1
    jz end
    inc r0
    jmp loop
end:
    hlt
