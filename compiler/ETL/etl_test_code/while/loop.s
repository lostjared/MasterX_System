.section .data
t8: .asciz "Letter is: %c at index: %d\n"
x: .asciz "Hello, World"
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
    subq $208, %rsp
    movq $0, %rcx
    leaq x(%rip), %rax
    movq %rax, -8(%rbp)
    movq $0, %rax
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rax # x # x
    movq %rax, -24(%rbp)
    movq -24(%rbp), %rdi # t0 # t0
    movq $0, %rax
    call strlen
    movq %rax, -32(%rbp)
    movq %rax, -40(%rbp)
while_start_2: 
    movq -16(%rbp), %rax # i # i
    movq %rax, -48(%rbp)
    movq -40(%rbp), %rbx # length # length
    cmpq %rbx, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -56(%rbp)
    movq -56(%rbp), %rax # t4 # t4
    cmpq $0, %rax
    je while_end_2
    movq -8(%rbp), %rax # x # x
    movq %rax, -64(%rbp)
    movq -16(%rbp), %rax # i # i
    movq %rax, -72(%rbp)
    movq -64(%rbp), %rdi # t5 # t5
    movq -72(%rbp), %rsi # t6 # t6
    movq $0, %rax
    call at
    movq %rax, -80(%rbp)
    movq %rax, -88(%rbp)
    leaq t8(%rip), %rax
    movq %rax, -96(%rbp)
    movq -16(%rbp), %rax # i # i
    movq %rax, -104(%rbp)
    movq -96(%rbp), %rdi # t8 # t8
    movq -88(%rbp), %rsi # value # value
    movq -104(%rbp), %rdx # t9 # t9
    movq $0, %rax
    call printf
    movq %rax, -112(%rbp)
    movq -16(%rbp), %rax # i # i
    movq %rax, -120(%rbp)
    movq $1, %rax
    movq %rax, -128(%rbp)
    movq -120(%rbp), %rax # t11 # t11
    addq -128(%rbp), %rax
    movq %rax, -136(%rbp)
    movq -136(%rbp), %rdx # t13 # t13
    movq %rdx, -16(%rbp)
    jmp while_start_2
while_end_2: 
    movq $0, %rax
    movq %rax, -144(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

