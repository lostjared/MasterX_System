.section .data
t23: .asciz "%d\n"
t7: .asciz " = "
t2: .asciz "  & "
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
.globl andString
andString:
    pushq %rbp
    movq %rsp, %rbp
    subq $176, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq -8(%rbp), %rax # x # x
    movq %rax, -24(%rbp)
    movq -24(%rbp), %rdi # t0 # t0
    movq $0, %rax
    call str
    movq %rax, -32(%rbp)
    addq $22, -40(%rbp)
    leaq t2(%rip), %rax
    movq %rax, -48(%rbp)
    movq $0, %rcx
    movq %rcx, -40(%rbp)
    movq -32(%rbp), %rdi # t1 # t1
    call strlen # t1
    addq %rax, -40(%rbp)
    addq $7, -40(%rbp)
    addq $1, -40(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -40(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -56(%rbp)
    movq -32(%rbp), %rsi # t1 # t1
    call strcpy
    movq -48(%rbp), %rsi # t2 # t2
    call strcat
    movq -16(%rbp), %rax # y # y
    movq %rax, -64(%rbp)
    movq -64(%rbp), %rdi # t4 # t4
    movq $0, %rax
    call str
    movq %rax, -72(%rbp)
    addq $22, -40(%rbp)
    movq $0, %rcx
    movq -56(%rbp), %rdi # t3 # t3
    call strlen # t3
    addq %rax, -40(%rbp)
    addq $1, -40(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -40(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -80(%rbp)
    movq -56(%rbp), %rsi # t3 # t3
    call strcpy
    movq -72(%rbp), %rsi # t5 # t5
    call strcat
    leaq t7(%rip), %rax
    movq %rax, -88(%rbp)
    movq $0, %rcx
    movq -80(%rbp), %rdi # t6 # t6
    call strlen # t6
    addq %rax, -40(%rbp)
    addq $6, -40(%rbp)
    addq $1, -40(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -40(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -96(%rbp)
    movq -80(%rbp), %rsi # t6 # t6
    call strcpy
    movq -88(%rbp), %rsi # t7 # t7
    call strcat
    movq -8(%rbp), %rax # x # x
    movq %rax, -104(%rbp)
    movq -16(%rbp), %rax # y # y
    movq %rax, -112(%rbp)
    movq -104(%rbp), %rax # t9 # t9
    movq -112(%rbp), %rbx # t10 # t10
    andq %rbx, %rax
    movq %rax, -120(%rbp)
    movq -120(%rbp), %rdi # t11 # t11
    movq $0, %rax
    call str
    movq %rax, -128(%rbp)
    addq $22, -40(%rbp)
    movq $0, %rcx
    movq -96(%rbp), %rdi # t8 # t8
    call strlen # t8
    addq %rax, -40(%rbp)
    addq $1, -40(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -40(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -136(%rbp)
    movq -96(%rbp), %rsi # t8 # t8
    call strcpy
    movq -128(%rbp), %rsi # t12 # t12
    call strcat
    movq -96(%rbp), %rdi # t8 # t8
    call free # local variable: t8
    movq -80(%rbp), %rdi # t6 # t6
    call free # local variable: t6
    movq -56(%rbp), %rdi # t3 # t3
    call free # local variable: t3
    movq -136(%rbp), %rax # t13 # t13
    leave
    ret
.globl init
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $176, %rsp
    movq $0, %rcx
    movq $10, %rax
    movq %rax, -8(%rbp)
    movq $100, %rax
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rdi # t14 # t14
    movq -16(%rbp), %rsi # t15 # t15
    movq $0, %rax
    call andString
    movq %rax, -24(%rbp)
    movq -24(%rbp), %rdi # t16 # t16
    movq $0, %rax
    call puts
    movq %rax, -32(%rbp)
    movq $4242411111, %rax
    movq %rax, -40(%rbp)
    movq $8, %rax
    movq %rax, -48(%rbp)
    movq -40(%rbp), %rax # t18 # t18
    movq -48(%rbp), %rcx # t19 # t19
    sarq %cl, %rax
    movq %rax, -56(%rbp)
    movq $2, %rax
    movq %rax, -64(%rbp)
    movq -56(%rbp), %rax # t20 # t20
    movq -64(%rbp), %rbx # t21 # t21
    xorq %rbx, %rax
    movq %rax, -72(%rbp)
    movq %rax, -80(%rbp)
    leaq t23(%rip), %rax
    movq %rax, -88(%rbp)
    movq -88(%rbp), %rdi # t23 # t23
    movq -80(%rbp), %rsi # u # u
    movq $0, %rax
    call printf
    movq %rax, -96(%rbp)
    movq $0, %rax
    movq %rax, -104(%rbp)
    movq -24(%rbp), %rdi # t16 # t16
    call free # local variable: t16
    movq -104(%rbp), %rax # t25 # t25
    leave
    ret
.section .note.GNU-stack,"",@progbits

