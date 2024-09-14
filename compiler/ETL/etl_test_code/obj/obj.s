.section .data
t7: .asciz "%d * %d / 100 = %d\n"
t0: .asciz "Hello, World"
.section .text
.globl program
program:
    pushq %rbp
    movq %rsp, %rbp
    subq $192, %rsp
    movq $0, %rcx
    leaq t0(%rip), %rax
    movq %rax, -8(%rbp)
    movq -8(%rbp), %rdi # t0 # t0
    movq $0, %rax
    call puts
    movq %rax, -16(%rbp)
    movq $25, %rax
    movq %rax, -24(%rbp)
    movq $25, %rax
    movq %rax, -32(%rbp)
    movq -24(%rbp), %rax # x # x
    movq %rax, -40(%rbp)
    movq -32(%rbp), %rax # y # y
    movq %rax, -48(%rbp)
    movq -40(%rbp), %rax # t2 # t2
    imulq -48(%rbp), %rax
    movq %rax, -56(%rbp)
    movq $100, %rax
    movq %rax, -64(%rbp)
    movq -56(%rbp), %rax # t4 # t4
    cqto
    idivq -64(%rbp)
    movq %rax, -72(%rbp)
    movq %rax, -80(%rbp)
    leaq t7(%rip), %rax
    movq %rax, -88(%rbp)
    movq -24(%rbp), %rax # x # x
    movq %rax, -96(%rbp)
    movq -32(%rbp), %rax # y # y
    movq %rax, -104(%rbp)
    movq -88(%rbp), %rdi # t7 # t7
    movq -96(%rbp), %rsi # t8 # t8
    movq -104(%rbp), %rdx # t9 # t9
    movq -80(%rbp), %rcx # z # z
    movq $0, %rax
    call printf
    movq %rax, -112(%rbp)
    movq $0, %rax
    movq %rax, -120(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

