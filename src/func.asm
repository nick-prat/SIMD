section .text
    global multiply;

multiply:
    mov rax, rdi
    imul rsi
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
