.section .data
t11: .asciz "="
t3: .asciz "Enter second number: "
t7: .asciz "+"
t0: .asciz "Enter firsT number: "
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
.globl add_numbers
add_numbers:
    pushq %rbp
    movq %rsp, %rbp
    subq $224, %rsp
    movq $0, %rcx
    leaq t0(%rip), %rax
    movq %rax, -8(%rbp)
    movq -8(%rbp), %rdi # t0 # t0
    movq $0, %rax
    call puts
    movq %rax, -16(%rbp)
    movq $0, %rax
    call scan_integer
    movq %rax, -24(%rbp)
    movq %rax, -32(%rbp)
    leaq t3(%rip), %rax
    movq %rax, -40(%rbp)
    movq -40(%rbp), %rdi # t3 # t3
    movq $0, %rax
    call puts
    movq %rax, -48(%rbp)
    movq $0, %rax
    call scan_integer
    movq %rax, -56(%rbp)
    movq %rax, -64(%rbp)
    movq -32(%rbp), %rdi # x # x
    movq $0, %rax
    call str
    movq %rax, -72(%rbp)
    addq $22, -80(%rbp)
    leaq t7(%rip), %rax
    movq %rax, -88(%rbp)
    movq $0, %rcx
    movq %rcx, -80(%rbp)
    movq -72(%rbp), %rdi # t6 # t6
    call strlen # t6
    addq %rax, -80(%rbp)
    addq $4, -80(%rbp)
    addq $1, -80(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -80(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -96(%rbp)
    movq -72(%rbp), %rsi # t6 # t6
    call strcpy
    movq -88(%rbp), %rsi # t7 # t7
    call strcat
    movq -64(%rbp), %rdi # y # y
    movq $0, %rax
    call str
    movq %rax, -104(%rbp)
    addq $22, -80(%rbp)
    movq $0, %rcx
    movq -96(%rbp), %rdi # t8 # t8
    call strlen # t8
    addq %rax, -80(%rbp)
    addq $1, -80(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -80(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -112(%rbp)
    movq -96(%rbp), %rsi # t8 # t8
    call strcpy
    movq -104(%rbp), %rsi # t9 # t9
    call strcat
    leaq t11(%rip), %rax
    movq %rax, -120(%rbp)
    movq $0, %rcx
    movq -112(%rbp), %rdi # t10 # t10
    call strlen # t10
    addq %rax, -80(%rbp)
    addq $4, -80(%rbp)
    addq $1, -80(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -80(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -128(%rbp)
    movq -112(%rbp), %rsi # t10 # t10
    call strcpy
    movq -120(%rbp), %rsi # t11 # t11
    call strcat
    movq -32(%rbp), %rax # x # x
    addq -64(%rbp), %rax
    movq %rax, -136(%rbp)
    movq -136(%rbp), %rdi # t13 # t13
    movq $0, %rax
    call str
    movq %rax, -144(%rbp)
    addq $22, -80(%rbp)
    movq $0, %rcx
    movq -128(%rbp), %rdi # t12 # t12
    call strlen # t12
    addq %rax, -80(%rbp)
    addq $1, -80(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -80(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -152(%rbp)
    movq -128(%rbp), %rsi # t12 # t12
    call strcpy
    movq -144(%rbp), %rsi # t14 # t14
    call strcat
    movq -152(%rbp), %rdi # t15 # t15
    movq $0, %rax
    call puts
    movq %rax, -160(%rbp)
    movq $0, %rax
    movq %rax, -168(%rbp)
    movq -128(%rbp), %rdi # t12 # t12
    call free # local variable: t12
    movq -112(%rbp), %rdi # t10 # t10
    call free # local variable: t10
    movq -152(%rbp), %rdi # t15 # t15
    call free # local variable: t15
    movq -96(%rbp), %rdi # t8 # t8
    call free # local variable: t8
    movq -168(%rbp), %rax # t17 # t17
    leave
    ret
.globl init
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $96, %rsp
    movq $0, %rcx
    movq $0, %rax
    movq %rax, -8(%rbp)
    movq $0, %rax
    movq %rax, -16(%rbp)
    movq $0, %rax
    call add_numbers
    movq %rax, -24(%rbp)
    movq $0, %rax
    movq %rax, -32(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

