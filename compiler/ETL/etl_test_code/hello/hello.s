.section .data
t0: .asciz "Hello,$### \" World!"
.section .text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    call init
    movq $0, %rax
    leave
    ret
.globl init
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $96, %rsp
    movq $0, %rcx
    leaq t0(%rip), %rax
    movq %rax, -8(%rbp)
    movq -8(%rbp), %rdi # t0 # t0
    movq $0, %rax
    call puts
    movq %rax, -16(%rbp)
    movq $0, %rax
    movq %rax, -24(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

