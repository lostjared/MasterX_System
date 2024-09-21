.section .data
t23: .asciz "Pong"
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
.globl draw_paddle
draw_paddle:
    pushq %rbp
    movq %rsp, %rbp
    subq $160, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq $255, %rax
    movq %rax, -24(%rbp)
    movq $255, %rax
    movq %rax, -32(%rbp)
    movq $255, %rax
    movq %rax, -40(%rbp)
    movq $255, %rax
    movq %rax, -48(%rbp)
    movq -24(%rbp), %rdi # t0 # t0
    movq -32(%rbp), %rsi # t1 # t1
    movq -40(%rbp), %rdx # t2 # t2
    movq -48(%rbp), %rcx # t3 # t3
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -56(%rbp)
    movq -8(%rbp), %rax # x # x
    movq %rax, -64(%rbp)
    movq -16(%rbp), %rax # y # y
    movq %rax, -72(%rbp)
    movq $10, %rax
    movq %rax, -80(%rbp)
    movq $60, %rax
    movq %rax, -88(%rbp)
    movq -64(%rbp), %rdi # t5 # t5
    movq -72(%rbp), %rsi # t6 # t6
    movq -80(%rbp), %rdx # t7 # t7
    movq -88(%rbp), %rcx # t8 # t8
    movq $0, %rax
    call sdl_fillrect
    movq %rax, -96(%rbp)
    movq $0, %rax
    movq %rax, -104(%rbp)
    leave
    ret
.globl draw_ball
draw_ball:
    pushq %rbp
    movq %rsp, %rbp
    subq $160, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq $255, %rax
    movq %rax, -24(%rbp)
    movq $255, %rax
    movq %rax, -32(%rbp)
    movq $255, %rax
    movq %rax, -40(%rbp)
    movq $255, %rax
    movq %rax, -48(%rbp)
    movq -24(%rbp), %rdi # t11 # t11
    movq -32(%rbp), %rsi # t12 # t12
    movq -40(%rbp), %rdx # t13 # t13
    movq -48(%rbp), %rcx # t14 # t14
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -56(%rbp)
    movq -8(%rbp), %rax # x # x
    movq %rax, -64(%rbp)
    movq -16(%rbp), %rax # y # y
    movq %rax, -72(%rbp)
    movq $16, %rax
    movq %rax, -80(%rbp)
    movq $16, %rax
    movq %rax, -88(%rbp)
    movq -64(%rbp), %rdi # t16 # t16
    movq -72(%rbp), %rsi # t17 # t17
    movq -80(%rbp), %rdx # t18 # t18
    movq -88(%rbp), %rcx # t19 # t19
    movq $0, %rax
    call sdl_fillrect
    movq %rax, -96(%rbp)
    movq $0, %rax
    movq %rax, -104(%rbp)
    leave
    ret
.globl init
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $880, %rsp
    movq $0, %rcx
    movq $0, %rax
    call sdl_init
    movq %rax, -8(%rbp)
    leaq t23(%rip), %rax
    movq %rax, -16(%rbp)
    movq $640, %rax
    movq %rax, -24(%rbp)
    movq $480, %rax
    movq %rax, -32(%rbp)
    movq -16(%rbp), %rdi # t23 # t23
    movq -24(%rbp), %rsi # t24 # t24
    movq -32(%rbp), %rdx # t25 # t25
    movq $0, %rax
    call sdl_create
    movq %rax, -40(%rbp)
    movq $200, %rax
    movq %rax, -48(%rbp)
    movq $200, %rax
    movq %rax, -56(%rbp)
    movq $320, %rax
    movq %rax, -64(%rbp)
    movq $240, %rax
    movq %rax, -72(%rbp)
    movq $2, %rax
    movq %rax, -80(%rbp)
    movq $2, %rax
    movq %rax, -88(%rbp)
    movq $0, %rax
    call sdl_getticks
    movq %rax, -96(%rbp)
    movq %rax, -104(%rbp)
