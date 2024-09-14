.section .data
s: .asciz "Jared Says: "
t18: .asciz "Hey two numers: "
t11: .asciz "="
t6: .asciz "+"
t1: .asciz ": "
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
.globl format
format:
    pushq %rbp
    movq %rsp, %rbp
    subq $208, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq %rdx, -24(%rbp)
    movq -8(%rbp), %rax # hello # hello
    movq %rax, -32(%rbp)
    leaq t1(%rip), %rax
    movq %rax, -40(%rbp)
    movq $0, %rcx
    movq %rcx, -48(%rbp)
    movq -32(%rbp), %rdi # t0 # t0
    call strlen # t0
    addq %rax, -48(%rbp)
    addq $5, -48(%rbp)
    addq $1, -48(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -48(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -56(%rbp)
    movq -32(%rbp), %rsi # t0 # t0
    call strcpy
    movq -40(%rbp), %rsi # t1 # t1
    call strcat
    movq -16(%rbp), %rax # x # x
    movq %rax, -64(%rbp)
    movq -64(%rbp), %rdi # t3 # t3
    movq $0, %rax
    call str
    movq %rax, -72(%rbp)
    addq $22, -48(%rbp)
    movq $0, %rcx
    movq -56(%rbp), %rdi # t2 # t2
    call strlen # t2
    addq %rax, -48(%rbp)
    addq $1, -48(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -48(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -80(%rbp)
    movq -56(%rbp), %rsi # t2 # t2
    call strcpy
    movq -72(%rbp), %rsi # t4 # t4
    call strcat
    leaq t6(%rip), %rax
    movq %rax, -88(%rbp)
    movq $0, %rcx
    movq -80(%rbp), %rdi # t5 # t5
    call strlen # t5
    addq %rax, -48(%rbp)
    addq $4, -48(%rbp)
    addq $1, -48(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -48(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -96(%rbp)
    movq -80(%rbp), %rsi # t5 # t5
    call strcpy
    movq -88(%rbp), %rsi # t6 # t6
    call strcat
    movq -24(%rbp), %rax # y # y
    movq %rax, -104(%rbp)
    movq -104(%rbp), %rdi # t8 # t8
    movq $0, %rax
    call str
    movq %rax, -112(%rbp)
    addq $22, -48(%rbp)
    movq $0, %rcx
    movq -96(%rbp), %rdi # t7 # t7
    call strlen # t7
    addq %rax, -48(%rbp)
    addq $1, -48(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -48(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -120(%rbp)
    movq -96(%rbp), %rsi # t7 # t7
    call strcpy
    movq -112(%rbp), %rsi # t9 # t9
    call strcat
    leaq t11(%rip), %rax
    movq %rax, -128(%rbp)
    movq $0, %rcx
    movq -120(%rbp), %rdi # t10 # t10
    call strlen # t10
    addq %rax, -48(%rbp)
    addq $4, -48(%rbp)
    addq $1, -48(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -48(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -136(%rbp)
    movq -120(%rbp), %rsi # t10 # t10
    call strcpy
    movq -128(%rbp), %rsi # t11 # t11
    call strcat
    movq -16(%rbp), %rax # x # x
    movq %rax, -144(%rbp)
    movq -24(%rbp), %rax # y # y
    movq %rax, -152(%rbp)
    movq -144(%rbp), %rax # t13 # t13
    addq -152(%rbp), %rax
    movq %rax, -160(%rbp)
    movq -160(%rbp), %rdi # t15 # t15
    movq $0, %rax
    call str
    movq %rax, -168(%rbp)
    addq $22, -48(%rbp)
    movq $0, %rcx
    movq -136(%rbp), %rdi # t12 # t12
    call strlen # t12
    addq %rax, -48(%rbp)
    addq $1, -48(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -48(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -176(%rbp)
    movq -136(%rbp), %rsi # t12 # t12
    call strcpy
    movq -168(%rbp), %rsi # t16 # t16
    call strcat
    movq -136(%rbp), %rdi # t12 # t12
    call free # local variable: t12
    movq -120(%rbp), %rdi # t10 # t10
    call free # local variable: t10
    movq -96(%rbp), %rdi # t7 # t7
    call free # local variable: t7
    movq -80(%rbp), %rdi # t5 # t5
    call free # local variable: t5
    movq -56(%rbp), %rdi # t2 # t2
    call free # local variable: t2
    movq -176(%rbp), %rax # t17 # t17
    leave
    ret
.globl init
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $176, %rsp
    movq $0, %rcx
    movq $25, %rax
    movq %rax, -8(%rbp)
    movq $25, %rax
    movq %rax, -16(%rbp)
    leaq t18(%rip), %rax
    movq %rax, -24(%rbp)
    movq -8(%rbp), %rax # x # x
    movq %rax, -32(%rbp)
    movq -16(%rbp), %rax # y # y
    movq %rax, -40(%rbp)
    movq -24(%rbp), %rdi # t18 # t18
    movq -32(%rbp), %rsi # t19 # t19
    movq -40(%rbp), %rdx # t20 # t20
    movq $0, %rax
    call format
    movq %rax, -48(%rbp)
    movq -48(%rbp), %rdi # t21 # t21
    movq $0, %rax
    call puts
    movq %rax, -56(%rbp)
    leaq s(%rip), %rax
    movq %rax, -64(%rbp)
    movq %rax, -72(%rbp)
    movq $10, %rax
    movq %rax, -80(%rbp)
    movq $10, %rax
    movq %rax, -88(%rbp)
    movq -72(%rbp), %rdi # t23 # t23
    movq -80(%rbp), %rsi # t24 # t24
    movq -88(%rbp), %rdx # t25 # t25
    movq $0, %rax
    call format
    movq %rax, -96(%rbp)
    movq -96(%rbp), %rdi # t26 # t26
    movq $0, %rax
    call puts
    movq %rax, -104(%rbp)
    movq $0, %rax
    movq %rax, -112(%rbp)
    movq -96(%rbp), %rdi # t26 # t26
    call free # local variable: t26
    movq -48(%rbp), %rdi # t21 # t21
    call free # local variable: t21
    movq -112(%rbp), %rax # t28 # t28
    leave
    ret
.section .note.GNU-stack,"",@progbits

