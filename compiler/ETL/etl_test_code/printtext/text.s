.section .data
t16: .asciz "Hello, World!"
t1: .asciz "Print Text"
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
    movq $600, %rax
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
    movq $0, %rax
    call sdl_cleartarget
    movq %rax, -64(%rbp)
    movq $255, %rax
    movq %rax, -72(%rbp)
    movq $255, %rax
    movq %rax, -80(%rbp)
    movq $255, %rax
    movq %rax, -88(%rbp)
    movq $255, %rax
    movq %rax, -96(%rbp)
    movq -72(%rbp), %rdi # t6 # t6
    movq -80(%rbp), %rsi # t7 # t7
    movq -88(%rbp), %rdx # t8 # t8
    movq -96(%rbp), %rcx # t9 # t9
    movq $0, %rax
    call sdl_settextcolor
    movq %rax, -104(%rbp)
while_start_11: 
    movq $0, %rax
    call sdl_pump
    movq %rax, -112(%rbp)
    cmpq $0, %rax
    je while_end_11
    movq $0, %rax
    call sdl_clear
    movq %rax, -120(%rbp)
    movq $25, %rax
    movq %rax, -128(%rbp)
    movq $25, %rax
    movq %rax, -136(%rbp)
    leaq t16(%rip), %rax
    movq %rax, -144(%rbp)
    movq -128(%rbp), %rdi # t14 # t14
    movq -136(%rbp), %rsi # t15 # t15
    movq -144(%rbp), %rdx # t16 # t16
    movq $0, %rax
    call sdl_printtext
    movq %rax, -152(%rbp)
    movq $0, %rax
    call sdl_flip
    movq %rax, -160(%rbp)
    jmp while_start_11
while_end_11: 
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