while_start_28: 
    movq $0, %rax
    call sdl_pump
    movq %rax, -112(%rbp)
    cmpq $0, %rax
    je while_end_28
    movq $0, %rax
    movq %rax, -120(%rbp)
    movq $0, %rax
    movq %rax, -128(%rbp)
    movq $0, %rax
    movq %rax, -136(%rbp)
    movq $255, %rax
    movq %rax, -144(%rbp)
    movq -120(%rbp), %rdi # t30 # t30
    movq -128(%rbp), %rsi # t31 # t31
    movq -136(%rbp), %rdx # t32 # t32
    movq -144(%rbp), %rcx # t33 # t33
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -152(%rbp)
    movq $0, %rax
    call sdl_clear
    movq %rax, -160(%rbp)
    movq $10, %rax
    movq %rax, -168(%rbp)
    movq -48(%rbp), %rax # paddle1_y # paddle1_y
    movq %rax, -176(%rbp)
    movq -168(%rbp), %rdi # t36 # t36
    movq -176(%rbp), %rsi # t37 # t37
    movq $0, %rax
    call draw_paddle
    movq %rax, -184(%rbp)
    movq $620, %rax
    movq %rax, -192(%rbp)
    movq -56(%rbp), %rax # paddle2_y # paddle2_y
    movq %rax, -200(%rbp)
    movq -192(%rbp), %rdi # t39 # t39
    movq -200(%rbp), %rsi # t40 # t40
    movq $0, %rax
    call draw_paddle
    movq %rax, -208(%rbp)
    movq -64(%rbp), %rax # ball_x # ball_x
    movq %rax, -216(%rbp)
    movq -72(%rbp), %rax # ball_y # ball_y
    movq %rax, -224(%rbp)
    movq -216(%rbp), %rdi # t42 # t42
    movq -224(%rbp), %rsi # t43 # t43
    movq $0, %rax
    call draw_ball
    movq %rax, -232(%rbp)
    movq $0, %rax
    call sdl_getticks
    movq %rax, -240(%rbp)
    movq %rax, -248(%rbp)
    subq -104(%rbp), %rax
    movq %rax, -256(%rbp)
    movq %rax, -264(%rbp)
    movq $4, %rax
    movq %rax, -272(%rbp)
    movq -264(%rbp), %rax # adjusted # adjusted
    movq -272(%rbp), %rdi # t47 # t47
    cmpq %rdi, %rax
    setge %cl
    movzbq %cl, %rdx
    movq %rdx, -280(%rbp)
    movq -280(%rbp), %rax # t48 # t48
    cmpq $0, %rax
    je sublabel_else_49
sublabel_if_49: 
# load current_time to prev_time
    movq -248(%rbp), %rcx
    movq %rcx, -104(%rbp)
    movq -64(%rbp), %rax # ball_x # ball_x
    movq %rax, -288(%rbp)
    movq -80(%rbp), %rax # ball_vel_x # ball_vel_x
    movq %rax, -296(%rbp)
    movq -288(%rbp), %rax # t50 # t50
    addq -296(%rbp), %rax
    movq %rax, -304(%rbp)
# load t52 to ball_x
    movq -304(%rbp), %rcx
    movq %rcx, -64(%rbp)
    movq -72(%rbp), %rax # ball_y # ball_y
    movq %rax, -312(%rbp)
    movq -88(%rbp), %rax # ball_vel_y # ball_vel_y
    movq %rax, -320(%rbp)
    movq -312(%rbp), %rax # t53 # t53
    addq -320(%rbp), %rax
    movq %rax, -328(%rbp)
# load t55 to ball_y
    movq -328(%rbp), %rcx
    movq %rcx, -72(%rbp)
    movq $0, %rax
    movq %rax, -336(%rbp)
    movq -72(%rbp), %rax # ball_y # ball_y
    movq -336(%rbp), %rdi # t56 # t56
    cmpq %rdi, %rax
    setle %cl
    movzbq %cl, %rdx
    movq %rdx, -344(%rbp)
    movq $470, %rax
    movq %rax, -352(%rbp)
    movq -72(%rbp), %rax # ball_y # ball_y
    movq -352(%rbp), %rdi # t58 # t58
    cmpq %rdi, %rax
    setge %cl
    movzbq %cl, %rdx
    movq %rdx, -360(%rbp)
    movq -344(%rbp), %rdi # t57 # t57
    cmpq $0, %rdi
    setne %al
    movq -360(%rbp), %rdx # t59 # t59
    cmpq $0, %rdx
    setne %cl
    orb %al, %cl
    movzbq %cl, %rdx
    movq %rdx, -368(%rbp)
    movq -368(%rbp), %rax # t60 # t60
    cmpq $0, %rax
    je sublabel_else_61
sublabel_if_61: 
    movq -88(%rbp), %rax # ball_vel_y # ball_vel_y
    movq %rax, -376(%rbp)
    negq %rax
    movq %rax, -384(%rbp)
# load t63 to ball_vel_y
    movq -384(%rbp), %rcx
    movq %rcx, -88(%rbp)
    jmp sublabel_end_61
