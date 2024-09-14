.section .data
t15: .asciz "value at (%d,%d) -> %d\n"
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
.globl allocate_array
allocate_array:
    pushq %rbp
    movq %rsp, %rbp
    subq $128, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq -8(%rbp), %rax # size # size
    movq %rax, -16(%rbp)
    movq $1, %rax
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rax # t0 # t0
    addq -24(%rbp), %rax
    movq %rax, -32(%rbp)
    movq -32(%rbp), %rdi # t2 # t2
    movq $0, %rax
    call malloc
    movq %rax, -40(%rbp)
    movq %rax, -48(%rbp)
    movq -8(%rbp), %rax # size # size
    movq %rax, -56(%rbp)
    movq -48(%rbp), %rdi # x # x
    movq -56(%rbp), %rsi # t4 # t4
    movq $0, %rax
    call memclr
    movq %rax, -64(%rbp)
    movq -48(%rbp), %rax # x # x
    leave
    ret
.globl echo_array
echo_array:
    pushq %rbp
    movq %rsp, %rbp
    subq $240, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq %rdx, -24(%rbp)
    movq $0, %rax
    movq %rax, -32(%rbp)
for_start_6: 
    movq -16(%rbp), %rax # w # w
    movq %rax, -40(%rbp)
    movq -32(%rbp), %rax # x # x
    movq -40(%rbp), %rdi # t8 # t8
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -48(%rbp)
    movq -48(%rbp), %rax # t9 # t9
    cmpq $0, %rax
    je for_end_6
    movq $0, %rax
    movq %rax, -56(%rbp)
for_start_10: 
    movq -56(%rbp), %rax # y # y
    movq %rax, -64(%rbp)
    movq -24(%rbp), %rax # h # h
    movq %rax, -72(%rbp)
    movq -64(%rbp), %rax # t12 # t12
    movq -72(%rbp), %rdi # t13 # t13
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -80(%rbp)
    movq -80(%rbp), %rax # t14 # t14
    cmpq $0, %rax
    je for_end_10
    leaq t15(%rip), %rax
    movq %rax, -88(%rbp)
    movq -56(%rbp), %rax # y # y
    movq %rax, -96(%rbp)
    movq -8(%rbp), %rax # array # array
    movq %rax, -104(%rbp)
    movq -16(%rbp), %rax # w # w
    movq %rax, -112(%rbp)
    movq -32(%rbp), %rax # x # x
    imulq -112(%rbp), %rax
    movq %rax, -120(%rbp)
    movq -56(%rbp), %rax # y # y
    movq %rax, -128(%rbp)
    movq -120(%rbp), %rax # t19 # t19
    addq -128(%rbp), %rax
    movq %rax, -136(%rbp)
    movq -104(%rbp), %rdi # t17 # t17
    movq -136(%rbp), %rsi # t21 # t21
    movq $0, %rax
    call mematl
    movq %rax, -144(%rbp)
    movq -88(%rbp), %rdi # t15 # t15
    movq -32(%rbp), %rsi # x # x
    movq -96(%rbp), %rdx # t16 # t16
    movq -144(%rbp), %rcx # t22 # t22
    movq $0, %rax
    call printf
    movq %rax, -152(%rbp)
for_post_11: 
    movq -56(%rbp), %rax # y # y
    movq %rax, -160(%rbp)
    movq $1, %rax
    movq %rax, -168(%rbp)
    movq -160(%rbp), %rax # t24 # t24
    addq -168(%rbp), %rax
    movq %rax, -176(%rbp)
    movq -176(%rbp), %rdx # t26 # t26
    movq %rdx, -56(%rbp)
    jmp for_start_10
for_end_10: 
for_post_7: 
    movq $1, %rax
    movq %rax, -184(%rbp)
    movq -32(%rbp), %rax # x # x
    addq -184(%rbp), %rax
    movq %rax, -192(%rbp)
    movq -192(%rbp), %rdx # t28 # t28
    movq %rdx, -32(%rbp)
    jmp for_start_6
for_end_6: 
    movq $0, %rax
    movq %rax, -200(%rbp)
    leave
    ret
