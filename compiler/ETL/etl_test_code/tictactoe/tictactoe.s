.section .data
t395: .asciz "Game Over: Was a draw.\n"
t302: .asciz "Tic-Tac-Toe"
t327: .asciz "Player 1 Wins!"
t335: .asciz "Player 2 Wins!"
t343: .asciz "It's a Draw!"
t393: .asciz "Game Over: Winner was Player: %d\n"
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
.globl allocate_grid
allocate_grid:
    pushq %rbp
    movq %rsp, %rbp
    subq $144, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq -8(%rbp), %rax # size # size
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rax # size # size
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rax # t0 # t0
    imulq -24(%rbp), %rax
    movq %rax, -32(%rbp)
    movq -32(%rbp), %rdi # t2 # t2
    movq $0, %rax
    call malloc
    movq %rax, -40(%rbp)
    movq %rax, -48(%rbp)
    movq -8(%rbp), %rax # size # size
    movq %rax, -56(%rbp)
    movq -8(%rbp), %rax # size # size
    movq %rax, -64(%rbp)
    movq -56(%rbp), %rax # t4 # t4
    imulq -64(%rbp), %rax
    movq %rax, -72(%rbp)
    movq -48(%rbp), %rdi # grid # grid
    movq -72(%rbp), %rsi # t6 # t6
    movq $0, %rax
    call memclr
    movq %rax, -80(%rbp)
    movq -48(%rbp), %rax # grid # grid
    leave
    ret
.globl draw_grid
draw_grid:
    pushq %rbp
    movq %rsp, %rbp
    subq $704, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq $3, %rax
    movq %rax, -16(%rbp)
    movq $0, %rax
    movq %rax, -24(%rbp)
    movq $0, %rax
    movq %rax, -32(%rbp)
    movq $0, %rax
    movq %rax, -40(%rbp)
    movq $255, %rax
    movq %rax, -48(%rbp)
    movq -24(%rbp), %rdi # t8 # t8
    movq -32(%rbp), %rsi # t9 # t9
    movq -40(%rbp), %rdx # t10 # t10
    movq -48(%rbp), %rcx # t11 # t11
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -56(%rbp)
    movq $0, %rax
    movq %rax, -64(%rbp)
    movq $0, %rax
    movq %rax, -72(%rbp)
    movq $600, %rax
    movq %rax, -80(%rbp)
    movq $600, %rax
    movq %rax, -88(%rbp)
    movq -64(%rbp), %rdi # t13 # t13
    movq -72(%rbp), %rsi # t14 # t14
    movq -80(%rbp), %rdx # t15 # t15
    movq -88(%rbp), %rcx # t16 # t16
    movq $0, %rax
    call sdl_fillrect
    movq %rax, -96(%rbp)
    movq $255, %rax
    movq %rax, -104(%rbp)
    movq $225, %rax
    movq %rax, -112(%rbp)
    movq $255, %rax
    movq %rax, -120(%rbp)
    movq $255, %rax
    movq %rax, -128(%rbp)
    movq -104(%rbp), %rdi # t18 # t18
    movq -112(%rbp), %rsi # t19 # t19
    movq -120(%rbp), %rdx # t20 # t20
    movq -128(%rbp), %rcx # t21 # t21
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -136(%rbp)
    movq $1, %rax
    movq %rax, -144(%rbp)
