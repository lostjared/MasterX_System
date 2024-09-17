.section .data
t337: .asciz "It's a Draw!"
t329: .asciz "Player 2 Wins!"
t321: .asciz "Player 1 Wins!"
t296: .asciz "Tic-Tac-Toe"
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
    subq $1248, %rsp
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
    movq $0, %rax
    movq %rax, -104(%rbp)
    movq -96(%rbp), %rax # t152 # t152
    movq -104(%rbp), %rdi # t153 # t153
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -112(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -120(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -128(%rbp)
    movq -120(%rbp), %rax # t155 # t155
    imulq -128(%rbp), %rax
    movq %rax, -136(%rbp)
    movq $0, %rax
    movq %rax, -144(%rbp)
    movq -136(%rbp), %rax # t157 # t157
    addq -144(%rbp), %rax
    movq %rax, -152(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -152(%rbp), %rsi # t159 # t159
    movq $0, %rax
    call mematb
    movq %rax, -160(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -168(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -176(%rbp)
    movq -168(%rbp), %rax # t161 # t161
    imulq -176(%rbp), %rax
    movq %rax, -184(%rbp)
    movq $1, %rax
    movq %rax, -192(%rbp)
    movq -184(%rbp), %rax # t163 # t163
    addq -192(%rbp), %rax
    movq %rax, -200(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -200(%rbp), %rsi # t165 # t165
    movq $0, %rax
    call mematb
    movq %rax, -208(%rbp)
    movq -160(%rbp), %rsi # t160 # t160
    movq -208(%rbp), %rdi # t166 # t166
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -216(%rbp)
    movq -112(%rbp), %rsi # t154 # t154
    cmpq $0, %rsi
    setne %al
    movq -216(%rbp), %rdi # t167 # t167
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -224(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -232(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -240(%rbp)
    movq -232(%rbp), %rax # t169 # t169
    imulq -240(%rbp), %rax
    movq %rax, -248(%rbp)
    movq $1, %rax
    movq %rax, -256(%rbp)
    movq -248(%rbp), %rax # t171 # t171
    addq -256(%rbp), %rax
    movq %rax, -264(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -264(%rbp), %rsi # t173 # t173
    movq $0, %rax
    call mematb
    movq %rax, -272(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -280(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -288(%rbp)
    movq -280(%rbp), %rax # t175 # t175
    imulq -288(%rbp), %rax
    movq %rax, -296(%rbp)
    movq $2, %rax
    movq %rax, -304(%rbp)
    movq -296(%rbp), %rax # t177 # t177
    addq -304(%rbp), %rax
    movq %rax, -312(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -312(%rbp), %rsi # t179 # t179
    movq $0, %rax
    call mematb
    movq %rax, -320(%rbp)
    movq -272(%rbp), %rsi # t174 # t174
    movq -320(%rbp), %rdi # t180 # t180
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -328(%rbp)
    movq -224(%rbp), %rsi # t168 # t168
    cmpq $0, %rsi
    setne %al
    movq -328(%rbp), %rdi # t181 # t181
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -336(%rbp)
    cmpq $0, %rax
    je sublabel_else_183
sublabel_if_183: 
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -344(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -352(%rbp)
    movq -344(%rbp), %rax # t184 # t184
    imulq -352(%rbp), %rax
    movq %rax, -360(%rbp)
    movq $0, %rax
    movq %rax, -368(%rbp)
    movq -360(%rbp), %rax # t186 # t186
    addq -368(%rbp), %rax
    movq %rax, -376(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -376(%rbp), %rsi # t188 # t188
    movq $0, %rax
    call mematb
    movq %rax, -384(%rbp)
    leave
    ret
    jmp sublabel_end_183
sublabel_else_183: 
sublabel_end_183: 
    movq $0, %rax
    movq %rax, -392(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -400(%rbp)
    movq -392(%rbp), %rax # t190 # t190
    imulq -400(%rbp), %rax
    movq %rax, -408(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -416(%rbp)
    movq -408(%rbp), %rax # t192 # t192
    addq -416(%rbp), %rax
    movq %rax, -424(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -424(%rbp), %rsi # t194 # t194
    movq $0, %rax
    call mematb
    movq %rax, -432(%rbp)
    movq $0, %rax
    movq %rax, -440(%rbp)
    movq -432(%rbp), %rax # t195 # t195
    movq -440(%rbp), %rdi # t196 # t196
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -448(%rbp)
    movq $0, %rax
    movq %rax, -456(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -464(%rbp)
    movq -456(%rbp), %rax # t198 # t198
    imulq -464(%rbp), %rax
    movq %rax, -472(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -480(%rbp)
    movq -472(%rbp), %rax # t200 # t200
    addq -480(%rbp), %rax
    movq %rax, -488(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -488(%rbp), %rsi # t202 # t202
    movq $0, %rax
    call mematb
    movq %rax, -496(%rbp)
    movq $1, %rax
    movq %rax, -504(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -512(%rbp)
    movq -504(%rbp), %rax # t204 # t204
    imulq -512(%rbp), %rax
    movq %rax, -520(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -528(%rbp)
    movq -520(%rbp), %rax # t206 # t206
    addq -528(%rbp), %rax
    movq %rax, -536(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -536(%rbp), %rsi # t208 # t208
    movq $0, %rax
    call mematb
    movq %rax, -544(%rbp)
    movq -496(%rbp), %rsi # t203 # t203
    movq -544(%rbp), %rdi # t209 # t209
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -552(%rbp)
    movq -448(%rbp), %rsi # t197 # t197
    cmpq $0, %rsi
    setne %al
    movq -552(%rbp), %rdi # t210 # t210
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -560(%rbp)
    movq $1, %rax
    movq %rax, -568(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -576(%rbp)
    movq -568(%rbp), %rax # t212 # t212
    imulq -576(%rbp), %rax
    movq %rax, -584(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -592(%rbp)
    movq -584(%rbp), %rax # t214 # t214
    addq -592(%rbp), %rax
    movq %rax, -600(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -600(%rbp), %rsi # t216 # t216
    movq $0, %rax
    call mematb
    movq %rax, -608(%rbp)
    movq $2, %rax
    movq %rax, -616(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -624(%rbp)
    movq -616(%rbp), %rax # t218 # t218
    imulq -624(%rbp), %rax
    movq %rax, -632(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -640(%rbp)
    movq -632(%rbp), %rax # t220 # t220
    addq -640(%rbp), %rax
    movq %rax, -648(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -648(%rbp), %rsi # t222 # t222
    movq $0, %rax
    call mematb
    movq %rax, -656(%rbp)
    movq -608(%rbp), %rsi # t217 # t217
    movq -656(%rbp), %rdi # t223 # t223
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -664(%rbp)
    movq -560(%rbp), %rsi # t211 # t211
    cmpq $0, %rsi
    setne %al
    movq -664(%rbp), %rdi # t224 # t224
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -672(%rbp)
    cmpq $0, %rax
    je sublabel_else_226
sublabel_if_226: 
    movq $0, %rax
    movq %rax, -680(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -688(%rbp)
    movq -680(%rbp), %rax # t227 # t227
    imulq -688(%rbp), %rax
    movq %rax, -696(%rbp)
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -704(%rbp)
    movq -696(%rbp), %rax # t229 # t229
    addq -704(%rbp), %rax
    movq %rax, -712(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -712(%rbp), %rsi # t231 # t231
    movq $0, %rax
    call mematb
    movq %rax, -720(%rbp)
    leave
    ret
    jmp sublabel_end_226
sublabel_else_226: 
sublabel_end_226: 
for_post_143: 
    movq -24(%rbp), %rax # i3 # i3
    movq %rax, -728(%rbp)
    movq $1, %rax
    movq %rax, -736(%rbp)
    movq -728(%rbp), %rax # t233 # t233
    addq -736(%rbp), %rax
    movq %rax, -744(%rbp)
# load t235 to i3
    movq -744(%rbp), %rcx
    movq %rcx, -24(%rbp)
    jmp for_start_142
for_end_142: 
    movq $0, %rax
    movq %rax, -752(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -752(%rbp), %rsi # t236 # t236
    movq $0, %rax
    call mematb
    movq %rax, -760(%rbp)
    movq $0, %rax
    movq %rax, -768(%rbp)
    movq -760(%rbp), %rax # t237 # t237
    movq -768(%rbp), %rdi # t238 # t238
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -776(%rbp)
    movq $0, %rax
    movq %rax, -784(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -784(%rbp), %rsi # t240 # t240
    movq $0, %rax
    call mematb
    movq %rax, -792(%rbp)
    movq $4, %rax
    movq %rax, -800(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -800(%rbp), %rsi # t242 # t242
    movq $0, %rax
    call mematb
    movq %rax, -808(%rbp)
    movq -792(%rbp), %rsi # t241 # t241
    movq -808(%rbp), %rdi # t243 # t243
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -816(%rbp)
    movq -776(%rbp), %rsi # t239 # t239
    cmpq $0, %rsi
    setne %al
    movq -816(%rbp), %rdi # t244 # t244
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -824(%rbp)
    movq $4, %rax
    movq %rax, -832(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -832(%rbp), %rsi # t246 # t246
    movq $0, %rax
    call mematb
    movq %rax, -840(%rbp)
    movq $8, %rax
    movq %rax, -848(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -848(%rbp), %rsi # t248 # t248
    movq $0, %rax
    call mematb
    movq %rax, -856(%rbp)
    movq -840(%rbp), %rsi # t247 # t247
    movq -856(%rbp), %rdi # t249 # t249
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -864(%rbp)
    movq -824(%rbp), %rsi # t245 # t245
    cmpq $0, %rsi
    setne %al
    movq -864(%rbp), %rdi # t250 # t250
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -872(%rbp)
    cmpq $0, %rax
    je sublabel_else_252
sublabel_if_252: 
    movq $0, %rax
    movq %rax, -880(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -880(%rbp), %rsi # t253 # t253
    movq $0, %rax
    call mematb
    movq %rax, -888(%rbp)
    leave
    ret
    jmp sublabel_end_252
sublabel_else_252: 
sublabel_end_252: 
    movq $2, %rax
    movq %rax, -896(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -896(%rbp), %rsi # t255 # t255
    movq $0, %rax
    call mematb
    movq %rax, -904(%rbp)
    movq $0, %rax
    movq %rax, -912(%rbp)
    movq -904(%rbp), %rax # t256 # t256
    movq -912(%rbp), %rdi # t257 # t257
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -920(%rbp)
    movq $2, %rax
    movq %rax, -928(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -928(%rbp), %rsi # t259 # t259
    movq $0, %rax
    call mematb
    movq %rax, -936(%rbp)
    movq $4, %rax
    movq %rax, -944(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -944(%rbp), %rsi # t261 # t261
    movq $0, %rax
    call mematb
    movq %rax, -952(%rbp)
    movq -936(%rbp), %rsi # t260 # t260
    movq -952(%rbp), %rdi # t262 # t262
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -960(%rbp)
    movq -920(%rbp), %rsi # t258 # t258
    cmpq $0, %rsi
    setne %al
    movq -960(%rbp), %rdi # t263 # t263
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -968(%rbp)
    movq $4, %rax
    movq %rax, -976(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -976(%rbp), %rsi # t265 # t265
    movq $0, %rax
    call mematb
    movq %rax, -984(%rbp)
    movq $6, %rax
    movq %rax, -992(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -992(%rbp), %rsi # t267 # t267
    movq $0, %rax
    call mematb
    movq %rax, -1000(%rbp)
    movq -984(%rbp), %rsi # t266 # t266
    movq -1000(%rbp), %rdi # t268 # t268
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -1008(%rbp)
    movq -968(%rbp), %rsi # t264 # t264
    cmpq $0, %rsi
    setne %al
    movq -1008(%rbp), %rdi # t269 # t269
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -1016(%rbp)
    cmpq $0, %rax
    je sublabel_else_271
sublabel_if_271: 
    movq $2, %rax
    movq %rax, -1024(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -1024(%rbp), %rsi # t272 # t272
    movq $0, %rax
    call mematb
    movq %rax, -1032(%rbp)
    leave
    ret
    jmp sublabel_end_271
sublabel_else_271: 
sublabel_end_271: 
    movq $1, %rax
    movq %rax, -1040(%rbp)
    movq $0, %rax
    movq %rax, -1048(%rbp)
for_start_274: 
    movq -1048(%rbp), %rax # i4 # i4
    movq %rax, -1056(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -1064(%rbp)
    movq -16(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -1072(%rbp)
    movq -1064(%rbp), %rax # t277 # t277
    imulq -1072(%rbp), %rax
    movq %rax, -1080(%rbp)
    movq -1056(%rbp), %rax # t276 # t276
    movq -1080(%rbp), %rdi # t279 # t279
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -1088(%rbp)
    movq -1088(%rbp), %rax # t280 # t280
    cmpq $0, %rax
    je for_end_274
    movq -1048(%rbp), %rax # i4 # i4
    movq %rax, -1096(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -1096(%rbp), %rsi # t281 # t281
    movq $0, %rax
    call mematb
    movq %rax, -1104(%rbp)
    movq $0, %rax
    movq %rax, -1112(%rbp)
    movq -1104(%rbp), %rsi # t282 # t282
    movq -1112(%rbp), %rdi # t283 # t283
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -1120(%rbp)
    movq -1120(%rbp), %rax # t284 # t284
    cmpq $0, %rax
    je sublabel_else_285
sublabel_if_285: 
    movq $0, %rcx # here
    movq %rcx, -1040(%rbp)
    jmp for_end_274
    jmp sublabel_end_285
sublabel_else_285: 
sublabel_end_285: 
for_post_275: 
    movq -1048(%rbp), %rax # i4 # i4
    movq %rax, -1128(%rbp)
    movq $1, %rax
    movq %rax, -1136(%rbp)
    movq -1128(%rbp), %rax # t286 # t286
    addq -1136(%rbp), %rax
    movq %rax, -1144(%rbp)
# load t288 to i4
    movq -1144(%rbp), %rcx
    movq %rcx, -1048(%rbp)
    jmp for_start_274
for_end_274: 
    movq -1040(%rbp), %rax # is_draw # is_draw
    movq %rax, -1152(%rbp)
    movq $1, %rax
    movq %rax, -1160(%rbp)
    movq -1152(%rbp), %rsi # t289 # t289
    movq -1160(%rbp), %rdi # t290 # t290
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -1168(%rbp)
    movq -1168(%rbp), %rax # t291 # t291
    cmpq $0, %rax
    je sublabel_else_292
sublabel_if_292: 
    movq $3, %rax
    movq %rax, -1176(%rbp)
    leave
    ret
    jmp sublabel_end_292
sublabel_else_292: 
sublabel_end_292: 
    movq $0, %rax
    movq %rax, -1184(%rbp)
    leave
    ret
.globl init
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $784, %rsp
    movq $0, %rcx
    movq $3, %rax
    movq %rax, -8(%rbp)
    movq $0, %rax
    call sdl_init
    movq %rax, -16(%rbp)
    leaq t296(%rip), %rax
    movq %rax, -24(%rbp)
    movq $600, %rax
    movq %rax, -32(%rbp)
    movq $600, %rax
    movq %rax, -40(%rbp)
    movq -24(%rbp), %rdi # t296 # t296
    movq -32(%rbp), %rsi # t297 # t297
    movq -40(%rbp), %rdx # t298 # t298
    movq $0, %rax
    call sdl_create
    movq %rax, -48(%rbp)
    movq -8(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -56(%rbp)
    movq -56(%rbp), %rdi # t300 # t300
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
while_start_302: 
    movq $0, %rax
    call sdl_pump
    movq %rax, -104(%rbp)
    cmpq $0, %rax
    je while_end_302
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
    movq -128(%rbp), %rsi # t306 # t306
    movq -136(%rbp), %rdi # t307 # t307
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -144(%rbp)
    movq -144(%rbp), %rax # t308 # t308
    cmpq $0, %rax
    je sublabel_else_309
sublabel_if_309: 
    movq $255, %rax
    movq %rax, -152(%rbp)
    movq $255, %rax
    movq %rax, -160(%rbp)
    movq $255, %rax
    movq %rax, -168(%rbp)
    movq $255, %rax
    movq %rax, -176(%rbp)
    movq -152(%rbp), %rdi # t310 # t310
    movq -160(%rbp), %rsi # t311 # t311
    movq -168(%rbp), %rdx # t312 # t312
    movq -176(%rbp), %rcx # t313 # t313
    movq $0, %rax
    call sdl_settextcolor
    movq %rax, -184(%rbp)
    movq -96(%rbp), %rax # game_index # game_index
    movq %rax, -192(%rbp)
    movq $1, %rax
    movq %rax, -200(%rbp)
    movq -192(%rbp), %rsi # t315 # t315
    movq -200(%rbp), %rdi # t316 # t316
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -208(%rbp)
    movq -208(%rbp), %rax # t317 # t317
    cmpq $0, %rax
    je sublabel_else_318
sublabel_if_318: 
    movq $15, %rax
    movq %rax, -216(%rbp)
    movq $15, %rax
    movq %rax, -224(%rbp)
    leaq t321(%rip), %rax
    movq %rax, -232(%rbp)
    movq -216(%rbp), %rdi # t319 # t319
    movq -224(%rbp), %rsi # t320 # t320
    movq -232(%rbp), %rdx # t321 # t321
    movq $0, %rax
    call sdl_printtext
    movq %rax, -240(%rbp)
    jmp sublabel_end_318
sublabel_else_318: 
sublabel_end_318: 
    movq -96(%rbp), %rax # game_index # game_index
    movq %rax, -248(%rbp)
    movq $2, %rax
    movq %rax, -256(%rbp)
    movq -248(%rbp), %rsi # t323 # t323
    movq -256(%rbp), %rdi # t324 # t324
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -264(%rbp)
    movq -264(%rbp), %rax # t325 # t325
    cmpq $0, %rax
    je sublabel_else_326
sublabel_if_326: 
    movq $15, %rax
    movq %rax, -272(%rbp)
    movq $15, %rax
    movq %rax, -280(%rbp)
    leaq t329(%rip), %rax
    movq %rax, -288(%rbp)
    movq -272(%rbp), %rdi # t327 # t327
    movq -280(%rbp), %rsi # t328 # t328
    movq -288(%rbp), %rdx # t329 # t329
    movq $0, %rax
    call sdl_printtext
    movq %rax, -296(%rbp)
    jmp sublabel_end_326
sublabel_else_326: 
sublabel_end_326: 
    movq -96(%rbp), %rax # game_index # game_index
    movq %rax, -304(%rbp)
    movq $3, %rax
    movq %rax, -312(%rbp)
    movq -304(%rbp), %rsi # t331 # t331
    movq -312(%rbp), %rdi # t332 # t332
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -320(%rbp)
    movq -320(%rbp), %rax # t333 # t333
    cmpq $0, %rax
    je sublabel_else_334
sublabel_if_334: 
    movq $15, %rax
    movq %rax, -328(%rbp)
    movq $15, %rax
    movq %rax, -336(%rbp)
    leaq t337(%rip), %rax
    movq %rax, -344(%rbp)
    movq -328(%rbp), %rdi # t335 # t335
    movq -336(%rbp), %rsi # t336 # t336
    movq -344(%rbp), %rdx # t337 # t337
    movq $0, %rax
    call sdl_printtext
    movq %rax, -352(%rbp)
    jmp sublabel_end_334
sublabel_else_334: 
sublabel_end_334: 
    jmp sublabel_end_309
sublabel_else_309: 
sublabel_end_309: 
    movq $0, %rax
    call sdl_flip
    movq %rax, -360(%rbp)
    movq -88(%rbp), %rax # game_over # game_over
    movq %rax, -368(%rbp)
    movq $0, %rax
    movq %rax, -376(%rbp)
    movq -368(%rbp), %rsi # t340 # t340
    movq -376(%rbp), %rdi # t341 # t341
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -384(%rbp)
    movq -384(%rbp), %rax # t342 # t342
    cmpq $0, %rax
    je sublabel_else_343
sublabel_if_343: 
    movq $0, %rax
    call sdl_mousedown
    movq %rax, -392(%rbp)
    cmpq $0, %rax
    je sublabel_else_345
sublabel_if_345: 
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
    movq -480(%rbp), %rdi # t352 # t352
    cmpq %rdi, %rax
    setge %cl
    movzbq %cl, %rdx
    movq %rdx, -488(%rbp)
    movq -8(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -496(%rbp)
    movq -448(%rbp), %rax # grid_x # grid_x
    movq -496(%rbp), %rdi # t354 # t354
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -504(%rbp)
    movq -488(%rbp), %rsi # t353 # t353
    cmpq $0, %rsi
    setne %al
    movq -504(%rbp), %rdi # t355 # t355
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -512(%rbp)
    movq $0, %rax
    movq %rax, -520(%rbp)
    movq -472(%rbp), %rax # grid_y # grid_y
    movq -520(%rbp), %rdi # t357 # t357
    cmpq %rdi, %rax
    setge %cl
    movzbq %cl, %rdx
    movq %rdx, -528(%rbp)
    movq -512(%rbp), %rsi # t356 # t356
    cmpq $0, %rsi
    setne %al
    movq -528(%rbp), %rdi # t358 # t358
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -536(%rbp)
    movq -8(%rbp), %rax # GRID_SIZE # GRID_SIZE
    movq %rax, -544(%rbp)
    movq -472(%rbp), %rax # grid_y # grid_y
    movq -544(%rbp), %rdi # t360 # t360
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -552(%rbp)
    movq -536(%rbp), %rsi # t359 # t359
    cmpq $0, %rsi
    setne %al
    movq -552(%rbp), %rdi # t361 # t361
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -560(%rbp)
    cmpq $0, %rax
    je sublabel_else_363
sublabel_if_363: 
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
    movq -600(%rbp), %rsi # t367 # t367
    movq -608(%rbp), %rdi # t368 # t368
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -616(%rbp)
    movq -616(%rbp), %rax # t369 # t369
    cmpq $0, %rax
    je sublabel_else_370
sublabel_if_370: 
    movq -80(%rbp), %rax # current_player # current_player
    movq %rax, -624(%rbp)
    movq -72(%rbp), %rdi # grid # grid
    movq -592(%rbp), %rsi # index # index
    movq -624(%rbp), %rdx # t371 # t371
    movq $0, %rax
    call memstoreb
    movq %rax, -632(%rbp)
    movq -72(%rbp), %rdi # grid # grid
    movq $0, %rax
    call check_winner
    movq %rax, -640(%rbp)
    movq %rax, -648(%rbp)
    movq $0, %rax
    movq %rax, -656(%rbp)
    movq -648(%rbp), %rax # result # result
    movq -656(%rbp), %rdi # t374 # t374
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -664(%rbp)
    movq -664(%rbp), %rax # t375 # t375
    cmpq $0, %rax
    je sublabel_else_376
sublabel_if_376: 
    movq $1, %rcx # here
    movq %rcx, -88(%rbp)
# load t375 to game_index
    movq -664(%rbp), %rcx
    movq %rcx, -96(%rbp)
    jmp sublabel_end_376
sublabel_else_376: 
    movq -80(%rbp), %rax # current_player # current_player
    movq %rax, -672(%rbp)
    movq $1, %rax
    movq %rax, -680(%rbp)
    movq -672(%rbp), %rsi # t377 # t377
    movq -680(%rbp), %rdi # t378 # t378
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -688(%rbp)
    movq -688(%rbp), %rax # t379 # t379
    cmpq $0, %rax
    je sublabel_else_380
sublabel_if_380: 
    movq $2, %rcx # here
    movq %rcx, -80(%rbp)
    jmp sublabel_end_380
sublabel_else_380: 
    movq $1, %rcx # here
    movq %rcx, -80(%rbp)
sublabel_end_380: 
sublabel_end_376: 
    jmp sublabel_end_370
sublabel_else_370: 
sublabel_end_370: 
    jmp sublabel_end_363
sublabel_else_363: 
sublabel_end_363: 
    jmp sublabel_end_345
sublabel_else_345: 
sublabel_end_345: 
    jmp sublabel_end_343
sublabel_else_343: 
sublabel_end_343: 
    jmp while_start_302
while_end_302: 
    movq -72(%rbp), %rdi # grid # grid
    movq $0, %rax
    call free
    movq %rax, -696(%rbp)
    movq $0, %rax
    call sdl_release
    movq %rax, -704(%rbp)
    movq $0, %rax
    call sdl_quit
    movq %rax, -712(%rbp)
    movq $0, %rax
    movq %rax, -720(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

