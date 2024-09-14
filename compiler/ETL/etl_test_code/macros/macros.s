.section .data
t0: .asciz "Hey this is a number: "
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
    subq $128, %rsp
    movq $0, %rcx
    leaq t0(%rip), %rax
    movq %rax, -8(%rbp)
    movq $10, %rax
    movq %rax, -16(%rbp)
    movq $10, %rax
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rax # t1 # t1
    imulq -24(%rbp), %rax
    movq %rax, -32(%rbp)
    movq -32(%rbp), %rdi # t3 # t3
    movq $0, %rax
    call str
    movq %rax, -40(%rbp)
    addq $22, -48(%rbp)
    movq $0, %rcx
    movq %rcx, -48(%rbp)
    addq $25, -48(%rbp)
    addq $1, -48(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -48(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -56(%rbp)
    movq -8(%rbp), %rsi # t0 # t0
    call strcpy
    movq -40(%rbp), %rsi # t4 # t4
    call strcat
    movq -56(%rbp), %rdi # t5 # t5
    movq $0, %rax
    call puts
    movq %rax, -64(%rbp)
    movq $0, %rax
    movq %rax, -72(%rbp)
    movq -56(%rbp), %rdi # t5 # t5
    call free # local variable: t5
    movq -72(%rbp), %rax # t7 # t7
    leave
    ret
.section .note.GNU-stack,"",@progbits

