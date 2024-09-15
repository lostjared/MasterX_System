.section .data
t34: .asciz "Random Squares"
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
.globl draw_grid
draw_grid:
    pushq %rbp
    movq %rsp, %rbp
    subq $320, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq $0, %rax
    movq %rax, -24(%rbp)
for_start_0: 
    movq -24(%rbp), %rax # x # x
    movq %rax, -32(%rbp)
    movq -8(%rbp), %rax # w # w
    movq %rax, -40(%rbp)
    movq -32(%rbp), %rax # t2 # t2
    movq -40(%rbp), %rdi # t3 # t3
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -48(%rbp)
    movq -48(%rbp), %rax # t4 # t4
    cmpq $0, %rax
    je for_end_0
    movq $0, %rax
    movq %rax, -56(%rbp)
for_start_5: 
    movq -56(%rbp), %rax # y # y
    movq %rax, -64(%rbp)
    movq -16(%rbp), %rax # h # h
    movq %rax, -72(%rbp)
    movq -64(%rbp), %rax # t7 # t7
    movq -72(%rbp), %rdi # t8 # t8
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -80(%rbp)
    movq -80(%rbp), %rax # t9 # t9
    cmpq $0, %rax
    je for_end_5
    movq $0, %rax
    call rand
    movq %rax, -88(%rbp)
    movq $255, %rax
    movq %rax, -96(%rbp)
    movq -88(%rbp), %rax # t10 # t10
    cqto
    movq -96(%rbp), %rdi # t11 # t11
    idivq %rdi
    movq %rdx, -104(%rbp)
    movq $0, %rax
    call rand
    movq %rax, -112(%rbp)
    movq $255, %rax
    movq %rax, -120(%rbp)
    movq -112(%rbp), %rax # t13 # t13
    cqto
    movq -120(%rbp), %rdi # t14 # t14
    idivq %rdi
    movq %rdx, -128(%rbp)
    movq $0, %rax
    call rand
    movq %rax, -136(%rbp)
    movq $255, %rax
    movq %rax, -144(%rbp)
    movq -136(%rbp), %rax # t16 # t16
    cqto
    movq -144(%rbp), %rdi # t17 # t17
    idivq %rdi
    movq %rdx, -152(%rbp)
    movq $255, %rax
    movq %rax, -160(%rbp)
    movq -104(%rbp), %rdi # t12 # t12
    movq -128(%rbp), %rsi # t15 # t15
    movq -152(%rbp), %rdx # t18 # t18
    movq -160(%rbp), %rcx # t19 # t19
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -168(%rbp)
    movq -24(%rbp), %rax # x # x
    movq %rax, -176(%rbp)
    movq -56(%rbp), %rax # y # y
    movq %rax, -184(%rbp)
    movq $4, %rax
    movq %rax, -192(%rbp)
    movq $4, %rax
    movq %rax, -200(%rbp)
    movq -176(%rbp), %rdi # t21 # t21
    movq -184(%rbp), %rsi # t22 # t22
    movq -192(%rbp), %rdx # t23 # t23
    movq -200(%rbp), %rcx # t24 # t24
    movq $0, %rax
    call sdl_fillrect
    movq %rax, -208(%rbp)
for_post_6: 
    movq -56(%rbp), %rax # y # y
    movq %rax, -216(%rbp)
    movq $4, %rax
    movq %rax, -224(%rbp)
    movq -216(%rbp), %rax # t26 # t26
    addq -224(%rbp), %rax
    movq %rax, -232(%rbp)
# load t28 to y
    movq -232(%rbp), %rcx
    movq %rcx, -56(%rbp)
    jmp for_start_5
for_end_5: 
for_post_1: 
    movq -24(%rbp), %rax # x # x
    movq %rax, -240(%rbp)
    movq $4, %rax
    movq %rax, -248(%rbp)
    movq -240(%rbp), %rax # t29 # t29
    addq -248(%rbp), %rax
    movq %rax, -256(%rbp)
# load t31 to x
    movq -256(%rbp), %rcx
    movq %rcx, -24(%rbp)
    jmp for_start_0
for_end_0: 
    movq $0, %rax
    movq %rax, -264(%rbp)
    leave
    ret
.globl init
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $224, %rsp
    movq $0, %rcx
    movq $800, %rax
    movq %rax, -8(%rbp)
    movq $600, %rax
    movq %rax, -16(%rbp)
    movq $0, %rax
    call sdl_init
    movq %rax, -24(%rbp)
    leaq t34(%rip), %rax
    movq %rax, -32(%rbp)
    movq -8(%rbp), %rax # width # width
    movq %rax, -40(%rbp)
    movq -16(%rbp), %rax # height # height
    movq %rax, -48(%rbp)
    movq -32(%rbp), %rdi # t34 # t34
    movq -40(%rbp), %rsi # t35 # t35
    movq -48(%rbp), %rdx # t36 # t36
    movq $0, %rax
    call sdl_create
    movq %rax, -56(%rbp)
    movq $0, %rax
    movq %rax, -64(%rbp)
    movq -64(%rbp), %rdi # t38 # t38
    movq $0, %rax
    call time
    movq %rax, -72(%rbp)
    movq -72(%rbp), %rdi # t39 # t39
    movq $0, %rax
    call srand
    movq %rax, -80(%rbp)
while_start_41: 
    movq $0, %rax
    call sdl_pump
    movq %rax, -88(%rbp)
    cmpq $0, %rax
    je while_end_41
    movq $0, %rax
    call sdl_clear
    movq %rax, -96(%rbp)
    movq -8(%rbp), %rax # width # width
    movq %rax, -104(%rbp)
    movq -16(%rbp), %rax # height # height
    movq %rax, -112(%rbp)
    movq -104(%rbp), %rdi # t44 # t44
    movq -112(%rbp), %rsi # t45 # t45
    movq $0, %rax
    call draw_grid
    movq %rax, -120(%rbp)
    movq $0, %rax
    call sdl_flip
    movq %rax, -128(%rbp)
    jmp while_start_41
while_end_41: 
    movq $0, %rax
    call sdl_release
    movq %rax, -136(%rbp)
    movq $0, %rax
    call sdl_quit
    movq %rax, -144(%rbp)
    movq $0, %rax
    movq %rax, -152(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