.globl init
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $352, %rsp
    movq $0, %rcx
    movq $12, %rax
    movq %rax, -8(%rbp)
    movq $12, %rax
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rax # w # w
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rax # h # h
    movq %rax, -32(%rbp)
    movq -24(%rbp), %rax # t30 # t30
    imulq -32(%rbp), %rax
    movq %rax, -40(%rbp)
    movq $8, %rax
    movq %rax, -48(%rbp)
    movq -40(%rbp), %rax # t32 # t32
    imulq -48(%rbp), %rax
    movq %rax, -56(%rbp)
    movq -56(%rbp), %rdi # t34 # t34
    movq $0, %rax
    call allocate_array
    movq %rax, -64(%rbp)
    movq %rax, -72(%rbp)
    movq $0, %rax
    movq %rax, -80(%rbp)
    movq -80(%rbp), %rdi # t36 # t36
    movq $0, %rax
    call time
    movq %rax, -88(%rbp)
    movq -88(%rbp), %rdi # t37 # t37
    movq $0, %rax
    call srand
    movq %rax, -96(%rbp)
    movq $0, %rax
    movq %rax, -104(%rbp)
for_start_39: 
    movq -8(%rbp), %rax # w # w
    movq %rax, -112(%rbp)
    movq -104(%rbp), %rax # x # x
    movq -112(%rbp), %rdi # t41 # t41
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -120(%rbp)
    movq -120(%rbp), %rax # t42 # t42
    cmpq $0, %rax
    je for_end_39
    movq $0, %rax
    movq %rax, -128(%rbp)
for_start_43: 
    movq -16(%rbp), %rax # h # h
    movq %rax, -136(%rbp)
    movq -128(%rbp), %rax # y # y
    movq -136(%rbp), %rdi # t45 # t45
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -144(%rbp)
    movq -144(%rbp), %rax # t46 # t46
    cmpq $0, %rax
    je for_end_43
    movq -8(%rbp), %rax # w # w
    movq %rax, -152(%rbp)
    movq -104(%rbp), %rax # x # x
    imulq -152(%rbp), %rax
    movq %rax, -160(%rbp)
    addq -128(%rbp), %rax
    movq %rax, -168(%rbp)
    movq %rax, -176(%rbp)
    movq $0, %rax
    call rand
    movq %rax, -184(%rbp)
    movq $255, %rax
    movq %rax, -192(%rbp)
    movq -184(%rbp), %rax # t50 # t50
    cqto
    movq -192(%rbp), %rdi # t51 # t51
    idivq %rdi
    movq %rdx, -200(%rbp)
    movq -72(%rbp), %rdi # array # array
    movq -176(%rbp), %rsi # index # index
    movq -200(%rbp), %rdx # t52 # t52
    movq $0, %rax
    call memstorel
    movq %rax, -208(%rbp)
for_post_44: 
    movq $1, %rax
    movq %rax, -216(%rbp)
    movq -128(%rbp), %rax # y # y
    addq -216(%rbp), %rax
    movq %rax, -224(%rbp)
    movq -224(%rbp), %rdx # t55 # t55
    movq %rdx, -128(%rbp)
    jmp for_start_43
for_end_43: 
for_post_40: 
    movq $1, %rax
    movq %rax, -232(%rbp)
    movq -104(%rbp), %rax # x # x
    addq -232(%rbp), %rax
    movq %rax, -240(%rbp)
    movq -240(%rbp), %rdx # t57 # t57
    movq %rdx, -104(%rbp)
    jmp for_start_39
for_end_39: 
    movq -8(%rbp), %rax # w # w
    movq %rax, -248(%rbp)
    movq -16(%rbp), %rax # h # h
    movq %rax, -256(%rbp)
    movq -72(%rbp), %rdi # array # array
    movq -248(%rbp), %rsi # t58 # t58
    movq -256(%rbp), %rdx # t59 # t59
    movq $0, %rax
    call echo_array
    movq %rax, -264(%rbp)
    movq -72(%rbp), %rdi # array # array
    movq $0, %rax
    call free
    movq %rax, -272(%rbp)
    movq $0, %rax
    movq %rax, -280(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