for_start_23: 
    movq -144(%rbp), %rax # i1 # i1
    movq %rax, -152(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -160(%rbp)
    movq -152(%rbp), %rax # t25 # t25
    movq -160(%rbp), %rdi # t26 # t26
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -168(%rbp)
    movq -168(%rbp), %rax # t27 # t27
    cmpq $0, %rax
    je for_end_23
    movq $200, %rax
    movq %rax, -176(%rbp)
    movq -144(%rbp), %rax # i1 # i1
    movq %rax, -184(%rbp)
    movq -176(%rbp), %rax # t28 # t28
    imulq -184(%rbp), %rax
    movq %rax, -192(%rbp)
    movq $0, %rax
    movq %rax, -200(%rbp)
    movq $200, %rax
    movq %rax, -208(%rbp)
    movq -144(%rbp), %rax # i1 # i1
    movq %rax, -216(%rbp)
    movq -208(%rbp), %rax # t32 # t32
    imulq -216(%rbp), %rax
    movq %rax, -224(%rbp)
    movq $600, %rax
    movq %rax, -232(%rbp)
    movq -192(%rbp), %rdi # t30 # t30
    movq -200(%rbp), %rsi # t31 # t31
    movq -224(%rbp), %rdx # t34 # t34
    movq -232(%rbp), %rcx # t35 # t35
    movq $0, %rax
    call sdl_drawline
    movq %rax, -240(%rbp)
for_post_24: 
    movq -144(%rbp), %rax # i1 # i1
    movq %rax, -248(%rbp)
    movq $1, %rax
    movq %rax, -256(%rbp)
    movq -248(%rbp), %rax # t37 # t37
    addq -256(%rbp), %rax
    movq %rax, -264(%rbp)
# load t39 to i1
    movq -264(%rbp), %rcx
    movq %rcx, -144(%rbp)
    jmp for_start_23
for_end_23: 
    movq $1, %rax
    movq %rax, -272(%rbp)
for_start_40: 
    movq -272(%rbp), %rax # i2 # i2
    movq %rax, -280(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -288(%rbp)
    movq -280(%rbp), %rax # t42 # t42
    movq -288(%rbp), %rdi # t43 # t43
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -296(%rbp)
    movq -296(%rbp), %rax # t44 # t44
    cmpq $0, %rax
    je for_end_40
    movq $0, %rax
    movq %rax, -304(%rbp)
    movq $200, %rax
    movq %rax, -312(%rbp)
    movq -272(%rbp), %rax # i2 # i2
    movq %rax, -320(%rbp)
    movq -312(%rbp), %rax # t46 # t46
    imulq -320(%rbp), %rax
    movq %rax, -328(%rbp)
    movq $600, %rax
    movq %rax, -336(%rbp)
    movq $200, %rax
    movq %rax, -344(%rbp)
    movq -272(%rbp), %rax # i2 # i2
    movq %rax, -352(%rbp)
    movq -344(%rbp), %rax # t50 # t50
    imulq -352(%rbp), %rax
    movq %rax, -360(%rbp)
    movq -304(%rbp), %rdi # t45 # t45
    movq -328(%rbp), %rsi # t48 # t48
    movq -336(%rbp), %rdx # t49 # t49
    movq -360(%rbp), %rcx # t52 # t52
    movq $0, %rax
    call sdl_drawline
    movq %rax, -368(%rbp)
for_post_41: 
    movq -272(%rbp), %rax # i2 # i2
    movq %rax, -376(%rbp)
    movq $1, %rax
    movq %rax, -384(%rbp)
    movq -376(%rbp), %rax # t54 # t54
    addq -384(%rbp), %rax
    movq %rax, -392(%rbp)
# load t56 to i2
    movq -392(%rbp), %rcx
    movq %rcx, -272(%rbp)
    jmp for_start_40
for_end_40: 
    movq $0, %rax
    movq %rax, -400(%rbp)
for_start_57: 
    movq -400(%rbp), %rax # x1 # x1
    movq %rax, -408(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -416(%rbp)
    movq -408(%rbp), %rax # t59 # t59
    movq -416(%rbp), %rdi # t60 # t60
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -424(%rbp)
    movq -424(%rbp), %rax # t61 # t61
    cmpq $0, %rax
    je for_end_57
    movq $0, %rax
    movq %rax, -432(%rbp)
for_start_62: 
    movq -432(%rbp), %rax # y1 # y1
    movq %rax, -440(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -448(%rbp)
    movq -440(%rbp), %rax # t64 # t64
    movq -448(%rbp), %rdi # t65 # t65
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -456(%rbp)
    movq -456(%rbp), %rax # t66 # t66
    cmpq $0, %rax
    je for_end_62
    movq -432(%rbp), %rax # y1 # y1
    movq %rax, -464(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -472(%rbp)
    movq -464(%rbp), %rax # t67 # t67
    imulq -472(%rbp), %rax
    movq %rax, -480(%rbp)
    movq -400(%rbp), %rax # x1 # x1
    movq %rax, -488(%rbp)
    movq -480(%rbp), %rax # t69 # t69
    addq -488(%rbp), %rax
    movq %rax, -496(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -496(%rbp), %rsi # t71 # t71
    movq $0, %rax
    call mematb
    movq %rax, -504(%rbp)
    movq %rax, -512(%rbp)
    movq $1, %rax
    movq %rax, -520(%rbp)
    movq -512(%rbp), %rsi # mark # mark
    movq -520(%rbp), %rdi # t73 # t73
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -528(%rbp)
    movq -528(%rbp), %rax # t74 # t74
    cmpq $0, %rax
    je sublabel_else_75
sublabel_if_75: 
    movq -400(%rbp), %rax # x1 # x1
    movq %rax, -536(%rbp)
    movq -432(%rbp), %rax # y1 # y1
    movq %rax, -544(%rbp)
    movq -536(%rbp), %rdi # t76 # t76
    movq -544(%rbp), %rsi # t77 # t77
    movq $0, %rax
    call draw_x
    movq %rax, -552(%rbp)
    jmp sublabel_end_75
sublabel_else_75: 
sublabel_end_75: 
    movq $2, %rax
    movq %rax, -560(%rbp)
    movq -512(%rbp), %rsi # mark # mark
    movq -560(%rbp), %rdi # t79 # t79
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -568(%rbp)
    movq -568(%rbp), %rax # t80 # t80
    cmpq $0, %rax
    je sublabel_else_81
sublabel_if_81: 
    movq -400(%rbp), %rax # x1 # x1
    movq %rax, -576(%rbp)
    movq -432(%rbp), %rax # y1 # y1
    movq %rax, -584(%rbp)
    movq -576(%rbp), %rdi # t82 # t82
    movq -584(%rbp), %rsi # t83 # t83
    movq $0, %rax
    call draw_o
    movq %rax, -592(%rbp)
    jmp sublabel_end_81
sublabel_else_81: 
sublabel_end_81: 
for_post_63: 
    movq -432(%rbp), %rax # y1 # y1
    movq %rax, -600(%rbp)
    movq $1, %rax
    movq %rax, -608(%rbp)
    movq -600(%rbp), %rax # t85 # t85
    addq -608(%rbp), %rax
    movq %rax, -616(%rbp)
# load t87 to y1
    movq -616(%rbp), %rcx
    movq %rcx, -432(%rbp)
    jmp for_start_62
for_end_62: 
for_post_58: 
    movq -400(%rbp), %rax # x1 # x1
    movq %rax, -624(%rbp)
    movq $1, %rax
    movq %rax, -632(%rbp)
    movq -624(%rbp), %rax # t88 # t88
    addq -632(%rbp), %rax
    movq %rax, -640(%rbp)
# load t90 to x1
    movq -640(%rbp), %rcx
    movq %rcx, -400(%rbp)
    jmp for_start_57
for_end_57: 
    movq $0, %rax
    movq %rax, -648(%rbp)
    leave
    ret
.globl draw_x
draw_x:
    pushq %rbp
    movq %rsp, %rbp
    subq $368, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq $3, %rax
    movq %rax, -24(%rbp)
    movq $255, %rax
    movq %rax, -32(%rbp)
    movq $0, %rax
    movq %rax, -40(%rbp)
    movq $0, %rax
    movq %rax, -48(%rbp)
    movq $255, %rax
    movq %rax, -56(%rbp)
    movq -32(%rbp), %rdi # t92 # t92
    movq -40(%rbp), %rsi # t93 # t93
    movq -48(%rbp), %rdx # t94 # t94
    movq -56(%rbp), %rcx # t95 # t95
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -64(%rbp)
    movq $20, %rax
    movq %rax, -72(%rbp)
    movq -8(%rbp), %rax # grid_x # grid_x
    movq %rax, -80(%rbp)
    movq $200, %rax
    movq %rax, -88(%rbp)
    movq -80(%rbp), %rax # t97 # t97
    imulq -88(%rbp), %rax
    movq %rax, -96(%rbp)
    movq -72(%rbp), %rax # offset # offset
    movq %rax, -104(%rbp)
    movq -96(%rbp), %rax # t99 # t99
    addq -104(%rbp), %rax
    movq %rax, -112(%rbp)
    movq %rax, -120(%rbp)
    movq -16(%rbp), %rax # grid_y # grid_y
    movq %rax, -128(%rbp)
    movq $200, %rax
    movq %rax, -136(%rbp)
    movq -128(%rbp), %rax # t102 # t102
    imulq -136(%rbp), %rax
    movq %rax, -144(%rbp)
    movq -72(%rbp), %rax # offset # offset
    movq %rax, -152(%rbp)
    movq -144(%rbp), %rax # t104 # t104
    addq -152(%rbp), %rax
    movq %rax, -160(%rbp)
    movq %rax, -168(%rbp)
    movq -8(%rbp), %rax # grid_x # grid_x
    movq %rax, -176(%rbp)
    movq $1, %rax
    movq %rax, -184(%rbp)
    movq -176(%rbp), %rax # t107 # t107
    addq -184(%rbp), %rax
    movq %rax, -192(%rbp)
    movq $200, %rax
    movq %rax, -200(%rbp)
    movq -192(%rbp), %rax # t109 # t109
    imulq -200(%rbp), %rax
    movq %rax, -208(%rbp)
    movq -72(%rbp), %rax # offset # offset
    movq %rax, -216(%rbp)
    movq -208(%rbp), %rax # t111 # t111
    subq -216(%rbp), %rax
    movq %rax, -224(%rbp)
    movq %rax, -232(%rbp)
    movq -16(%rbp), %rax # grid_y # grid_y
    movq %rax, -240(%rbp)
    movq $1, %rax
    movq %rax, -248(%rbp)
    movq -240(%rbp), %rax # t114 # t114
    addq -248(%rbp), %rax
    movq %rax, -256(%rbp)
    movq $200, %rax
    movq %rax, -264(%rbp)
    movq -256(%rbp), %rax # t116 # t116
    imulq -264(%rbp), %rax
    movq %rax, -272(%rbp)
    movq -72(%rbp), %rax # offset # offset
    movq %rax, -280(%rbp)
    movq -272(%rbp), %rax # t118 # t118
    subq -280(%rbp), %rax
    movq %rax, -288(%rbp)
    movq %rax, -296(%rbp)
    movq -120(%rbp), %rdi # x1 # x1
    movq -168(%rbp), %rsi # y1 # y1
    movq -232(%rbp), %rdx # x2 # x2
    movq -296(%rbp), %rcx # y2 # y2
    movq $0, %rax
    call sdl_drawline
    movq %rax, -304(%rbp)
    movq -120(%rbp), %rdi # x1 # x1
    movq -296(%rbp), %rsi # y2 # y2
    movq -232(%rbp), %rdx # x2 # x2
    movq -168(%rbp), %rcx # y1 # y1
    movq $0, %rax
    call sdl_drawline
    movq %rax, -312(%rbp)
    movq $0, %rax
    movq %rax, -320(%rbp)
    leave
    ret
.globl draw_o
draw_o:
    pushq %rbp
    movq %rsp, %rbp
    subq $240, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq $3, %rax
    movq %rax, -24(%rbp)
    movq $0, %rax
    movq %rax, -32(%rbp)
    movq $0, %rax
    movq %rax, -40(%rbp)
    movq $255, %rax
    movq %rax, -48(%rbp)
    movq $255, %rax
    movq %rax, -56(%rbp)
    movq -32(%rbp), %rdi # t124 # t124
    movq -40(%rbp), %rsi # t125 # t125
    movq -48(%rbp), %rdx # t126 # t126
    movq -56(%rbp), %rcx # t127 # t127
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -64(%rbp)
    movq -8(%rbp), %rax # grid_x # grid_x
    movq %rax, -72(%rbp)
    movq $200, %rax
    movq %rax, -80(%rbp)
    movq -72(%rbp), %rax # t129 # t129
    imulq -80(%rbp), %rax
    movq %rax, -88(%rbp)
    movq $100, %rax
    movq %rax, -96(%rbp)
    movq -88(%rbp), %rax # t131 # t131
    addq -96(%rbp), %rax
    movq %rax, -104(%rbp)
    movq %rax, -112(%rbp)
    movq -16(%rbp), %rax # grid_y # grid_y
    movq %rax, -120(%rbp)
    movq $200, %rax
    movq %rax, -128(%rbp)
    movq -120(%rbp), %rax # t134 # t134
    imulq -128(%rbp), %rax
    movq %rax, -136(%rbp)
    movq $100, %rax
    movq %rax, -144(%rbp)
    movq -136(%rbp), %rax # t136 # t136
    addq -144(%rbp), %rax
    movq %rax, -152(%rbp)
    movq %rax, -160(%rbp)
    movq $80, %rax
    movq %rax, -168(%rbp)
    movq -112(%rbp), %rdi # center_x # center_x
    movq -160(%rbp), %rsi # center_y # center_y
    movq -168(%rbp), %rdx # t139 # t139
    movq $0, %rax
    call sdl_drawcircle
    movq %rax, -176(%rbp)
    movq $0, %rax
    movq %rax, -184(%rbp)
    leave
    ret
.globl check_winner
check_winner:
    pushq %rbp
    movq %rsp, %rbp
    subq $1296, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq $3, %rax
    movq %rax, -16(%rbp)
    movq $0, %rax
    movq %rax, -24(%rbp)
for_start_142: 
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -32(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -40(%rbp)
    movq -32(%rbp), %rax # t144 # t144
    movq -40(%rbp), %rdi # t145 # t145
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -48(%rbp)
    movq -48(%rbp), %rax # t146 # t146
    cmpq $0, %rax
    je for_end_142
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -56(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -64(%rbp)
    movq -56(%rbp), %rax # t147 # t147
    imulq -64(%rbp), %rax
    movq %rax, -72(%rbp)
    movq $0, %rax
    movq %rax, -80(%rbp)
    movq -72(%rbp), %rax # t149 # t149
    addq -80(%rbp), %rax
    movq %rax, -88(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -88(%rbp), %rsi # t151 # t151
    movq $0, %rax
    call mematb
    movq %rax, -96(%rbp)
    movq %rax, -104(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -112(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -120(%rbp)
    movq -112(%rbp), %rax # t153 # t153
    imulq -120(%rbp), %rax
    movq %rax, -128(%rbp)
    movq $0, %rax
    movq %rax, -136(%rbp)
    movq -128(%rbp), %rax # t155 # t155
    addq -136(%rbp), %rax
    movq %rax, -144(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -144(%rbp), %rsi # t157 # t157
    movq $0, %rax
    call mematb
    movq %rax, -152(%rbp)
    movq $0, %rax
    movq %rax, -160(%rbp)
    movq -152(%rbp), %rax # t158 # t158
    movq -160(%rbp), %rdi # t159 # t159
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -168(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -176(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -184(%rbp)
    movq -176(%rbp), %rax # t161 # t161
    imulq -184(%rbp), %rax
    movq %rax, -192(%rbp)
    movq $0, %rax
    movq %rax, -200(%rbp)
    movq -192(%rbp), %rax # t163 # t163
    addq -200(%rbp), %rax
    movq %rax, -208(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -208(%rbp), %rsi # t165 # t165
    movq $0, %rax
    call mematb
    movq %rax, -216(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -224(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -232(%rbp)
    movq -224(%rbp), %rax # t167 # t167
    imulq -232(%rbp), %rax
    movq %rax, -240(%rbp)
    movq $1, %rax
    movq %rax, -248(%rbp)
    movq -240(%rbp), %rax # t169 # t169
    addq -248(%rbp), %rax
    movq %rax, -256(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -256(%rbp), %rsi # t171 # t171
    movq $0, %rax
    call mematb
    movq %rax, -264(%rbp)
    movq -216(%rbp), %rsi # t166 # t166
    movq -264(%rbp), %rdi # t172 # t172
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -272(%rbp)
    movq -168(%rbp), %rsi # t160 # t160
    cmpq $0, %rsi
    setne %al
    movq -272(%rbp), %rdi # t173 # t173
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -280(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -288(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -296(%rbp)
    movq -288(%rbp), %rax # t175 # t175
    imulq -296(%rbp), %rax
    movq %rax, -304(%rbp)
    movq $1, %rax
    movq %rax, -312(%rbp)
    movq -304(%rbp), %rax # t177 # t177
    addq -312(%rbp), %rax
    movq %rax, -320(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -320(%rbp), %rsi # t179 # t179
    movq $0, %rax
    call mematb
    movq %rax, -328(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -336(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -344(%rbp)
    movq -336(%rbp), %rax # t181 # t181
    imulq -344(%rbp), %rax
    movq %rax, -352(%rbp)
    movq $2, %rax
    movq %rax, -360(%rbp)
    movq -352(%rbp), %rax # t183 # t183
    addq -360(%rbp), %rax
    movq %rax, -368(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -368(%rbp), %rsi # t185 # t185
    movq $0, %rax
    call mematb
    movq %rax, -376(%rbp)
    movq -328(%rbp), %rsi # t180 # t180
    movq -376(%rbp), %rdi # t186 # t186
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -384(%rbp)
    movq -280(%rbp), %rsi # t174 # t174
    cmpq $0, %rsi
    setne %al
    movq -384(%rbp), %rdi # t187 # t187
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -392(%rbp)
    cmpq $0, %rax
    je sublabel_else_189
sublabel_if_189: 
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -400(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -408(%rbp)
    movq -400(%rbp), %rax # t190 # t190
    imulq -408(%rbp), %rax
    movq %rax, -416(%rbp)
    movq $0, %rax
    movq %rax, -424(%rbp)
    movq -416(%rbp), %rax # t192 # t192
    addq -424(%rbp), %rax
    movq %rax, -432(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -432(%rbp), %rsi # t194 # t194
    movq $0, %rax
    call mematb
    movq %rax, -440(%rbp)
    leave
    ret
    jmp sublabel_end_189
sublabel_else_189: 
sublabel_end_189: 
    movq $0, %rax
    movq %rax, -448(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -456(%rbp)
    movq -448(%rbp), %rax # t196 # t196
    imulq -456(%rbp), %rax
    movq %rax, -464(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -472(%rbp)
    movq -464(%rbp), %rax # t198 # t198
    addq -472(%rbp), %rax
    movq %rax, -480(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -480(%rbp), %rsi # t200 # t200
    movq $0, %rax
    call mematb
    movq %rax, -488(%rbp)
    movq $0, %rax
    movq %rax, -496(%rbp)
    movq -488(%rbp), %rax # t201 # t201
    movq -496(%rbp), %rdi # t202 # t202
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -504(%rbp)
    movq $0, %rax
    movq %rax, -512(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -520(%rbp)
    movq -512(%rbp), %rax # t204 # t204
    imulq -520(%rbp), %rax
    movq %rax, -528(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -536(%rbp)
    movq -528(%rbp), %rax # t206 # t206
    addq -536(%rbp), %rax
    movq %rax, -544(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -544(%rbp), %rsi # t208 # t208
    movq $0, %rax
    call mematb
    movq %rax, -552(%rbp)
    movq $1, %rax
    movq %rax, -560(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -568(%rbp)
    movq -560(%rbp), %rax # t210 # t210
    imulq -568(%rbp), %rax
    movq %rax, -576(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -584(%rbp)
    movq -576(%rbp), %rax # t212 # t212
    addq -584(%rbp), %rax
    movq %rax, -592(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -592(%rbp), %rsi # t214 # t214
    movq $0, %rax
    call mematb
    movq %rax, -600(%rbp)
    movq -552(%rbp), %rsi # t209 # t209
    movq -600(%rbp), %rdi # t215 # t215
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -608(%rbp)
    movq -504(%rbp), %rsi # t203 # t203
    cmpq $0, %rsi
    setne %al
    movq -608(%rbp), %rdi # t216 # t216
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -616(%rbp)
    movq $1, %rax
    movq %rax, -624(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -632(%rbp)
    movq -624(%rbp), %rax # t218 # t218
    imulq -632(%rbp), %rax
    movq %rax, -640(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -648(%rbp)
    movq -640(%rbp), %rax # t220 # t220
    addq -648(%rbp), %rax
    movq %rax, -656(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -656(%rbp), %rsi # t222 # t222
    movq $0, %rax
    call mematb
    movq %rax, -664(%rbp)
    movq $2, %rax
    movq %rax, -672(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -680(%rbp)
    movq -672(%rbp), %rax # t224 # t224
    imulq -680(%rbp), %rax
    movq %rax, -688(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -696(%rbp)
    movq -688(%rbp), %rax # t226 # t226
    addq -696(%rbp), %rax
    movq %rax, -704(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -704(%rbp), %rsi # t228 # t228
    movq $0, %rax
    call mematb
    movq %rax, -712(%rbp)
    movq -664(%rbp), %rsi # t223 # t223
    movq -712(%rbp), %rdi # t229 # t229
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -720(%rbp)
    movq -616(%rbp), %rsi # t217 # t217
    cmpq $0, %rsi
    setne %al
    movq -720(%rbp), %rdi # t230 # t230
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -728(%rbp)
    cmpq $0, %rax
    je sublabel_else_232
sublabel_if_232: 
    movq $0, %rax
    movq %rax, -736(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -744(%rbp)
    movq -736(%rbp), %rax # t233 # t233
    imulq -744(%rbp), %rax
    movq %rax, -752(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -760(%rbp)
    movq -752(%rbp), %rax # t235 # t235
    addq -760(%rbp), %rax
    movq %rax, -768(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -768(%rbp), %rsi # t237 # t237
    movq $0, %rax
    call mematb
    movq %rax, -776(%rbp)
    leave
    ret
    jmp sublabel_end_232
sublabel_else_232: 
sublabel_end_232: 
for_post_143: 
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -784(%rbp)
    movq $1, %rax
    movq %rax, -792(%rbp)
    movq -784(%rbp), %rax # t239 # t239
    addq -792(%rbp), %rax
    movq %rax, -800(%rbp)
# load t241 to i3
    movq -800(%rbp), %rcx
    movq %rcx, -24(%rbp)
    jmp for_start_142
for_end_142: 
    movq $0, %rax
    movq %rax, -808(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -808(%rbp), %rsi # t242 # t242
    movq $0, %rax
    call mematb
    movq %rax, -816(%rbp)
    movq $0, %rax
    movq %rax, -824(%rbp)
    movq -816(%rbp), %rax # t243 # t243
    movq -824(%rbp), %rdi # t244 # t244
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -832(%rbp)
    movq $0, %rax
    movq %rax, -840(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -840(%rbp), %rsi # t246 # t246
    movq $0, %rax
    call mematb
    movq %rax, -848(%rbp)
    movq $4, %rax
    movq %rax, -856(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -856(%rbp), %rsi # t248 # t248
    movq $0, %rax
    call mematb
    movq %rax, -864(%rbp)
    movq -848(%rbp), %rsi # t247 # t247
    movq -864(%rbp), %rdi # t249 # t249
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -872(%rbp)
    movq -832(%rbp), %rsi # t245 # t245
    cmpq $0, %rsi
    setne %al
    movq -872(%rbp), %rdi # t250 # t250
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -880(%rbp)
    movq $4, %rax
    movq %rax, -888(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -888(%rbp), %rsi # t252 # t252
    movq $0, %rax
    call mematb
    movq %rax, -896(%rbp)
    movq $8, %rax
    movq %rax, -904(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -904(%rbp), %rsi # t254 # t254
    movq $0, %rax
    call mematb
    movq %rax, -912(%rbp)
    movq -896(%rbp), %rsi # t253 # t253
    movq -912(%rbp), %rdi # t255 # t255
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -920(%rbp)
    movq -880(%rbp), %rsi # t251 # t251
    cmpq $0, %rsi
    setne %al
    movq -920(%rbp), %rdi # t256 # t256
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -928(%rbp)
    cmpq $0, %rax
    je sublabel_else_258
sublabel_if_258: 
    movq $0, %rax
    movq %rax, -936(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -936(%rbp), %rsi # t259 # t259
    movq $0, %rax
    call mematb
    movq %rax, -944(%rbp)
    leave
    ret
    jmp sublabel_end_258
sublabel_else_258: 
sublabel_end_258: 
    movq $2, %rax
    movq %rax, -952(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -952(%rbp), %rsi # t261 # t261
    movq $0, %rax
    call mematb
    movq %rax, -960(%rbp)
    movq $0, %rax
    movq %rax, -968(%rbp)
    movq -960(%rbp), %rax # t262 # t262
    movq -968(%rbp), %rdi # t263 # t263
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -976(%rbp)
    movq $2, %rax
    movq %rax, -984(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -984(%rbp), %rsi # t265 # t265
    movq $0, %rax
    call mematb
    movq %rax, -992(%rbp)
    movq $4, %rax
    movq %rax, -1000(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -1000(%rbp), %rsi # t267 # t267
    movq $0, %rax
    call mematb
    movq %rax, -1008(%rbp)
    movq -992(%rbp), %rsi # t266 # t266
    movq -1008(%rbp), %rdi # t268 # t268
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -1016(%rbp)
    movq -976(%rbp), %rsi # t264 # t264
    cmpq $0, %rsi
    setne %al
    movq -1016(%rbp), %rdi # t269 # t269
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -1024(%rbp)
    movq $4, %rax
    movq %rax, -1032(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -1032(%rbp), %rsi # t271 # t271
    movq $0, %rax
    call mematb
    movq %rax, -1040(%rbp)
    movq $6, %rax
    movq %rax, -1048(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -1048(%rbp), %rsi # t273 # t273
    movq $0, %rax
    call mematb
    movq %rax, -1056(%rbp)
    movq -1040(%rbp), %rsi # t272 # t272
    movq -1056(%rbp), %rdi # t274 # t274
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -1064(%rbp)
    movq -1024(%rbp), %rsi # t270 # t270
    cmpq $0, %rsi
    setne %al
    movq -1064(%rbp), %rdi # t275 # t275
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -1072(%rbp)
    cmpq $0, %rax
    je sublabel_else_277
sublabel_if_277: 
    movq $2, %rax
    movq %rax, -1080(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -1080(%rbp), %rsi # t278 # t278
    movq $0, %rax
    call mematb
    movq %rax, -1088(%rbp)
    leave
    ret
    jmp sublabel_end_277
sublabel_else_277: 
sublabel_end_277: 
    movq $1, %rax
    movq %rax, -1096(%rbp)
    movq $0, %rax
    movq %rax, -1104(%rbp)
for_start_280: 
    movq -1104(%rbp), %rax # i4 # i4
    movq %rax, -1112(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -1120(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -1128(%rbp)
    movq -1120(%rbp), %rax # t283 # t283
    imulq -1128(%rbp), %rax
    movq %rax, -1136(%rbp)
    movq -1112(%rbp), %rax # t282 # t282
    movq -1136(%rbp), %rdi # t285 # t285
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -1144(%rbp)
    movq -1144(%rbp), %rax # t286 # t286
    cmpq $0, %rax
    je for_end_280
    movq -1104(%rbp), %rax # i4 # i4
    movq %rax, -1152(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -1152(%rbp), %rsi # t287 # t287
    movq $0, %rax
    call mematb
    movq %rax, -1160(%rbp)
    movq $0, %rax
    movq %rax, -1168(%rbp)
    movq -1160(%rbp), %rsi # t288 # t288
    movq -1168(%rbp), %rdi # t289 # t289
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -1176(%rbp)
    movq -1176(%rbp), %rax # t290 # t290
    cmpq $0, %rax
    je sublabel_else_291
sublabel_if_291: 
    movq $0, %rcx # here
    movq %rcx, -1096(%rbp)
    jmp for_end_280
    jmp sublabel_end_291
sublabel_else_291: 
sublabel_end_291: 
for_post_281: 
    movq -1104(%rbp), %rax # i4 # i4
    movq %rax, -1184(%rbp)
    movq $1, %rax
    movq %rax, -1192(%rbp)
    movq -1184(%rbp), %rax # t292 # t292
    addq -1192(%rbp), %rax
    movq %rax, -1200(%rbp)
# load t294 to i4
    movq -1200(%rbp), %rcx
    movq %rcx, -1104(%rbp)
    jmp for_start_280
for_end_280: 
    movq -1096(%rbp), %rax # is_draw # is_draw
    movq %rax, -1208(%rbp)
    movq $1, %rax
    movq %rax, -1216(%rbp)
    movq -1208(%rbp), %rsi # t295 # t295
    movq -1216(%rbp), %rdi # t296 # t296
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -1224(%rbp)
    movq -1224(%rbp), %rax # t297 # t297
    cmpq $0, %rax
    je sublabel_else_298
sublabel_if_298: 
    movq $3, %rax
    movq %rax, -1232(%rbp)
    leave
    ret
    jmp sublabel_end_298
sublabel_else_298: 
sublabel_end_298: 
    movq $0, %rax
    movq %rax, -1240(%rbp)
    leave
    ret
.globl init
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $848, %rsp
    movq $0, %rcx
    movq $3, %rax
    movq %rax, -8(%rbp)
    movq $0, %rax
    call sdl_init
    movq %rax, -16(%rbp)
    leaq t302(%rip), %rax
    movq %rax, -24(%rbp)
    movq $600, %rax
    movq %rax, -32(%rbp)
    movq $600, %rax
    movq %rax, -40(%rbp)
    movq -24(%rbp), %rdi # t302 # t302
    movq -32(%rbp), %rsi # t303 # t303
    movq -40(%rbp), %rdx # t304 # t304
    movq $0, %rax
    call sdl_create
    movq %rax, -48(%rbp)
    movq -8(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -56(%rbp)
    movq -56(%rbp), %rdi # t306 # t306
    movq $0, %rax
    call allocate_grid
    movq %rax, -64(%rbp)
    movq %rax, -72(%rbp)
    movq $1, %rax
    movq %rax, -80(%rbp)
    movq $0, %rax
    movq %rax, -88(%rbp)
    movq $0, %rax
    movq %rax, -96(%rbp)
while_start_308: 
    movq $0, %rax
    call sdl_pump
    movq %rax, -104(%rbp)
    cmpq $0, %rax
    je while_end_308
    movq $0, %rax
    call sdl_clear
    movq %rax, -112(%rbp)
    movq -72(%rbp), %rdi # grid # grid
    movq $0, %rax
    call draw_grid
    movq %rax, -120(%rbp)
    movq -88(%rbp), %rax # game_over # game_over
    movq %rax, -128(%rbp)
    movq $1, %rax
    movq %rax, -136(%rbp)
    movq -128(%rbp), %rsi # t312 # t312
    movq -136(%rbp), %rdi # t313 # t313
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -144(%rbp)
    movq -144(%rbp), %rax # t314 # t314
    cmpq $0, %rax
    je sublabel_else_315
sublabel_if_315: 
    movq $255, %rax
    movq %rax, -152(%rbp)
    movq $255, %rax
    movq %rax, -160(%rbp)
    movq $255, %rax
    movq %rax, -168(%rbp)
    movq $255, %rax
    movq %rax, -176(%rbp)
    movq -152(%rbp), %rdi # t316 # t316
    movq -160(%rbp), %rsi # t317 # t317
    movq -168(%rbp), %rdx # t318 # t318
    movq -176(%rbp), %rcx # t319 # t319
    movq $0, %rax
    call sdl_settextcolor
    movq %rax, -184(%rbp)
    movq -96(%rbp), %rax # game_index # game_index
    movq %rax, -192(%rbp)
    movq $1, %rax
    movq %rax, -200(%rbp)
    movq -192(%rbp), %rsi # t321 # t321
    movq -200(%rbp), %rdi # t322 # t322
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -208(%rbp)
    movq -208(%rbp), %rax # t323 # t323
    cmpq $0, %rax
    je sublabel_else_324
sublabel_if_324: 
    movq $15, %rax
    movq %rax, -216(%rbp)
    movq $15, %rax
    movq %rax, -224(%rbp)
    leaq t327(%rip), %rax
    movq %rax, -232(%rbp)
    movq -216(%rbp), %rdi # t325 # t325
    movq -224(%rbp), %rsi # t326 # t326
    movq -232(%rbp), %rdx # t327 # t327
    movq $0, %rax
    call sdl_printtext
    movq %rax, -240(%rbp)
    jmp sublabel_end_324
sublabel_else_324: 
sublabel_end_324: 
    movq -96(%rbp), %rax # game_index # game_index
    movq %rax, -248(%rbp)
    movq $2, %rax
    movq %rax, -256(%rbp)
    movq -248(%rbp), %rsi # t329 # t329
    movq -256(%rbp), %rdi # t330 # t330
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -264(%rbp)
    movq -264(%rbp), %rax # t331 # t331
    cmpq $0, %rax
    je sublabel_else_332
sublabel_if_332: 
    movq $15, %rax
    movq %rax, -272(%rbp)
    movq $15, %rax
    movq %rax, -280(%rbp)
    leaq t335(%rip), %rax
    movq %rax, -288(%rbp)
    movq -272(%rbp), %rdi # t333 # t333
    movq -280(%rbp), %rsi # t334 # t334
    movq -288(%rbp), %rdx # t335 # t335
    movq $0, %rax
    call sdl_printtext
    movq %rax, -296(%rbp)
    jmp sublabel_end_332
sublabel_else_332: 
sublabel_end_332: 
    movq -96(%rbp), %rax # game_index # game_index
    movq %rax, -304(%rbp)
    movq $3, %rax
    movq %rax, -312(%rbp)
    movq -304(%rbp), %rsi # t337 # t337
    movq -312(%rbp), %rdi # t338 # t338
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -320(%rbp)
    movq -320(%rbp), %rax # t339 # t339
    cmpq $0, %rax
    je sublabel_else_340
sublabel_if_340: 
    movq $15, %rax
    movq %rax, -328(%rbp)
    movq $15, %rax
    movq %rax, -336(%rbp)
    leaq t343(%rip), %rax
    movq %rax, -344(%rbp)
    movq -328(%rbp), %rdi # t341 # t341
    movq -336(%rbp), %rsi # t342 # t342
    movq -344(%rbp), %rdx # t343 # t343
    movq $0, %rax
    call sdl_printtext
    movq %rax, -352(%rbp)
    jmp sublabel_end_340
sublabel_else_340: 
sublabel_end_340: 
    jmp sublabel_end_315
sublabel_else_315: 
sublabel_end_315: 
    movq $0, %rax
    call sdl_flip
    movq %rax, -360(%rbp)
    movq -88(%rbp), %rax # game_over # game_over
    movq %rax, -368(%rbp)
    movq $0, %rax
    movq %rax, -376(%rbp)
    movq -368(%rbp), %rsi # t346 # t346
    movq -376(%rbp), %rdi # t347 # t347
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -384(%rbp)
    movq -384(%rbp), %rax # t348 # t348
    cmpq $0, %rax
    je sublabel_else_349
sublabel_if_349: 
    movq $0, %rax
    call sdl_mousedown
    movq %rax, -392(%rbp)
    cmpq $0, %rax
    je sublabel_else_351
sublabel_if_351: 
    movq $0, %rax
    call sdl_mousex
    movq %rax, -400(%rbp)
    movq %rax, -408(%rbp)
    movq $0, %rax
    call sdl_mousey
    movq %rax, -416(%rbp)
    movq %rax, -424(%rbp)
    movq $200, %rax
    movq %rax, -432(%rbp)
    movq -408(%rbp), %rax # mx # mx
    cqto
    idivq -432(%rbp)
    movq %rax, -440(%rbp)
    movq %rax, -448(%rbp)
    movq $200, %rax
    movq %rax, -456(%rbp)
    movq -424(%rbp), %rax # my # my
    cqto
    idivq -456(%rbp)
    movq %rax, -464(%rbp)
    movq %rax, -472(%rbp)
    movq $0, %rax
    movq %rax, -480(%rbp)
    movq -448(%rbp), %rax # grid_x # grid_x
    movq -480(%rbp), %rdi # t358 # t358
    cmpq %rdi, %rax
    setge %cl
    movzbq %cl, %rdx
    movq %rdx, -488(%rbp)
    movq -8(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -496(%rbp)
    movq -448(%rbp), %rax # grid_x # grid_x
    movq -496(%rbp), %rdi # t360 # t360
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -504(%rbp)
    movq -488(%rbp), %rsi # t359 # t359
    cmpq $0, %rsi
    setne %al
    movq -504(%rbp), %rdi # t361 # t361
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -512(%rbp)
    movq $0, %rax
    movq %rax, -520(%rbp)
    movq -472(%rbp), %rax # grid_y # grid_y
    movq -520(%rbp), %rdi # t363 # t363
    cmpq %rdi, %rax
    setge %cl
    movzbq %cl, %rdx
    movq %rdx, -528(%rbp)
    movq -512(%rbp), %rsi # t362 # t362
    cmpq $0, %rsi
    setne %al
    movq -528(%rbp), %rdi # t364 # t364
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -536(%rbp)
    movq -8(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -544(%rbp)
    movq -472(%rbp), %rax # grid_y # grid_y
    movq -544(%rbp), %rdi # t366 # t366
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -552(%rbp)
    movq -536(%rbp), %rsi # t365 # t365
    cmpq $0, %rsi
    setne %al
    movq -552(%rbp), %rdi # t367 # t367
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -560(%rbp)
    cmpq $0, %rax
    je sublabel_else_369
sublabel_if_369: 
    movq -8(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -568(%rbp)
    movq -472(%rbp), %rax # grid_y # grid_y
    imulq -568(%rbp), %rax
    movq %rax, -576(%rbp)
    addq -448(%rbp), %rax
    movq %rax, -584(%rbp)
    movq %rax, -592(%rbp)
    movq -72(%rbp), %rdi # grid # grid
    movq -592(%rbp), %rsi # index # index
    movq $0, %rax
    call mematb
    movq %rax, -600(%rbp)
    movq $0, %rax
    movq %rax, -608(%rbp)
    movq -600(%rbp), %rsi # t373 # t373
    movq -608(%rbp), %rdi # t374 # t374
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -616(%rbp)
    movq -616(%rbp), %rax # t375 # t375
    cmpq $0, %rax
    je sublabel_else_376
sublabel_if_376: 
    movq -80(%rbp), %rax # current_player # current_player
    movq %rax, -624(%rbp)
    movq -72(%rbp), %rdi # grid # grid
    movq -592(%rbp), %rsi # index # index
    movq -624(%rbp), %rdx # t377 # t377
    movq $0, %rax
    call memstoreb
    movq %rax, -632(%rbp)
    movq -72(%rbp), %rdi # grid # grid
    movq $0, %rax
    call check_winner
    movq %rax, -640(%rbp)
# load t379 to game_index
    movq -640(%rbp), %rcx
    movq %rcx, -96(%rbp)
    movq $0, %rax
    movq %rax, -648(%rbp)
    movq -96(%rbp), %rax # game_index # game_index
    movq -648(%rbp), %rdi # t380 # t380
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -656(%rbp)
    movq -656(%rbp), %rax # t381 # t381
    cmpq $0, %rax
    je sublabel_else_382
sublabel_if_382: 
    movq $1, %rcx # here
    movq %rcx, -88(%rbp)
    jmp sublabel_end_382
sublabel_else_382: 
    movq -80(%rbp), %rax # current_player # current_player
    movq %rax, -664(%rbp)
    movq $1, %rax
    movq %rax, -672(%rbp)
    movq -664(%rbp), %rsi # t383 # t383
    movq -672(%rbp), %rdi # t384 # t384
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -680(%rbp)
    movq -680(%rbp), %rax # t385 # t385
    cmpq $0, %rax
    je sublabel_else_386
sublabel_if_386: 
    movq $2, %rcx # here
    movq %rcx, -80(%rbp)
    jmp sublabel_end_386
sublabel_else_386: 
    movq $1, %rcx # here
    movq %rcx, -80(%rbp)
sublabel_end_386: 
sublabel_end_382: 
    jmp sublabel_end_376
sublabel_else_376: 
sublabel_end_376: 
    jmp sublabel_end_369
sublabel_else_369: 
sublabel_end_369: 
    jmp sublabel_end_351
sublabel_else_351: 
sublabel_end_351: 
    jmp sublabel_end_349
sublabel_else_349: 
sublabel_end_349: 
    jmp while_start_308
while_end_308: 
    movq $1, %rax
    movq %rax, -688(%rbp)
    movq -96(%rbp), %rsi # game_index # game_index
    movq -688(%rbp), %rdi # t387 # t387
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -696(%rbp)
    movq $2, %rax
    movq %rax, -704(%rbp)
    movq -96(%rbp), %rsi # game_index # game_index
    movq -704(%rbp), %rdi # t389 # t389
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -712(%rbp)
    movq -696(%rbp), %rdi # t388 # t388
    cmpq $0, %rdi
    setne %al
    movq -712(%rbp), %rdx # t390 # t390
    cmpq $0, %rdx
    setne %cl
    orb %al, %cl
    movzbq %cl, %rdx
    movq %rdx, -720(%rbp)
    movq -720(%rbp), %rax # t391 # t391
    cmpq $0, %rax
    je sublabel_else_392
sublabel_if_392: 
    leaq t393(%rip), %rax
    movq %rax, -728(%rbp)
    movq -728(%rbp), %rdi # t393 # t393
    movq -96(%rbp), %rsi # game_index # game_index
    movq $0, %rax
    call printf
    movq %rax, -736(%rbp)
    jmp sublabel_end_392
sublabel_else_392: 
    leaq t395(%rip), %rax
    movq %rax, -744(%rbp)
    movq -744(%rbp), %rdi # t395 # t395
    movq $0, %rax
    call printf
    movq %rax, -752(%rbp)
sublabel_end_392: 
    movq -72(%rbp), %rdi # grid # grid
    movq $0, %rax
    call free
    movq %rax, -760(%rbp)
    movq $0, %rax
    call sdl_release
    movq %rax, -768(%rbp)
    movq $0, %rax
    call sdl_quit
    movq %rax, -776(%rbp)
    movq $0, %rax
    movq %rax, -784(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

