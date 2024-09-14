.section .data
t6: .asciz "value is here"
t5: .asciz "Len value:"
t0: .asciz "hey: "
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
.globl testLen
testLen:
    pushq %rbp
    movq %rsp, %rbp
    subq $128, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    leaq t0(%rip), %rax
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rax # what # what
    movq %rax, -24(%rbp)
    movq $0, %rcx
    movq %rcx, -32(%rbp)
    addq $8, -32(%rbp)
    movq -24(%rbp), %rdi # t1 # t1
    call strlen # t1
    addq %rax, -32(%rbp)
    addq $1, -32(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -32(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -40(%rbp)
    movq -16(%rbp), %rsi # t0 # t0
    call strcpy
    movq -24(%rbp), %rsi # t1 # t1
    call strcat
    movq -40(%rbp), %rax # t2 # t2
    movq %rax, -48(%rbp)
    movq -48(%rbp), %rdi # n_s # n_s
    movq $0, %rax
    call puts
    movq %rax, -56(%rbp)
    movq -48(%rbp), %rdi # n_s # n_s
    movq $0, %rax
    call len
    movq %rax, -64(%rbp)
    movq -40(%rbp), %rdi # t2 # t2
    call free # local variable: t2
    movq -64(%rbp), %rax # t4 # t4
    leave
    ret
.globl init
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $128, %rsp
    movq $0, %rcx
    leaq t5(%rip), %rax
    movq %rax, -8(%rbp)
    leaq t6(%rip), %rax
    movq %rax, -16(%rbp)
    movq -16(%rbp), %rdi # t6 # t6
    movq $0, %rax
    call testLen
    movq %rax, -24(%rbp)
    movq -24(%rbp), %rdi # t7 # t7
    movq $0, %rax
    call str
    movq %rax, -32(%rbp)
    addq $22, -40(%rbp)
    movq $0, %rcx
    movq %rcx, -40(%rbp)
    addq $13, -40(%rbp)
    addq $1, -40(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -40(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -48(%rbp)
    movq -8(%rbp), %rsi # t5 # t5
    call strcpy
    movq -32(%rbp), %rsi # t8 # t8
    call strcat
    movq -48(%rbp), %rdi # t9 # t9
    movq $0, %rax
    call puts
    movq %rax, -56(%rbp)
    movq $0, %rax
    movq %rax, -64(%rbp)
    movq -48(%rbp), %rdi # t9 # t9
    call free # local variable: t9
    movq -64(%rbp), %rax # t11 # t11
    leave
    ret
.section .note.GNU-stack,"",@progbits