sublabel_else_61: 
sublabel_end_61: 
    movq $20, %rax
    movq %rax, -392(%rbp)
    movq -64(%rbp), %rax # ball_x # ball_x
    movq -392(%rbp), %rdi # t64 # t64
    cmpq %rdi, %rax
    setle %cl
    movzbq %cl, %rdx
    movq %rdx, -400(%rbp)
    movq -48(%rbp), %rax # paddle1_y # paddle1_y
    movq %rax, -408(%rbp)
    movq -72(%rbp), %rax # ball_y # ball_y
    movq -408(%rbp), %rdi # t66 # t66
    cmpq %rdi, %rax
    setge %cl
    movzbq %cl, %rdx
    movq %rdx, -416(%rbp)
    movq -400(%rbp), %rsi # t65 # t65
    cmpq $0, %rsi
    setne %al
    movq -416(%rbp), %rdi # t67 # t67
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -424(%rbp)
    movq -48(%rbp), %rax # paddle1_y # paddle1_y
    movq %rax, -432(%rbp)
    movq $60, %rax
    movq %rax, -440(%rbp)
    movq -432(%rbp), %rax # t69 # t69
    addq -440(%rbp), %rax
    movq %rax, -448(%rbp)
    movq -72(%rbp), %rax # ball_y # ball_y
    movq -448(%rbp), %rdi # t71 # t71
    cmpq %rdi, %rax
    setle %cl
    movzbq %cl, %rdx
    movq %rdx, -456(%rbp)
    movq -424(%rbp), %rsi # t68 # t68
    cmpq $0, %rsi
    setne %al
    movq -456(%rbp), %rdi # t72 # t72
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -464(%rbp)
    cmpq $0, %rax
    je sublabel_else_74
sublabel_if_74: 
    movq -80(%rbp), %rax # ball_vel_x # ball_vel_x
    movq %rax, -472(%rbp)
    negq %rax
    movq %rax, -480(%rbp)
# load t76 to ball_vel_x
    movq -480(%rbp), %rcx
    movq %rcx, -80(%rbp)
    jmp sublabel_end_74
sublabel_else_74: 
sublabel_end_74: 
    movq $610, %rax
    movq %rax, -488(%rbp)
    movq -64(%rbp), %rax # ball_x # ball_x
    movq -488(%rbp), %rdi # t77 # t77
    cmpq %rdi, %rax
    setge %cl
    movzbq %cl, %rdx
    movq %rdx, -496(%rbp)
    movq -56(%rbp), %rax # paddle2_y # paddle2_y
    movq %rax, -504(%rbp)
    movq -72(%rbp), %rax # ball_y # ball_y
    movq -504(%rbp), %rdi # t79 # t79
    cmpq %rdi, %rax
    setge %cl
    movzbq %cl, %rdx
    movq %rdx, -512(%rbp)
    movq -496(%rbp), %rsi # t78 # t78
    cmpq $0, %rsi
    setne %al
    movq -512(%rbp), %rdi # t80 # t80
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -520(%rbp)
    movq -56(%rbp), %rax # paddle2_y # paddle2_y
    movq %rax, -528(%rbp)
    movq $60, %rax
    movq %rax, -536(%rbp)
    movq -528(%rbp), %rax # t82 # t82
    addq -536(%rbp), %rax
    movq %rax, -544(%rbp)
    movq -72(%rbp), %rax # ball_y # ball_y
    movq -544(%rbp), %rdi # t84 # t84
    cmpq %rdi, %rax
    setle %cl
    movzbq %cl, %rdx
    movq %rdx, -552(%rbp)
    movq -520(%rbp), %rsi # t81 # t81
    cmpq $0, %rsi
    setne %al
    movq -552(%rbp), %rdi # t85 # t85
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -560(%rbp)
    cmpq $0, %rax
    je sublabel_else_87
sublabel_if_87: 
    movq -80(%rbp), %rax # ball_vel_x # ball_vel_x
    negq %rax
    movq %rax, -568(%rbp)
# load t88 to ball_vel_x
    movq -568(%rbp), %rcx
    movq %rcx, -80(%rbp)
    jmp sublabel_end_87
sublabel_else_87: 
sublabel_end_87: 
    movq -56(%rbp), %rax # paddle2_y # paddle2_y
    movq %rax, -576(%rbp)
    movq $30, %rax
    movq %rax, -584(%rbp)
    movq -576(%rbp), %rax # t89 # t89
    addq -584(%rbp), %rax
    movq %rax, -592(%rbp)
    movq -72(%rbp), %rdi # ball_y # ball_y
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -600(%rbp)
    movq -600(%rbp), %rax # t92 # t92
    cmpq $0, %rax
    je sublabel_else_93
