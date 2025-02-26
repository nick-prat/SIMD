section .text
    global multiply;
    global simd_filter;

multiply:
    mov rax, rdi
    imul rsi
    ret

simd_filter:
    push rbx
    mov r13, 0 ; r13 -> R counter
hloop:
    mov rax, [rdi + r13 * 8]
    mov rbx, [rsi + r13 * 8] 

    mov r14, 0 ; r14 -> C counter
wloop:
    mov r11d, [rax + r14 * 4] 
    movq xmm0, [rax + r14 * 4]
    punpcklbw xmm0, xmm0

    mov r15, 0 ; r15 -> Ch counter
cloop:
    ; 32-bit Pixel is in r11d (ABGR)
    and r11, 0xFF808080

    inc r15 ; end of cloop
    cmp r15, 4
    jl cloop

    ; Load pixel into dst pointer
    mov [rbx + r14 * 4], r11d

    inc r14 ; end of wloop
    cmp r14, rdx
    jl wloop

    inc r13 ; end of hloop
    cmp r13, rcx
    jl hloop
end:
    pop rbx
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
