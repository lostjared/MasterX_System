.section .data
t9: .asciz "The value is: %d\n"
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
.globl print_numbers
print_numbers:
    pushq %rbp
    movq %rsp, %rbp
    subq $192, %rsp
    movq $0, %rcx
    movq $25, %rax
    movq %rax, -8(%rbp)
    movq $100, %rax
    movq %rax, -16(%rbp)
    movq $50, %rax
    movq %rax, -24(%rbp)
    movq -8(%rbp), %rax # x # x
    movq %rax, -32(%rbp)
    movq -16(%rbp), %rax # y # y
    movq %rax, -40(%rbp)
    movq -32(%rbp), %rax # t0 # t0
    imulq -40(%rbp), %rax
    movq %rax, -48(%rbp)
    movq -24(%rbp), %rax # z # z
    movq %rax, -56(%rbp)
    movq -48(%rbp), %rax # t2 # t2
    cqto
    idivq -56(%rbp)
    movq %rax, -64(%rbp)
    movq $2, %rax
    movq %rax, -72(%rbp)
    movq $4, %rax
    movq %rax, -80(%rbp)
    movq -72(%rbp), %rax # t5 # t5
    addq -80(%rbp), %rax
    movq %rax, -88(%rbp)
    movq -64(%rbp), %rax # t4 # t4
    addq -88(%rbp), %rax
    movq %rax, -96(%rbp)
    movq %rax, -104(%rbp)
    leaq t9(%rip), %rax
    movq %rax, -112(%rbp)
    movq -112(%rbp), %rdi # t9 # t9
    movq -104(%rbp), %rsi # value # value
    movq $0, %rax
    call printf
    movq %rax, -120(%rbp)
    movq -104(%rbp), %rax # value # value
    leave
    ret
.globl init
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $80, %rsp
    movq $0, %rcx
    movq $0, %rax
    call print_numbers
    movq %rax, -8(%rbp)
    movq $0, %rax
    movq %rax, -16(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

