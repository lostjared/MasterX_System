.section .data
t2: .quad 0
t0: .asciz "Hello, World!"
.section .text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    call init
    movq %rax, %rbx
    leave
    movq %rbx, %rax
    ret
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movq $0, %rcx
    leaq t0(%rip), %rax
    movq %rax, -8(%rbp)
    movq -8(%rbp), %rdi# t0
    movq $0, %rax
    pushq %rcx
    call puts
    movq %rax, -16(%rbp)
    popq %rcx
    movq $0, %rax
    movq %rax, -24(%rbp)
    movq -24(%rbp), %rax# t2
    leave
    ret

