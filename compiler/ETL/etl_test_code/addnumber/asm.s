.section .data
t19: .quad 0
t17: .quad 0
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
    movq %rax, %rbx
    leave
    movq %rbx, %rax
    ret
add_numbers:
    pushq %rbp
    movq %rsp, %rbp
    subq $160, %rsp
    movq $0, %rcx
    leaq t0(%rip), %rax
    movq %rax, -8(%rbp)
    movq -8(%rbp), %rdi# t0
    movq $0, %rax
    pushq %rcx
    call puts
    movq %rax, -16(%rbp)
    popq %rcx
    movq $0, %rax
    pushq %rcx
    call scan_integer
    movq %rax, -24(%rbp)
    popq %rcx
    movq -24(%rbp), %rax# t2
    movq %rax, -32(%rbp)
    leaq t3(%rip), %rax
    movq %rax, -40(%rbp)
    movq -40(%rbp), %rdi# t3
    movq $0, %rax
    pushq %rcx
    call puts
    movq %rax, -48(%rbp)
    popq %rcx
    movq $0, %rax
    pushq %rcx
    call scan_integer
    movq %rax, -56(%rbp)
    popq %rcx
    movq -56(%rbp), %rax# t5
    movq %rax, -64(%rbp)
    movq -32(%rbp), %rdi# x
    movq $0, %rax
    pushq %rcx
    call str
    movq %rax, -72(%rbp)
    popq %rcx
    addq $22, %rcx
    leaq t7(%rip), %rax
    movq %rax, -80(%rbp)
    movq -72(%rbp), %rdi# t6
    pushq %rcx
    call strlen # t6
    popq %rcx
    addq %rax, %rcx
    addq $1, %rcx
    addq $1, %rcx
    movq %rcx, %rdi
    xorq %rax, %rax
    pushq %rcx
    call malloc
    movq %rax, %rdi
    movq %rdi, -88(%rbp)
    movq -72(%rbp), %rsi# t6
    call strcpy
    movq -80(%rbp), %rsi# t7
    call strcat
    popq %rcx
    movq -64(%rbp), %rdi# y
    movq $0, %rax
    pushq %rcx
    call str
    movq %rax, -96(%rbp)
    popq %rcx
    addq $22, %rcx
    movq -88(%rbp), %rdi# t8
    pushq %rcx
    call strlen # t8
    popq %rcx
    addq %rax, %rcx
    movq -96(%rbp), %rdi# t9
    pushq %rcx
    call strlen # t9
    popq %rcx
    addq %rax, %rcx
    addq $1, %rcx
    movq %rcx, %rdi
    xorq %rax, %rax
    pushq %rcx
    call malloc
    movq %rax, %rdi
    movq %rdi, -104(%rbp)
    movq -88(%rbp), %rsi# t8
    call strcpy
    movq -96(%rbp), %rsi# t9
    call strcat
    popq %rcx
    leaq t11(%rip), %rax
    movq %rax, -112(%rbp)
    movq -104(%rbp), %rdi# t10
    pushq %rcx
    call strlen # t10
    popq %rcx
    addq %rax, %rcx
    addq $1, %rcx
    addq $1, %rcx
    movq %rcx, %rdi
    xorq %rax, %rax
    pushq %rcx
    call malloc
    movq %rax, %rdi
    movq %rdi, -120(%rbp)
    movq -104(%rbp), %rsi# t10
    call strcpy
    movq -112(%rbp), %rsi# t11
    call strcat
    popq %rcx
    movq -32(%rbp), %rax# x
    addq -64(%rbp), %rax
    movq %rax, -128(%rbp)
    movq -128(%rbp), %rdi# t13
    movq $0, %rax
    pushq %rcx
    call str
    movq %rax, -136(%rbp)
    popq %rcx
    addq $22, %rcx
    movq -120(%rbp), %rdi# t12
    pushq %rcx
    call strlen # t12
    popq %rcx
    addq %rax, %rcx
    movq -136(%rbp), %rdi# t14
    pushq %rcx
    call strlen # t14
    popq %rcx
    addq %rax, %rcx
    addq $1, %rcx
    movq %rcx, %rdi
    xorq %rax, %rax
    pushq %rcx
    call malloc
    movq %rax, %rdi
    movq %rdi, -144(%rbp)
    movq -120(%rbp), %rsi# t12
    call strcpy
    movq -136(%rbp), %rsi# t14
    call strcat
    popq %rcx
    movq -144(%rbp), %rdi# t15
    movq $0, %rax
    pushq %rcx
    call puts
    movq %rax, -152(%rbp)
    popq %rcx
    movq $0, %rax
    movq %rax, -160(%rbp)
    movq -160(%rbp), %rax# t17
    movq -136(%rbp), %rdi# t14
    call free #t14
    movq -104(%rbp), %rdi# t10
    call free #t10
    movq -120(%rbp), %rdi# t12
    call free #t12
    movq -96(%rbp), %rdi# t9
    call free #t9
    movq -144(%rbp), %rdi# t15
    call free #t15
    movq -88(%rbp), %rdi# t8
    call free #t8
    movq -72(%rbp), %rdi# t6
    call free #t6
    leave
    ret
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movq $0, %rcx
    movq $0, %rax
    pushq %rcx
    call add_numbers
    movq %rax, -8(%rbp)
    popq %rcx
    movq $0, %rax
    movq %rax, -16(%rbp)
    movq -16(%rbp), %rax# t19
    leave
    ret