sublabel_if_93: 
    movq -56(%rbp), %rax # paddle2_y # paddle2_y
    movq %rax, -608(%rbp)
    movq $2, %rax
    movq %rax, -616(%rbp)
    movq -608(%rbp), %rax # t94 # t94
    addq -616(%rbp), %rax
    movq %rax, -624(%rbp)
# load t96 to paddle2_y
    movq -624(%rbp), %rcx
    movq %rcx, -56(%rbp)
    jmp sublabel_end_93
sublabel_else_93: 
sublabel_end_93: 
    movq $30, %rax
    movq %rax, -632(%rbp)
    movq -56(%rbp), %rax # paddle2_y # paddle2_y
    addq -632(%rbp), %rax
    movq %rax, -640(%rbp)
    movq -72(%rbp), %rdi # ball_y # ball_y
    cmpq %rdi, %rax
    setg %cl
    movzbq %cl, %rdx
    movq %rdx, -648(%rbp)
    movq -648(%rbp), %rax # t99 # t99
    cmpq $0, %rax
    je sublabel_else_100
sublabel_if_100: 
    movq $2, %rax
    movq %rax, -656(%rbp)
    movq -56(%rbp), %rax # paddle2_y # paddle2_y
    subq -656(%rbp), %rax
    movq %rax, -664(%rbp)
# load t102 to paddle2_y
    movq -664(%rbp), %rcx
    movq %rcx, -56(%rbp)
    jmp sublabel_end_100
sublabel_else_100: 
sublabel_end_100: 
    movq $82, %rax
    movq %rax, -672(%rbp)
    movq -672(%rbp), %rdi # t103 # t103
    movq $0, %rax
    call sdl_keydown
    movq %rax, -680(%rbp)
    cmpq $0, %rax
    je sublabel_else_105
sublabel_if_105: 
    movq -48(%rbp), %rax # paddle1_y # paddle1_y
    movq %rax, -688(%rbp)
    movq $0, %rax
    movq %rax, -696(%rbp)
    movq -688(%rbp), %rax # t106 # t106
    movq -696(%rbp), %rdi # t107 # t107
    cmpq %rdi, %rax
    setg %cl
    movzbq %cl, %rdx
    movq %rdx, -704(%rbp)
    movq -704(%rbp), %rax # t108 # t108
    cmpq $0, %rax
    je sublabel_else_109
sublabel_if_109: 
    movq -48(%rbp), %rax # paddle1_y # paddle1_y
    movq %rax, -712(%rbp)
    movq $5, %rax
    movq %rax, -720(%rbp)
    movq -712(%rbp), %rax # t110 # t110
    subq -720(%rbp), %rax
    movq %rax, -728(%rbp)
# load t112 to paddle1_y
    movq -728(%rbp), %rcx
    movq %rcx, -48(%rbp)
    jmp sublabel_end_109
sublabel_else_109: 
sublabel_end_109: 
    jmp sublabel_end_105
sublabel_else_105: 
sublabel_end_105: 
    movq $81, %rax
    movq %rax, -736(%rbp)
    movq -736(%rbp), %rdi # t113 # t113
    movq $0, %rax
    call sdl_keydown
    movq %rax, -744(%rbp)
    cmpq $0, %rax
    je sublabel_else_115
sublabel_if_115: 
    movq $420, %rax
    movq %rax, -752(%rbp)
    movq -48(%rbp), %rax # paddle1_y # paddle1_y
    movq -752(%rbp), %rdi # t116 # t116
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -760(%rbp)
    movq -760(%rbp), %rax # t117 # t117
    cmpq $0, %rax
    je sublabel_else_118
sublabel_if_118: 
    movq $5, %rax
    movq %rax, -768(%rbp)
    movq -48(%rbp), %rax # paddle1_y # paddle1_y
    addq -768(%rbp), %rax
    movq %rax, -776(%rbp)
# load t120 to paddle1_y
    movq -776(%rbp), %rcx
    movq %rcx, -48(%rbp)
    jmp sublabel_end_118
sublabel_else_118: 
sublabel_end_118: 
    jmp sublabel_end_115
sublabel_else_115: 
sublabel_end_115: 
    jmp sublabel_end_49
sublabel_else_49: 
sublabel_end_49: 
    movq $0, %rax
    call sdl_flip
    movq %rax, -784(%rbp)
    jmp while_start_28
while_end_28: 
    movq $0, %rax
    call sdl_release
    movq %rax, -792(%rbp)
    movq $0, %rax
    call sdl_quit
    movq %rax, -800(%rbp)
    movq $0, %rax
    movq %rax, -808(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

