.section .data
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
    subq $80, %rsp
    movq $0, %rcx
    movq $0, %rax
    call hello
    movq %rax, -8(%rbp)
    movq $0, %rax
    movq %rax, -16(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

