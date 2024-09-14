.section .data
t32: .asciz "true\n"
t20: .asciz "%d\n"
t10: .asciz "Nope"
t8: .asciz "Yup\n"
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
    subq $336, %rsp
    movq $0, %rcx
    movq $2, %rax
    movq %rax, -8(%rbp)
    movq $4, %rax
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rax # x # x
    movq %rax, -24(%rbp)
    movq $2, %rax
    movq %rax, -32(%rbp)
    movq -24(%rbp), %rax # t0 # t0
    movq -32(%rbp), %rbx # t1 # t1
    cmpq %rbx, %rax
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -40(%rbp)
    movq -16(%rbp), %rax # y # y
    movq %rax, -48(%rbp)
    movq $4, %rax
    movq %rax, -56(%rbp)
    movq -48(%rbp), %rax # t3 # t3
    movq -56(%rbp), %rbx # t4 # t4
    cmpq %rbx, %rax
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -64(%rbp)
    movq -40(%rbp), %rbx # t2 # t2
    cmpq $0, %rbx
    setne %cl
    movq -64(%rbp), %rbx # t5 # t5
    cmpq $0, %rbx
    setne %dl
    movzbl %cl, %ecx
    movzbl %dl, %edx
    andl %ecx, %edx
    movl %edx, %edx
    movq %rdx, -72(%rbp)
    movq -72(%rbp), %rax # t6 # t6
    cmpq $0, %rax
    je sublabel_else_7
sublabel_if_7: 
    leaq t8(%rip), %rax
    movq %rax, -80(%rbp)
    movq -80(%rbp), %rdi # t8 # t8
    movq $0, %rax
    call printf
    movq %rax, -88(%rbp)
    jmp sublabel_end_7
sublabel_else_7: 
    leaq t10(%rip), %rax
    movq %rax, -96(%rbp)
    movq -96(%rbp), %rdi # t10 # t10
    movq $0, %rax
    call printf
    movq %rax, -104(%rbp)
sublabel_end_7: 
while_start_12: 
    movq -8(%rbp), %rax # x # x
    movq %rax, -112(%rbp)
    movq $10, %rax
    movq %rax, -120(%rbp)
    movq -112(%rbp), %rax # t13 # t13
    movq -120(%rbp), %rbx # t14 # t14
    cmpq %rbx, %rax
    setle %cl
    movzbq %cl, %rdx
    movq %rdx, -128(%rbp)
    movq -16(%rbp), %rax # y # y
    movq %rax, -136(%rbp)
    movq $4, %rax
    movq %rax, -144(%rbp)
    movq -136(%rbp), %rax # t16 # t16
    movq -144(%rbp), %rbx # t17 # t17
    cmpq %rbx, %rax
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -152(%rbp)
    movq -128(%rbp), %rbx # t15 # t15
    cmpq $0, %rbx
    setne %cl
    movq -152(%rbp), %rbx # t18 # t18
    cmpq $0, %rbx
    setne %dl
    movzbl %cl, %ecx
    movzbl %dl, %edx
    andl %ecx, %edx
    movl %edx, %edx
    movq %rdx, -160(%rbp)
    movq -160(%rbp), %rax # t19 # t19
    cmpq $0, %rax
    je while_end_12
    leaq t20(%rip), %rax
    movq %rax, -168(%rbp)
    movq -8(%rbp), %rax # x # x
    movq %rax, -176(%rbp)
    movq -168(%rbp), %rdi # t20 # t20
    movq -176(%rbp), %rsi # t21 # t21
    movq $0, %rax
    call printf
    movq %rax, -184(%rbp)
    movq -8(%rbp), %rax # x # x
    movq %rax, -192(%rbp)
    movq $1, %rax
    movq %rax, -200(%rbp)
    movq -192(%rbp), %rax # t23 # t23
    addq -200(%rbp), %rax
    movq %rax, -208(%rbp)
    movq -208(%rbp), %rdx # t25 # t25
    movq %rdx, -8(%rbp)
    jmp while_start_12
while_end_12: 
    movq $11, %rax
    movq %rax, -216(%rbp)
    movq -8(%rbp), %rax # x # x
    movq -216(%rbp), %rbx # t26 # t26
    cmpq %rbx, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -224(%rbp)
    movq $11, %rax
    movq %rax, -232(%rbp)
    movq -8(%rbp), %rax # x # x
    movq -232(%rbp), %rbx # t28 # t28
    cmpq %rbx, %rax
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -240(%rbp)
    movq -224(%rbp), %rbx # t27 # t27
    cmpq $0, %rbx
    setne %al
    movq -240(%rbp), %rbx # t29 # t29
    cmpq $0, %rbx
    setne %bl
    orb %al, %bl
    movzbq %bl, %rcx
    movq %rcx, -248(%rbp)
    movq -248(%rbp), %rax # t30 # t30
    cmpq $0, %rax
    je sublabel_else_31
sublabel_if_31: 
    leaq t32(%rip), %rax
    movq %rax, -256(%rbp)
    movq -256(%rbp), %rdi # t32 # t32
    movq $0, %rax
    call printf
    movq %rax, -264(%rbp)
    jmp sublabel_end_31
sublabel_else_31: 
sublabel_end_31: 
    movq $0, %rax
    movq %rax, -272(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

