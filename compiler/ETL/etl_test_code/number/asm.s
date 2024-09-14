.section .data
t14: .quad 0
t10: .quad 1
t9: .asciz "The Value is: %d\n"
t6: .quad 4
t5: .quad 2
z: .quad 50
y: .quad 100
x: .quad 25
.section .text
.globl print_numbers
print_numbers:
    pushq %rbp
    movq %rsp, %rbp
    subq $144, %rsp
    movq $0, %rcx
    movq $25, %rax
    movq %rax, -8(%rbp)
    movq $100, %rax
    movq %rax, -16(%rbp)
    movq $50, %rax
    movq %rax, -24(%rbp)
    movq -8(%rbp), %rax# x
    movq %rax, -32(%rbp)
    movq -16(%rbp), %rax# y
    movq %rax, -40(%rbp)
    movq -32(%rbp), %rax# t0
    imulq -40(%rbp), %rax
    movq %rax, -48(%rbp)
    movq -24(%rbp), %rax# z
    movq %rax, -56(%rbp)
    movq -48(%rbp), %rax# t2
    cqto
    idivq -56(%rbp)
    movq %rax, -64(%rbp)
    movq $2, %rax
    movq %rax, -72(%rbp)
    movq $4, %rax
    movq %rax, -80(%rbp)
    movq -72(%rbp), %rax# t5
    addq -80(%rbp), %rax
    movq %rax, -88(%rbp)
    movq -64(%rbp), %rax# t4
    addq -88(%rbp), %rax
    movq %rax, -96(%rbp)
    movq -96(%rbp), %rax# t8
    movq %rax, -104(%rbp)
    leaq t9(%rip), %rax
    movq %rax, -112(%rbp)
    movq $1, %rax
    movq %rax, -120(%rbp)
    movq -104(%rbp), %rax# value
    addq -120(%rbp), %rax
    movq %rax, -128(%rbp)
    movq -112(%rbp), %rdi# t9
    movq -128(%rbp), %rsi# t11
    movq $0, %rax
    pushq %rcx
    call printf
    movq %rax, -136(%rbp)
    popq %rcx
    movq -104(%rbp), %rax# value
    leave
    ret
.globl init
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movq $0, %rcx
    movq $0, %rax
    pushq %rcx
    call print_numbers
    movq %rax, -8(%rbp)
    popq %rcx
    movq $0, %rax
    movq %rax, -16(%rbp)
    movq -16(%rbp), %rax# t14
    leave
    ret

