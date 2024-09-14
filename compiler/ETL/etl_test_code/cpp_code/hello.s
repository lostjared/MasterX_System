.section .data
t1: .asciz "Goodbye!\n"
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
    movq $0, %rax
    call hello
    movq %rax, -8(%rbp)
    leaq t1(%rip), %rax
    movq %rax, -16(%rbp)
    movq -16(%rbp), %rdi # t1 # t1
    movq $0, %rax
    call printf
    movq %rax, -24(%rbp)
    movq $0, %rax
    movq %rax, -32(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

