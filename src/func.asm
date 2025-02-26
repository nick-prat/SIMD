section .text
    global simd_darken_filter

simd_darken_filter:
    push rbx
    pxor xmm1, xmm1 ; zero-d register to expand xmm0
    mov rbx, 0x0000FFFFFFFFFFFF; shift values for image filter
    movq xmm2, rbx
    movlhps xmm2, xmm2

    mov r13, 0 ; r13 -> R counter
hloop:
    mov rax, [rdi + r13 * 8]
    mov rbx, [rsi + r13 * 8] 

    mov r14, 0 ; r14 -> C counter
wloop:
    movups xmm0, [rax + r14 * 4] ; Load 4 pixels
    vpunpcklbw xmm3, xmm0, xmm1 ; Extend lower 8-bit pixels into 16-bit pixels
    vpunpckhbw xmm4, xmm0, xmm1 ; Extend higher 8-bit pixels into 16-bit pixels

    vpand xmm5, xmm2, xmm3
    psrlw xmm5, 1
    vpandn xmm6, xmm2, xmm3
    vpor xmm3, xmm5, xmm6 

    vpand xmm5, xmm2, xmm4
    psrlw xmm5, 1
    vpandn xmm6, xmm2, xmm4
    vpor xmm4, xmm5, xmm6 

    vpackuswb xmm0, xmm3, xmm4 ; Pack 16-bit pixels back to 8-bit pixels w/ saturation
    movups [rbx + r14 * 4], xmm0 ; Store 4 pixels from xmm0 into dst

    add r14, 4 ; end of wloop
    cmp r14, rdx
    jl wloop

    inc r13 ; end of hloop
    cmp r13, rcx
    jl hloop
end:
    pop rbx
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
