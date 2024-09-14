.section .data
t14: .asciz "Comparison: "
t0: .asciz "value is: %d\n"
t9: .asciz "Hello World"
v: .asciz "Hello World"
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
    subq $240, %rsp
    movq $0, %rcx
    leaq t9(%rip), %rax
    movq %rax, -8(%rbp)
    leaq t0(%rip), %rax
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rax # v # v
    movq %rax, -24(%rbp)
    movq -24(%rbp), %rdi # t1 # t1
    movq $0, %rax
    call strlen
    movq %rax, -32(%rbp)
    movq -16(%rbp), %rdi # t0 # t0
    movq -32(%rbp), %rsi # t2 # t2
    movq $0, %rax
    call printf
    movq %rax, -40(%rbp)
    movq -8(%rbp), %rax # v # v
    movq %rax, -48(%rbp)
    movq -48(%rbp), %rdi # t4 # t4
    movq $0, %rax
    call strlen
    movq %rax, -56(%rbp)
    movq $0, %rax
    movq %rax, -64(%rbp)
    movq -56(%rbp), %rax # t5 # t5
    movq -64(%rbp), %rbx # t6 # t6
    cmpq %rbx, %rax
    setg %cl
    movzbq %cl, %rdx
    movq %rdx, -72(%rbp)
    movq -8(%rbp), %rax # v # v
    movq %rax, -80(%rbp)
    leaq t9(%rip), %rax
    movq %rax, -88(%rbp)
    movq -80(%rbp), %rdi # t8 # t8
    movq -88(%rbp), %rsi # t9 # t9
    movq $0, %rax
    call strcmp
    movq %rax, -96(%rbp)
    movq $0, %rax
    movq %rax, -104(%rbp)
    movq -96(%rbp), %rax # t10 # t10
    movq -104(%rbp), %rbx # t11 # t11
    cmpq %rbx, %rax
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -112(%rbp)
    movq -72(%rbp), %rbx # t7 # t7
    cmpq $0, %rbx
    setne %cl
    movq -112(%rbp), %rbx # t12 # t12
    cmpq $0, %rbx
    setne %dl
    movzbl %cl, %ecx
    movzbl %dl, %edx
    andl %ecx, %edx
    movl %edx, %edx
    movq %rdx, -120(%rbp)
    movq -120(%rbp), %rax # t13 # t13
    movq %rax, -128(%rbp)
    leaq t14(%rip), %rax
    movq %rax, -136(%rbp)
    movq -128(%rbp), %rdi # x # x
    movq $0, %rax
    call str
    movq %rax, -144(%rbp)
    addq $22, -152(%rbp)
    movq $0, %rcx
    movq %rcx, -152(%rbp)
    addq $15, -152(%rbp)
    addq $1, -152(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -152(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -160(%rbp)
    movq -136(%rbp), %rsi # t14 # t14
    call strcpy
    movq -144(%rbp), %rsi # t15 # t15
    call strcat
    movq -160(%rbp), %rdi # t16 # t16
    movq $0, %rax
    call puts
    movq %rax, -168(%rbp)
    movq $0, %rax
    movq %rax, -176(%rbp)
    movq -160(%rbp), %rdi # t16 # t16
    call free # local variable: t16
    movq -176(%rbp), %rax # t18 # t18
    leave
    ret
.section .note.GNU-stack,"",@progbits

