.section .data
t5: .asciz "alien.bmp"
t1: .asciz "draw image"
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
    subq $256, %rsp
    movq $0, %rcx
    movq $800, %rax
    movq %rax, -8(%rbp)
    movq $800, %rax
    movq %rax, -16(%rbp)
    movq $0, %rax
    call sdl_init
    movq %rax, -24(%rbp)
    leaq t1(%rip), %rax
    movq %rax, -32(%rbp)
    movq -8(%rbp), %rax # width # width
    movq %rax, -40(%rbp)
    movq -16(%rbp), %rax # height # height
    movq %rax, -48(%rbp)
    movq -32(%rbp), %rdi # t1 # t1
    movq -40(%rbp), %rsi # t2 # t2
    movq -48(%rbp), %rdx # t3 # t3
    movq $0, %rax
    call sdl_create
    movq %rax, -56(%rbp)
    leaq t5(%rip), %rax
    movq %rax, -64(%rbp)
    movq -64(%rbp), %rdi # t5 # t5
    movq $0, %rax
    call sdl_loadtex
    movq %rax, -72(%rbp)
    movq %rax, -80(%rbp)
    movq $0, %rax
    call sdl_cleartarget
    movq %rax, -88(%rbp)
while_start_8: 
    movq $0, %rax
    call sdl_pump
    movq %rax, -96(%rbp)
    cmpq $0, %rax
    je while_end_8
    movq $0, %rax
    call sdl_clear
    movq %rax, -104(%rbp)
    movq $0, %rax
    movq %rax, -112(%rbp)
    movq $0, %rax
    movq %rax, -120(%rbp)
    movq -8(%rbp), %rax # width # width
    movq %rax, -128(%rbp)
    movq -16(%rbp), %rax # height # height
    movq %rax, -136(%rbp)
    movq -80(%rbp), %rdi # img # img
    movq -112(%rbp), %rsi # t11 # t11
    movq -120(%rbp), %rdx # t12 # t12
    movq -128(%rbp), %rcx # t13 # t13
    movq -136(%rbp), %r8 # t14 # t14
    movq $0, %rax
    call sdl_copytex
    movq %rax, -144(%rbp)
    movq $0, %rax
    call sdl_flip
    movq %rax, -152(%rbp)
    jmp while_start_8
while_end_8: 
    movq -80(%rbp), %rdi # img # img
    movq $0, %rax
    call sdl_destroytex
    movq %rax, -160(%rbp)
    movq $0, %rax
    call sdl_release
    movq %rax, -168(%rbp)
    movq $0, %rax
    call sdl_quit
    movq %rax, -176(%rbp)
    movq $0, %rax
    movq %rax, -184(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

