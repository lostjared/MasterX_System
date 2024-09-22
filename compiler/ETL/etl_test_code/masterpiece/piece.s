.section .data
t828: .asciz "Game Over Score: "
t806: .asciz "img/gameover.bmp"
t648: .asciz "MasterPiece"
t691: .asciz "Score: "
t669: .asciz "img/puzzle.bmp"
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
.globl set_block_color
set_block_color:
    pushq %rbp
    movq %rsp, %rbp
    subq $400, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq -8(%rbp), %rax # color # color
    movq %rax, -16(%rbp)
    movq $0, %rax
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rsi # t0 # t0
    movq -24(%rbp), %rdi # t1 # t1
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -32(%rbp)
    movq -32(%rbp), %rax # t2 # t2
    cmpq $0, %rax
    je sublabel_else_3
sublabel_if_3: 
    movq $0, %rax
    movq %rax, -40(%rbp)
    leave
    ret
    jmp sublabel_end_3
sublabel_else_3: 
sublabel_end_3: 
    movq -8(%rbp), %rax # color # color
    movq %rax, -48(%rbp)
    movq $1, %rax
    movq %rax, -56(%rbp)
    movq -48(%rbp), %rsi # t5 # t5
    movq -56(%rbp), %rdi # t6 # t6
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -64(%rbp)
    movq -64(%rbp), %rax # t7 # t7
    cmpq $0, %rax
    je sublabel_else_8
sublabel_if_8: 
    movq $255, %rax
    movq %rax, -72(%rbp)
    movq $0, %rax
    movq %rax, -80(%rbp)
    movq $0, %rax
    movq %rax, -88(%rbp)
    movq $255, %rax
    movq %rax, -96(%rbp)
    movq -72(%rbp), %rdi # t9 # t9
    movq -80(%rbp), %rsi # t10 # t10
    movq -88(%rbp), %rdx # t11 # t11
    movq -96(%rbp), %rcx # t12 # t12
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -104(%rbp)
    movq $1, %rax
    movq %rax, -112(%rbp)
    leave
    ret
    jmp sublabel_end_8
sublabel_else_8: 
sublabel_end_8: 
    movq -8(%rbp), %rax # color # color
    movq %rax, -120(%rbp)
    movq $2, %rax
    movq %rax, -128(%rbp)
    movq -120(%rbp), %rsi # t15 # t15
    movq -128(%rbp), %rdi # t16 # t16
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -136(%rbp)
    movq -136(%rbp), %rax # t17 # t17
    cmpq $0, %rax
    je sublabel_else_18
sublabel_if_18: 
    movq $0, %rax
    movq %rax, -144(%rbp)
    movq $255, %rax
    movq %rax, -152(%rbp)
    movq $0, %rax
    movq %rax, -160(%rbp)
    movq $255, %rax
    movq %rax, -168(%rbp)
    movq -144(%rbp), %rdi # t19 # t19
    movq -152(%rbp), %rsi # t20 # t20
    movq -160(%rbp), %rdx # t21 # t21
    movq -168(%rbp), %rcx # t22 # t22
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -176(%rbp)
    movq $2, %rax
    movq %rax, -184(%rbp)
    leave
    ret
    jmp sublabel_end_18
sublabel_else_18: 
sublabel_end_18: 
    movq -8(%rbp), %rax # color # color
    movq %rax, -192(%rbp)
    movq $3, %rax
    movq %rax, -200(%rbp)
    movq -192(%rbp), %rsi # t25 # t25
    movq -200(%rbp), %rdi # t26 # t26
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -208(%rbp)
    movq -208(%rbp), %rax # t27 # t27
    cmpq $0, %rax
    je sublabel_else_28
sublabel_if_28: 
    movq $0, %rax
    movq %rax, -216(%rbp)
    movq $0, %rax
    movq %rax, -224(%rbp)
    movq $255, %rax
    movq %rax, -232(%rbp)
    movq $255, %rax
    movq %rax, -240(%rbp)
    movq -216(%rbp), %rdi # t29 # t29
    movq -224(%rbp), %rsi # t30 # t30
    movq -232(%rbp), %rdx # t31 # t31
    movq -240(%rbp), %rcx # t32 # t32
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -248(%rbp)
    movq $3, %rax
    movq %rax, -256(%rbp)
    leave
    ret
    jmp sublabel_end_28
sublabel_else_28: 
sublabel_end_28: 
    movq -8(%rbp), %rax # color # color
    movq %rax, -264(%rbp)
    movq $4, %rax
    movq %rax, -272(%rbp)
    movq -264(%rbp), %rsi # t35 # t35
    movq -272(%rbp), %rdi # t36 # t36
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -280(%rbp)
    movq -280(%rbp), %rax # t37 # t37
    cmpq $0, %rax
    je sublabel_else_38
sublabel_if_38: 
    movq $255, %rax
    movq %rax, -288(%rbp)
    movq $0, %rax
    movq %rax, -296(%rbp)
    movq $255, %rax
    movq %rax, -304(%rbp)
    movq $25, %rax
    movq %rax, -312(%rbp)
    movq -288(%rbp), %rdi # t39 # t39
    movq -296(%rbp), %rsi # t40 # t40
    movq -304(%rbp), %rdx # t41 # t41
    movq -312(%rbp), %rcx # t42 # t42
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -320(%rbp)
    movq $4, %rax
    movq %rax, -328(%rbp)
    leave
    ret
    jmp sublabel_end_38
sublabel_else_38: 
sublabel_end_38: 
    movq $5, %rax
    movq %rax, -336(%rbp)
    leave
    ret
.globl set_block_at
set_block_at:
    pushq %rbp
    movq %rsp, %rbp
    subq $368, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq -8(%rbp), %rax # block # block
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rax # index # index
    movq %rax, -32(%rbp)
    movq $1, %rax
    movq %rax, -40(%rbp)
    movq -32(%rbp), %rax # t47 # t47
    addq -40(%rbp), %rax
    movq %rax, -48(%rbp)
    movq -24(%rbp), %rdi # t46 # t46
    movq -48(%rbp), %rsi # t49 # t49
    movq $0, %rax
    call mematb
    movq %rax, -56(%rbp)
    movq %rax, -64(%rbp)
    movq -8(%rbp), %rax # block # block
    movq %rax, -72(%rbp)
    movq -16(%rbp), %rax # index # index
    movq %rax, -80(%rbp)
    movq $2, %rax
    movq %rax, -88(%rbp)
    movq -80(%rbp), %rax # t52 # t52
    addq -88(%rbp), %rax
    movq %rax, -96(%rbp)
    movq -72(%rbp), %rdi # t51 # t51
    movq -96(%rbp), %rsi # t54 # t54
    movq $0, %rax
    call mematb
    movq %rax, -104(%rbp)
    movq %rax, -112(%rbp)
    movq $32, %rax
    movq %rax, -120(%rbp)
    movq $4, %rax
    movq %rax, -128(%rbp)
    movq -120(%rbp), %rax # t56 # t56
    imulq -128(%rbp), %rax
    movq %rax, -136(%rbp)
    movq %rax, -144(%rbp)
    movq $16, %rax
    movq %rax, -152(%rbp)
    movq $4, %rax
    movq %rax, -160(%rbp)
    movq -152(%rbp), %rax # t59 # t59
    imulq -160(%rbp), %rax
    movq %rax, -168(%rbp)
    movq %rax, -176(%rbp)
    movq -8(%rbp), %rax # block # block
    movq %rax, -184(%rbp)
    movq -16(%rbp), %rax # index # index
    movq %rax, -192(%rbp)
    movq -184(%rbp), %rdi # t62 # t62
    movq -192(%rbp), %rsi # t63 # t63
    movq $0, %rax
    call mematb
    movq %rax, -200(%rbp)
    movq -200(%rbp), %rdi # t64 # t64
    movq $0, %rax
    call set_block_color
    movq %rax, -208(%rbp)
    movq -144(%rbp), %rax # x # x
    imulq -64(%rbp), %rax
    movq %rax, -216(%rbp)
    movq $20, %rax
    movq %rax, -224(%rbp)
    movq -216(%rbp), %rax # t66 # t66
    addq -224(%rbp), %rax
    movq %rax, -232(%rbp)
    movq -176(%rbp), %rax # y # y
    imulq -112(%rbp), %rax
    movq %rax, -240(%rbp)
    movq $30, %rax
    movq %rax, -248(%rbp)
    movq -240(%rbp), %rax # t69 # t69
    addq -248(%rbp), %rax
    movq %rax, -256(%rbp)
    movq $32, %rax
    movq %rax, -264(%rbp)
    movq $4, %rax
    movq %rax, -272(%rbp)
    movq -264(%rbp), %rax # t72 # t72
    imulq -272(%rbp), %rax
    movq %rax, -280(%rbp)
    movq $16, %rax
    movq %rax, -288(%rbp)
    movq $4, %rax
    movq %rax, -296(%rbp)
    movq -288(%rbp), %rax # t75 # t75
    imulq -296(%rbp), %rax
    movq %rax, -304(%rbp)
    movq -232(%rbp), %rdi # t68 # t68
    movq -256(%rbp), %rsi # t71 # t71
    movq -280(%rbp), %rdx # t74 # t74
    movq -304(%rbp), %rcx # t77 # t77
    movq $0, %rax
    call sdl_fillrect
    movq %rax, -312(%rbp)
    movq $0, %rax
    movq %rax, -320(%rbp)
    leave
    ret
.globl draw_grid
draw_grid:
    pushq %rbp
    movq %rsp, %rbp
    subq $448, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq $32, %rax
    movq %rax, -24(%rbp)
    movq $4, %rax
    movq %rax, -32(%rbp)
    movq -24(%rbp), %rax # t80 # t80
    imulq -32(%rbp), %rax
    movq %rax, -40(%rbp)
    movq %rax, -48(%rbp)
    movq $16, %rax
    movq %rax, -56(%rbp)
    movq $4, %rax
    movq %rax, -64(%rbp)
    movq -56(%rbp), %rax # t83 # t83
    imulq -64(%rbp), %rax
    movq %rax, -72(%rbp)
    movq %rax, -80(%rbp)
    movq $20, %rax
    movq %rax, -88(%rbp)
    movq $30, %rax
    movq %rax, -96(%rbp)
    movq $0, %rax
    movq %rax, -104(%rbp)
for_start_86: 
    movq $11, %rax
    movq %rax, -112(%rbp)
    movq -104(%rbp), %rax # x # x
    movq -112(%rbp), %rdi # t88 # t88
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -120(%rbp)
    movq -120(%rbp), %rax # t89 # t89
    cmpq $0, %rax
    je for_end_86
    movq $0, %rax
    movq %rax, -128(%rbp)
for_start_90: 
    movq $16, %rax
    movq %rax, -136(%rbp)
    movq -128(%rbp), %rax # y # y
    movq -136(%rbp), %rdi # t92 # t92
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -144(%rbp)
    movq -144(%rbp), %rax # t93 # t93
    cmpq $0, %rax
    je for_end_90
    movq -104(%rbp), %rax # x # x
    imulq -48(%rbp), %rax
    movq %rax, -152(%rbp)
    movq -88(%rbp), %rax # offset_left # offset_left
    movq %rax, -160(%rbp)
    movq -152(%rbp), %rax # t94 # t94
    addq -160(%rbp), %rax
    movq %rax, -168(%rbp)
    movq %rax, -176(%rbp)
    movq -128(%rbp), %rax # y # y
    imulq -80(%rbp), %rax
    movq %rax, -184(%rbp)
    movq -96(%rbp), %rax # offset_top # offset_top
    movq %rax, -192(%rbp)
    movq -184(%rbp), %rax # t97 # t97
    addq -192(%rbp), %rax
    movq %rax, -200(%rbp)
    movq %rax, -208(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -216(%rbp)
    movq $16, %rax
    movq %rax, -224(%rbp)
    movq -104(%rbp), %rax # x # x
    imulq -224(%rbp), %rax
    movq %rax, -232(%rbp)
    addq -128(%rbp), %rax
    movq %rax, -240(%rbp)
    movq -216(%rbp), %rdi # t100 # t100
    movq -240(%rbp), %rsi # t103 # t103
    movq $0, %rax
    call mematb
    movq %rax, -248(%rbp)
    movq %rax, -256(%rbp)
    movq -256(%rbp), %rdi # color # color
    movq $0, %rax
    call set_block_color
    movq %rax, -264(%rbp)
    movq $0, %rax
    movq %rax, -272(%rbp)
    movq -264(%rbp), %rax # t105 # t105
    movq -272(%rbp), %rdi # t106 # t106
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -280(%rbp)
    movq -280(%rbp), %rax # t107 # t107
    cmpq $0, %rax
    je sublabel_else_108
sublabel_if_108: 
    movq -176(%rbp), %rdi # grid_x # grid_x
    movq -208(%rbp), %rsi # grid_y # grid_y
    movq -48(%rbp), %rdx # block_size_w # block_size_w
    movq -80(%rbp), %rcx # block_size_h # block_size_h
    movq $0, %rax
    call sdl_fillrect
    movq %rax, -288(%rbp)
    jmp sublabel_end_108
sublabel_else_108: 
sublabel_end_108: 
for_post_91: 
    movq $1, %rax
    movq %rax, -296(%rbp)
    movq -128(%rbp), %rax # y # y
    addq -296(%rbp), %rax
    movq %rax, -304(%rbp)
# load t111 to y
    movq -304(%rbp), %rcx
    movq %rcx, -128(%rbp)
    jmp for_start_90
for_end_90: 
for_post_87: 
    movq $1, %rax
    movq %rax, -312(%rbp)
    movq -104(%rbp), %rax # x # x
    addq -312(%rbp), %rax
    movq %rax, -320(%rbp)
# load t113 to x
    movq -320(%rbp), %rcx
    movq %rcx, -104(%rbp)
    jmp for_start_86
for_end_86: 
    movq -16(%rbp), %rax # block # block
    movq %rax, -328(%rbp)
    movq $0, %rax
    movq %rax, -336(%rbp)
    movq -328(%rbp), %rdi # t114 # t114
    movq -336(%rbp), %rsi # t115 # t115
    movq $0, %rax
    call set_block_at
    movq %rax, -344(%rbp)
    movq -16(%rbp), %rax # block # block
    movq %rax, -352(%rbp)
    movq $3, %rax
    movq %rax, -360(%rbp)
    movq -352(%rbp), %rdi # t117 # t117
    movq -360(%rbp), %rsi # t118 # t118
    movq $0, %rax
    call set_block_at
    movq %rax, -368(%rbp)
    movq -16(%rbp), %rax # block # block
    movq %rax, -376(%rbp)
    movq $6, %rax
    movq %rax, -384(%rbp)
    movq -376(%rbp), %rdi # t120 # t120
    movq -384(%rbp), %rsi # t121 # t121
    movq $0, %rax
    call set_block_at
    movq %rax, -392(%rbp)
    movq $0, %rax
    movq %rax, -400(%rbp)
    leave
    ret
.globl check_blocks
check_blocks:
    pushq %rbp
    movq %rsp, %rbp
    subq $2592, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq $0, %rax
    movq %rax, -24(%rbp)
    movq $0, %rax
    movq %rax, -32(%rbp)
    movq $0, %rax
    movq %rax, -40(%rbp)
for_start_124: 
    movq $11, %rax
    movq %rax, -48(%rbp)
    movq -40(%rbp), %rax # x # x
    movq -48(%rbp), %rdi # t126 # t126
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -56(%rbp)
    movq -56(%rbp), %rax # t127 # t127
    cmpq $0, %rax
    je for_end_124
    movq $0, %rax
    movq %rax, -64(%rbp)
for_start_128: 
    movq $14, %rax
    movq %rax, -72(%rbp)
    movq -64(%rbp), %rax # y # y
    movq -72(%rbp), %rdi # t130 # t130
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -80(%rbp)
    movq -80(%rbp), %rax # t131 # t131
    cmpq $0, %rax
    je for_end_128
# load x to bx
    movq -40(%rbp), %rcx
    movq %rcx, -24(%rbp)
# load y to by
    movq -64(%rbp), %rcx
    movq %rcx, -32(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -88(%rbp)
    movq $16, %rax
    movq %rax, -96(%rbp)
    movq -24(%rbp), %rax # bx # bx
    imulq -96(%rbp), %rax
    movq %rax, -104(%rbp)
    addq -32(%rbp), %rax
    movq %rax, -112(%rbp)
    movq -88(%rbp), %rdi # t132 # t132
    movq -112(%rbp), %rsi # t135 # t135
    movq $0, %rax
    call mematb
    movq %rax, -120(%rbp)
    movq %rax, -128(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -136(%rbp)
    movq $16, %rax
    movq %rax, -144(%rbp)
    movq -24(%rbp), %rax # bx # bx
    imulq -144(%rbp), %rax
    movq %rax, -152(%rbp)
    movq $1, %rax
    movq %rax, -160(%rbp)
    movq -32(%rbp), %rax # by # by
    addq -160(%rbp), %rax
    movq %rax, -168(%rbp)
    movq -152(%rbp), %rax # t139 # t139
    addq -168(%rbp), %rax
    movq %rax, -176(%rbp)
    movq -136(%rbp), %rdi # t137 # t137
    movq -176(%rbp), %rsi # t142 # t142
    movq $0, %rax
    call mematb
    movq %rax, -184(%rbp)
    movq %rax, -192(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -200(%rbp)
    movq $16, %rax
    movq %rax, -208(%rbp)
    movq -24(%rbp), %rax # bx # bx
    imulq -208(%rbp), %rax
    movq %rax, -216(%rbp)
    movq $2, %rax
    movq %rax, -224(%rbp)
    movq -32(%rbp), %rax # by # by
    addq -224(%rbp), %rax
    movq %rax, -232(%rbp)
    movq -216(%rbp), %rax # t146 # t146
    addq -232(%rbp), %rax
    movq %rax, -240(%rbp)
    movq -200(%rbp), %rdi # t144 # t144
    movq -240(%rbp), %rsi # t149 # t149
    movq $0, %rax
    call mematb
    movq %rax, -248(%rbp)
    movq %rax, -256(%rbp)
    movq $0, %rax
    movq %rax, -264(%rbp)
    movq -128(%rbp), %rax # color1 # color1
    movq -264(%rbp), %rdi # t151 # t151
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -272(%rbp)
    movq $0, %rax
    movq %rax, -280(%rbp)
    movq -192(%rbp), %rax # color2 # color2
    movq -280(%rbp), %rdi # t153 # t153
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -288(%rbp)
    movq -272(%rbp), %rsi # t152 # t152
    cmpq $0, %rsi
    setne %al
    movq -288(%rbp), %rdi # t154 # t154
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -296(%rbp)
    movq $0, %rax
    movq %rax, -304(%rbp)
    movq -256(%rbp), %rax # color3 # color3
    movq -304(%rbp), %rdi # t156 # t156
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -312(%rbp)
    movq -296(%rbp), %rsi # t155 # t155
    cmpq $0, %rsi
    setne %al
    movq -312(%rbp), %rdi # t157 # t157
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -320(%rbp)
    movq -128(%rbp), %rsi # color1 # color1
    movq -192(%rbp), %rdi # color2 # color2
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -328(%rbp)
    movq -320(%rbp), %rsi # t158 # t158
    cmpq $0, %rsi
    setne %al
    movq -328(%rbp), %rdi # t159 # t159
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -336(%rbp)
    movq -128(%rbp), %rsi # color1 # color1
    movq -256(%rbp), %rdi # color3 # color3
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -344(%rbp)
    movq -336(%rbp), %rsi # t160 # t160
    cmpq $0, %rsi
    setne %al
    movq -344(%rbp), %rdi # t161 # t161
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -352(%rbp)
    cmpq $0, %rax
    je sublabel_else_163
sublabel_if_163: 
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -360(%rbp)
    movq $16, %rax
    movq %rax, -368(%rbp)
    movq -24(%rbp), %rax # bx # bx
    imulq -368(%rbp), %rax
    movq %rax, -376(%rbp)
    addq -32(%rbp), %rax
    movq %rax, -384(%rbp)
    movq $0, %rax
    movq %rax, -392(%rbp)
    movq -360(%rbp), %rdi # t164 # t164
    movq -384(%rbp), %rsi # t167 # t167
    movq -392(%rbp), %rdx # t168 # t168
    movq $0, %rax
    call memstoreb
    movq %rax, -400(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -408(%rbp)
    movq $16, %rax
    movq %rax, -416(%rbp)
    movq -24(%rbp), %rax # bx # bx
    imulq -416(%rbp), %rax
    movq %rax, -424(%rbp)
    movq $1, %rax
    movq %rax, -432(%rbp)
    movq -32(%rbp), %rax # by # by
    addq -432(%rbp), %rax
    movq %rax, -440(%rbp)
    movq -424(%rbp), %rax # t172 # t172
    addq -440(%rbp), %rax
    movq %rax, -448(%rbp)
    movq $0, %rax
    movq %rax, -456(%rbp)
    movq -408(%rbp), %rdi # t170 # t170
    movq -448(%rbp), %rsi # t175 # t175
    movq -456(%rbp), %rdx # t176 # t176
    movq $0, %rax
    call memstoreb
    movq %rax, -464(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -472(%rbp)
    movq $16, %rax
    movq %rax, -480(%rbp)
    movq -24(%rbp), %rax # bx # bx
    imulq -480(%rbp), %rax
    movq %rax, -488(%rbp)
    movq $2, %rax
    movq %rax, -496(%rbp)
    movq -32(%rbp), %rax # by # by
    addq -496(%rbp), %rax
    movq %rax, -504(%rbp)
    movq -488(%rbp), %rax # t180 # t180
    addq -504(%rbp), %rax
    movq %rax, -512(%rbp)
    movq $0, %rax
    movq %rax, -520(%rbp)
    movq -472(%rbp), %rdi # t178 # t178
    movq -512(%rbp), %rsi # t183 # t183
    movq -520(%rbp), %rdx # t184 # t184
    movq $0, %rax
    call memstoreb
    movq %rax, -528(%rbp)
    movq -16(%rbp), %rax # score # score
    movq %rax, -536(%rbp)
    movq $0, %rax
    movq %rax, -544(%rbp)
    movq -16(%rbp), %rax # score # score
    movq %rax, -552(%rbp)
    movq $0, %rax
    movq %rax, -560(%rbp)
    movq -552(%rbp), %rdi # t188 # t188
    movq -560(%rbp), %rsi # t189 # t189
    movq $0, %rax
    call mematl
    movq %rax, -568(%rbp)
    movq $100, %rax
    movq %rax, -576(%rbp)
    movq -568(%rbp), %rax # t190 # t190
    addq -576(%rbp), %rax
    movq %rax, -584(%rbp)
    movq -536(%rbp), %rdi # t186 # t186
    movq -544(%rbp), %rsi # t187 # t187
    movq -584(%rbp), %rdx # t192 # t192
    movq $0, %rax
    call memstorel
    movq %rax, -592(%rbp)
    movq $0, %rax
    movq %rax, -600(%rbp)
    leave
    ret
    jmp sublabel_end_163
sublabel_else_163: 
sublabel_end_163: 
for_post_129: 
    movq $1, %rax
    movq %rax, -608(%rbp)
    movq -64(%rbp), %rax # y # y
    addq -608(%rbp), %rax
    movq %rax, -616(%rbp)
# load t196 to y
    movq -616(%rbp), %rcx
    movq %rcx, -64(%rbp)
    jmp for_start_128
for_end_128: 
for_post_125: 
    movq $1, %rax
    movq %rax, -624(%rbp)
    movq -40(%rbp), %rax # x # x
    addq -624(%rbp), %rax
    movq %rax, -632(%rbp)
# load t198 to x
    movq -632(%rbp), %rcx
    movq %rcx, -40(%rbp)
    jmp for_start_124
for_end_124: 
    movq $0, %rax
    movq %rax, -640(%rbp)
    movq $0, %rax
    movq %rax, -648(%rbp)
    movq $0, %rax
    movq %rax, -656(%rbp)
    movq $0, %rax
    movq %rax, -664(%rbp)
    movq $0, %rax
    movq %rax, -672(%rbp)
    movq $0, %rcx # here
    movq %rcx, -648(%rbp)
for_start_199: 
    movq -648(%rbp), %rax # y1 # y1
    movq %rax, -680(%rbp)
    movq $16, %rax
    movq %rax, -688(%rbp)
    movq -680(%rbp), %rax # t201 # t201
    movq -688(%rbp), %rdi # t202 # t202
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -696(%rbp)
    movq -696(%rbp), %rax # t203 # t203
    cmpq $0, %rax
    je for_end_199
    movq $0, %rcx # here
    movq %rcx, -640(%rbp)
for_start_204: 
    movq -640(%rbp), %rax # x1 # x1
    movq %rax, -704(%rbp)
    movq $9, %rax
    movq %rax, -712(%rbp)
    movq -704(%rbp), %rax # t206 # t206
    movq -712(%rbp), %rdi # t207 # t207
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -720(%rbp)
    movq -720(%rbp), %rax # t208 # t208
    cmpq $0, %rax
    je for_end_204
    movq -640(%rbp), %rax # x1 # x1
    movq %rax, -728(%rbp)
# load t209 to bx
    movq -728(%rbp), %rcx
    movq %rcx, -24(%rbp)
    movq -648(%rbp), %rax # y1 # y1
    movq %rax, -736(%rbp)
# load t210 to by
    movq -736(%rbp), %rcx
    movq %rcx, -32(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -744(%rbp)
    movq $16, %rax
    movq %rax, -752(%rbp)
    movq -24(%rbp), %rax # bx # bx
    imulq -752(%rbp), %rax
    movq %rax, -760(%rbp)
    addq -32(%rbp), %rax
    movq %rax, -768(%rbp)
    movq -744(%rbp), %rdi # t211 # t211
    movq -768(%rbp), %rsi # t214 # t214
    movq $0, %rax
    call mematb
    movq %rax, -776(%rbp)
# load t215 to color1x
    movq -776(%rbp), %rcx
    movq %rcx, -656(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -784(%rbp)
    movq $1, %rax
    movq %rax, -792(%rbp)
    movq -24(%rbp), %rax # bx # bx
    addq -792(%rbp), %rax
    movq %rax, -800(%rbp)
    movq $16, %rax
    movq %rax, -808(%rbp)
    movq -800(%rbp), %rax # t218 # t218
    imulq -808(%rbp), %rax
    movq %rax, -816(%rbp)
    addq -32(%rbp), %rax
    movq %rax, -824(%rbp)
    movq -784(%rbp), %rdi # t216 # t216
    movq -824(%rbp), %rsi # t221 # t221
    movq $0, %rax
    call mematb
    movq %rax, -832(%rbp)
# load t222 to color2x
    movq -832(%rbp), %rcx
    movq %rcx, -664(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -840(%rbp)
    movq $2, %rax
    movq %rax, -848(%rbp)
    movq -24(%rbp), %rax # bx # bx
    addq -848(%rbp), %rax
    movq %rax, -856(%rbp)
    movq $16, %rax
    movq %rax, -864(%rbp)
    movq -856(%rbp), %rax # t225 # t225
    imulq -864(%rbp), %rax
    movq %rax, -872(%rbp)
    addq -32(%rbp), %rax
    movq %rax, -880(%rbp)
    movq -840(%rbp), %rdi # t223 # t223
    movq -880(%rbp), %rsi # t228 # t228
    movq $0, %rax
    call mematb
    movq %rax, -888(%rbp)
# load t229 to color3x
    movq -888(%rbp), %rcx
    movq %rcx, -672(%rbp)
    movq $0, %rax
    movq %rax, -896(%rbp)
    movq -656(%rbp), %rax # color1x # color1x
    movq -896(%rbp), %rdi # t230 # t230
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -904(%rbp)
    movq $0, %rax
    movq %rax, -912(%rbp)
    movq -664(%rbp), %rax # color2x # color2x
    movq -912(%rbp), %rdi # t232 # t232
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -920(%rbp)
    movq -904(%rbp), %rsi # t231 # t231
    cmpq $0, %rsi
    setne %al
    movq -920(%rbp), %rdi # t233 # t233
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -928(%rbp)
    movq $0, %rax
    movq %rax, -936(%rbp)
    movq -672(%rbp), %rax # color3x # color3x
    movq -936(%rbp), %rdi # t235 # t235
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -944(%rbp)
    movq -928(%rbp), %rsi # t234 # t234
    cmpq $0, %rsi
    setne %al
    movq -944(%rbp), %rdi # t236 # t236
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -952(%rbp)
    movq -656(%rbp), %rsi # color1x # color1x
    movq -664(%rbp), %rdi # color2x # color2x
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -960(%rbp)
    movq -952(%rbp), %rsi # t237 # t237
    cmpq $0, %rsi
    setne %al
    movq -960(%rbp), %rdi # t238 # t238
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -968(%rbp)
    movq -656(%rbp), %rsi # color1x # color1x
    movq -672(%rbp), %rdi # color3x # color3x
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -976(%rbp)
    movq -968(%rbp), %rsi # t239 # t239
    cmpq $0, %rsi
    setne %al
    movq -976(%rbp), %rdi # t240 # t240
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -984(%rbp)
    cmpq $0, %rax
    je sublabel_else_242
sublabel_if_242: 
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -992(%rbp)
    movq $16, %rax
    movq %rax, -1000(%rbp)
    movq -24(%rbp), %rax # bx # bx
    imulq -1000(%rbp), %rax
    movq %rax, -1008(%rbp)
    addq -32(%rbp), %rax
    movq %rax, -1016(%rbp)
    movq $0, %rax
    movq %rax, -1024(%rbp)
    movq -992(%rbp), %rdi # t243 # t243
    movq -1016(%rbp), %rsi # t246 # t246
    movq -1024(%rbp), %rdx # t247 # t247
    movq $0, %rax
    call memstoreb
    movq %rax, -1032(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -1040(%rbp)
    movq $1, %rax
    movq %rax, -1048(%rbp)
    movq -24(%rbp), %rax # bx # bx
    addq -1048(%rbp), %rax
    movq %rax, -1056(%rbp)
    movq $16, %rax
    movq %rax, -1064(%rbp)
    movq -1056(%rbp), %rax # t251 # t251
    imulq -1064(%rbp), %rax
    movq %rax, -1072(%rbp)
    addq -32(%rbp), %rax
    movq %rax, -1080(%rbp)
    movq $0, %rax
    movq %rax, -1088(%rbp)
    movq -1040(%rbp), %rdi # t249 # t249
    movq -1080(%rbp), %rsi # t254 # t254
    movq -1088(%rbp), %rdx # t255 # t255
    movq $0, %rax
    call memstoreb
    movq %rax, -1096(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -1104(%rbp)
    movq $2, %rax
    movq %rax, -1112(%rbp)
    movq -24(%rbp), %rax # bx # bx
    addq -1112(%rbp), %rax
    movq %rax, -1120(%rbp)
    movq $16, %rax
    movq %rax, -1128(%rbp)
    movq -1120(%rbp), %rax # t259 # t259
    imulq -1128(%rbp), %rax
    movq %rax, -1136(%rbp)
    addq -32(%rbp), %rax
    movq %rax, -1144(%rbp)
    movq $0, %rax
    movq %rax, -1152(%rbp)
    movq -1104(%rbp), %rdi # t257 # t257
    movq -1144(%rbp), %rsi # t262 # t262
    movq -1152(%rbp), %rdx # t263 # t263
    movq $0, %rax
    call memstoreb
    movq %rax, -1160(%rbp)
    movq -16(%rbp), %rax # score # score
    movq %rax, -1168(%rbp)
    movq $0, %rax
    movq %rax, -1176(%rbp)
    movq -16(%rbp), %rax # score # score
    movq %rax, -1184(%rbp)
    movq $0, %rax
    movq %rax, -1192(%rbp)
    movq -1184(%rbp), %rdi # t267 # t267
    movq -1192(%rbp), %rsi # t268 # t268
    movq $0, %rax
    call mematl
    movq %rax, -1200(%rbp)
    movq $100, %rax
    movq %rax, -1208(%rbp)
    movq -1200(%rbp), %rax # t269 # t269
    addq -1208(%rbp), %rax
    movq %rax, -1216(%rbp)
    movq -1168(%rbp), %rdi # t265 # t265
    movq -1176(%rbp), %rsi # t266 # t266
    movq -1216(%rbp), %rdx # t271 # t271
    movq $0, %rax
    call memstorel
    movq %rax, -1224(%rbp)
    movq $0, %rax
    movq %rax, -1232(%rbp)
    leave
    ret
    jmp sublabel_end_242
sublabel_else_242: 
sublabel_end_242: 
for_post_205: 
    movq -640(%rbp), %rax # x1 # x1
    movq %rax, -1240(%rbp)
    movq $1, %rax
    movq %rax, -1248(%rbp)
    movq -1240(%rbp), %rax # t274 # t274
    addq -1248(%rbp), %rax
    movq %rax, -1256(%rbp)
# load t276 to x1
    movq -1256(%rbp), %rcx
    movq %rcx, -640(%rbp)
    jmp for_start_204
for_end_204: 
for_post_200: 
    movq -648(%rbp), %rax # y1 # y1
    movq %rax, -1264(%rbp)
    movq $1, %rax
    movq %rax, -1272(%rbp)
    movq -1264(%rbp), %rax # t277 # t277
    addq -1272(%rbp), %rax
    movq %rax, -1280(%rbp)
# load t279 to y1
    movq -1280(%rbp), %rcx
    movq %rcx, -648(%rbp)
    jmp for_start_199
for_end_199: 
    movq $0, %rcx # here
    movq %rcx, -648(%rbp)
for_start_280: 
    movq $14, %rax
    movq %rax, -1288(%rbp)
    movq -648(%rbp), %rax # y1 # y1
    movq -1288(%rbp), %rdi # t282 # t282
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -1296(%rbp)
    movq -1296(%rbp), %rax # t283 # t283
    cmpq $0, %rax
    je for_end_280
    movq $0, %rcx # here
    movq %rcx, -640(%rbp)
for_start_284: 
    movq $7, %rax
    movq %rax, -1304(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    movq -1304(%rbp), %rdi # t286 # t286
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -1312(%rbp)
    movq -1312(%rbp), %rax # t287 # t287
    cmpq $0, %rax
    je for_end_284
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -1320(%rbp)
    movq $16, %rax
    movq %rax, -1328(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    imulq -1328(%rbp), %rax
    movq %rax, -1336(%rbp)
    addq -648(%rbp), %rax
    movq %rax, -1344(%rbp)
    movq -1320(%rbp), %rdi # t288 # t288
    movq -1344(%rbp), %rsi # t291 # t291
    movq $0, %rax
    call mematb
    movq %rax, -1352(%rbp)
# load t292 to color1x
    movq -1352(%rbp), %rcx
    movq %rcx, -656(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -1360(%rbp)
    movq $1, %rax
    movq %rax, -1368(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    addq -1368(%rbp), %rax
    movq %rax, -1376(%rbp)
    movq $16, %rax
    movq %rax, -1384(%rbp)
    movq -1376(%rbp), %rax # t295 # t295
    imulq -1384(%rbp), %rax
    movq %rax, -1392(%rbp)
    movq $1, %rax
    movq %rax, -1400(%rbp)
    movq -648(%rbp), %rax # y1 # y1
    addq -1400(%rbp), %rax
    movq %rax, -1408(%rbp)
    movq -1392(%rbp), %rax # t297 # t297
    addq -1408(%rbp), %rax
    movq %rax, -1416(%rbp)
    movq -1360(%rbp), %rdi # t293 # t293
    movq -1416(%rbp), %rsi # t300 # t300
    movq $0, %rax
    call mematb
    movq %rax, -1424(%rbp)
# load t301 to color2x
    movq -1424(%rbp), %rcx
    movq %rcx, -664(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -1432(%rbp)
    movq $2, %rax
    movq %rax, -1440(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    addq -1440(%rbp), %rax
    movq %rax, -1448(%rbp)
    movq $16, %rax
    movq %rax, -1456(%rbp)
    movq -1448(%rbp), %rax # t304 # t304
    imulq -1456(%rbp), %rax
    movq %rax, -1464(%rbp)
    movq $2, %rax
    movq %rax, -1472(%rbp)
    movq -648(%rbp), %rax # y1 # y1
    addq -1472(%rbp), %rax
    movq %rax, -1480(%rbp)
    movq -1464(%rbp), %rax # t306 # t306
    addq -1480(%rbp), %rax
    movq %rax, -1488(%rbp)
    movq -1432(%rbp), %rdi # t302 # t302
    movq -1488(%rbp), %rsi # t309 # t309
    movq $0, %rax
    call mematb
    movq %rax, -1496(%rbp)
# load t310 to color3x
    movq -1496(%rbp), %rcx
    movq %rcx, -672(%rbp)
    movq $0, %rax
    movq %rax, -1504(%rbp)
    movq -656(%rbp), %rax # color1x # color1x
    movq -1504(%rbp), %rdi # t311 # t311
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -1512(%rbp)
    movq $0, %rax
    movq %rax, -1520(%rbp)
    movq -664(%rbp), %rax # color2x # color2x
    movq -1520(%rbp), %rdi # t313 # t313
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -1528(%rbp)
    movq -1512(%rbp), %rsi # t312 # t312
    cmpq $0, %rsi
    setne %al
    movq -1528(%rbp), %rdi # t314 # t314
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -1536(%rbp)
    movq $0, %rax
    movq %rax, -1544(%rbp)
    movq -672(%rbp), %rax # color3x # color3x
    movq -1544(%rbp), %rdi # t316 # t316
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -1552(%rbp)
    movq -1536(%rbp), %rsi # t315 # t315
    cmpq $0, %rsi
    setne %al
    movq -1552(%rbp), %rdi # t317 # t317
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -1560(%rbp)
    movq -656(%rbp), %rsi # color1x # color1x
    movq -664(%rbp), %rdi # color2x # color2x
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -1568(%rbp)
    movq -1560(%rbp), %rsi # t318 # t318
    cmpq $0, %rsi
    setne %al
    movq -1568(%rbp), %rdi # t319 # t319
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -1576(%rbp)
    movq -656(%rbp), %rsi # color1x # color1x
    movq -672(%rbp), %rdi # color3x # color3x
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -1584(%rbp)
    movq -1576(%rbp), %rsi # t320 # t320
    cmpq $0, %rsi
    setne %al
    movq -1584(%rbp), %rdi # t321 # t321
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -1592(%rbp)
    cmpq $0, %rax
    je sublabel_else_323
sublabel_if_323: 
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -1600(%rbp)
    movq $16, %rax
    movq %rax, -1608(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    imulq -1608(%rbp), %rax
    movq %rax, -1616(%rbp)
    addq -648(%rbp), %rax
    movq %rax, -1624(%rbp)
    movq $0, %rax
    movq %rax, -1632(%rbp)
    movq -1600(%rbp), %rdi # t324 # t324
    movq -1624(%rbp), %rsi # t327 # t327
    movq -1632(%rbp), %rdx # t328 # t328
    movq $0, %rax
    call memstoreb
    movq %rax, -1640(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -1648(%rbp)
    movq $1, %rax
    movq %rax, -1656(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    addq -1656(%rbp), %rax
    movq %rax, -1664(%rbp)
    movq $16, %rax
    movq %rax, -1672(%rbp)
    movq -1664(%rbp), %rax # t332 # t332
    imulq -1672(%rbp), %rax
    movq %rax, -1680(%rbp)
    movq $1, %rax
    movq %rax, -1688(%rbp)
    movq -648(%rbp), %rax # y1 # y1
    addq -1688(%rbp), %rax
    movq %rax, -1696(%rbp)
    movq -1680(%rbp), %rax # t334 # t334
    addq -1696(%rbp), %rax
    movq %rax, -1704(%rbp)
    movq $0, %rax
    movq %rax, -1712(%rbp)
    movq -1648(%rbp), %rdi # t330 # t330
    movq -1704(%rbp), %rsi # t337 # t337
    movq -1712(%rbp), %rdx # t338 # t338
    movq $0, %rax
    call memstoreb
    movq %rax, -1720(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -1728(%rbp)
    movq $2, %rax
    movq %rax, -1736(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    addq -1736(%rbp), %rax
    movq %rax, -1744(%rbp)
    movq $16, %rax
    movq %rax, -1752(%rbp)
    movq -1744(%rbp), %rax # t342 # t342
    imulq -1752(%rbp), %rax
    movq %rax, -1760(%rbp)
    movq $2, %rax
    movq %rax, -1768(%rbp)
    movq -648(%rbp), %rax # y1 # y1
    addq -1768(%rbp), %rax
    movq %rax, -1776(%rbp)
    movq -1760(%rbp), %rax # t344 # t344
    addq -1776(%rbp), %rax
    movq %rax, -1784(%rbp)
    movq $0, %rax
    movq %rax, -1792(%rbp)
    movq -1728(%rbp), %rdi # t340 # t340
    movq -1784(%rbp), %rsi # t347 # t347
    movq -1792(%rbp), %rdx # t348 # t348
    movq $0, %rax
    call memstoreb
    movq %rax, -1800(%rbp)
    movq -16(%rbp), %rax # score # score
    movq %rax, -1808(%rbp)
    movq $0, %rax
    movq %rax, -1816(%rbp)
    movq -16(%rbp), %rax # score # score
    movq %rax, -1824(%rbp)
    movq $0, %rax
    movq %rax, -1832(%rbp)
    movq -1824(%rbp), %rdi # t352 # t352
    movq -1832(%rbp), %rsi # t353 # t353
    movq $0, %rax
    call mematl
    movq %rax, -1840(%rbp)
    movq $100, %rax
    movq %rax, -1848(%rbp)
    movq -1840(%rbp), %rax # t354 # t354
    addq -1848(%rbp), %rax
    movq %rax, -1856(%rbp)
    movq -1808(%rbp), %rdi # t350 # t350
    movq -1816(%rbp), %rsi # t351 # t351
    movq -1856(%rbp), %rdx # t356 # t356
    movq $0, %rax
    call memstorel
    movq %rax, -1864(%rbp)
    movq $0, %rax
    movq %rax, -1872(%rbp)
    leave
    ret
    jmp sublabel_end_323
sublabel_else_323: 
sublabel_end_323: 
for_post_285: 
    movq $1, %rax
    movq %rax, -1880(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    addq -1880(%rbp), %rax
    movq %rax, -1888(%rbp)
# load t360 to x1
    movq -1888(%rbp), %rcx
    movq %rcx, -640(%rbp)
    jmp for_start_284
for_end_284: 
for_post_281: 
    movq $1, %rax
    movq %rax, -1896(%rbp)
    movq -648(%rbp), %rax # y1 # y1
    addq -1896(%rbp), %rax
    movq %rax, -1904(%rbp)
# load t362 to y1
    movq -1904(%rbp), %rcx
    movq %rcx, -648(%rbp)
    jmp for_start_280
for_end_280: 
    movq $0, %rcx # here
    movq %rcx, -648(%rbp)
for_start_363: 
    movq $14, %rax
    movq %rax, -1912(%rbp)
    movq -648(%rbp), %rax # y1 # y1
    movq -1912(%rbp), %rdi # t365 # t365
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -1920(%rbp)
    movq -1920(%rbp), %rax # t366 # t366
    cmpq $0, %rax
    je for_end_363
    movq $2, %rcx # here
    movq %rcx, -640(%rbp)
for_start_367: 
    movq $9, %rax
    movq %rax, -1928(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    movq -1928(%rbp), %rdi # t369 # t369
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -1936(%rbp)
    movq -1936(%rbp), %rax # t370 # t370
    cmpq $0, %rax
    je for_end_367
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -1944(%rbp)
    movq $16, %rax
    movq %rax, -1952(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    imulq -1952(%rbp), %rax
    movq %rax, -1960(%rbp)
    addq -648(%rbp), %rax
    movq %rax, -1968(%rbp)
    movq -1944(%rbp), %rdi # t371 # t371
    movq -1968(%rbp), %rsi # t374 # t374
    movq $0, %rax
    call mematb
    movq %rax, -1976(%rbp)
# load t375 to color1x
    movq -1976(%rbp), %rcx
    movq %rcx, -656(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -1984(%rbp)
    movq $1, %rax
    movq %rax, -1992(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    subq -1992(%rbp), %rax
    movq %rax, -2000(%rbp)
    movq $16, %rax
    movq %rax, -2008(%rbp)
    movq -2000(%rbp), %rax # t378 # t378
    imulq -2008(%rbp), %rax
    movq %rax, -2016(%rbp)
    movq $1, %rax
    movq %rax, -2024(%rbp)
    movq -648(%rbp), %rax # y1 # y1
    addq -2024(%rbp), %rax
    movq %rax, -2032(%rbp)
    movq -2016(%rbp), %rax # t380 # t380
    addq -2032(%rbp), %rax
    movq %rax, -2040(%rbp)
    movq -1984(%rbp), %rdi # t376 # t376
    movq -2040(%rbp), %rsi # t383 # t383
    movq $0, %rax
    call mematb
    movq %rax, -2048(%rbp)
# load t384 to color2x
    movq -2048(%rbp), %rcx
    movq %rcx, -664(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -2056(%rbp)
    movq $2, %rax
    movq %rax, -2064(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    subq -2064(%rbp), %rax
    movq %rax, -2072(%rbp)
    movq $16, %rax
    movq %rax, -2080(%rbp)
    movq -2072(%rbp), %rax # t387 # t387
    imulq -2080(%rbp), %rax
    movq %rax, -2088(%rbp)
    movq $2, %rax
    movq %rax, -2096(%rbp)
    movq -648(%rbp), %rax # y1 # y1
    addq -2096(%rbp), %rax
    movq %rax, -2104(%rbp)
    movq -2088(%rbp), %rax # t389 # t389
    addq -2104(%rbp), %rax
    movq %rax, -2112(%rbp)
    movq -2056(%rbp), %rdi # t385 # t385
    movq -2112(%rbp), %rsi # t392 # t392
    movq $0, %rax
    call mematb
    movq %rax, -2120(%rbp)
# load t393 to color3x
    movq -2120(%rbp), %rcx
    movq %rcx, -672(%rbp)
    movq $0, %rax
    movq %rax, -2128(%rbp)
    movq -656(%rbp), %rax # color1x # color1x
    movq -2128(%rbp), %rdi # t394 # t394
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -2136(%rbp)
    movq $0, %rax
    movq %rax, -2144(%rbp)
    movq -664(%rbp), %rax # color2x # color2x
    movq -2144(%rbp), %rdi # t396 # t396
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -2152(%rbp)
    movq -2136(%rbp), %rsi # t395 # t395
    cmpq $0, %rsi
    setne %al
    movq -2152(%rbp), %rdi # t397 # t397
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -2160(%rbp)
    movq $0, %rax
    movq %rax, -2168(%rbp)
    movq -672(%rbp), %rax # color3x # color3x
    movq -2168(%rbp), %rdi # t399 # t399
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -2176(%rbp)
    movq -2160(%rbp), %rsi # t398 # t398
    cmpq $0, %rsi
    setne %al
    movq -2176(%rbp), %rdi # t400 # t400
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -2184(%rbp)
    movq -656(%rbp), %rsi # color1x # color1x
    movq -664(%rbp), %rdi # color2x # color2x
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -2192(%rbp)
    movq -2184(%rbp), %rsi # t401 # t401
    cmpq $0, %rsi
    setne %al
    movq -2192(%rbp), %rdi # t402 # t402
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -2200(%rbp)
    movq -656(%rbp), %rsi # color1x # color1x
    movq -672(%rbp), %rdi # color3x # color3x
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -2208(%rbp)
    movq -2200(%rbp), %rsi # t403 # t403
    cmpq $0, %rsi
    setne %al
    movq -2208(%rbp), %rdi # t404 # t404
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -2216(%rbp)
    cmpq $0, %rax
    je sublabel_else_406
sublabel_if_406: 
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -2224(%rbp)
    movq $16, %rax
    movq %rax, -2232(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    imulq -2232(%rbp), %rax
    movq %rax, -2240(%rbp)
    addq -648(%rbp), %rax
    movq %rax, -2248(%rbp)
    movq $0, %rax
    movq %rax, -2256(%rbp)
    movq -2224(%rbp), %rdi # t407 # t407
    movq -2248(%rbp), %rsi # t410 # t410
    movq -2256(%rbp), %rdx # t411 # t411
    movq $0, %rax
    call memstoreb
    movq %rax, -2264(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -2272(%rbp)
    movq $1, %rax
    movq %rax, -2280(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    subq -2280(%rbp), %rax
    movq %rax, -2288(%rbp)
    movq $16, %rax
    movq %rax, -2296(%rbp)
    movq -2288(%rbp), %rax # t415 # t415
    imulq -2296(%rbp), %rax
    movq %rax, -2304(%rbp)
    movq $1, %rax
    movq %rax, -2312(%rbp)
    movq -648(%rbp), %rax # y1 # y1
    addq -2312(%rbp), %rax
    movq %rax, -2320(%rbp)
    movq -2304(%rbp), %rax # t417 # t417
    addq -2320(%rbp), %rax
    movq %rax, -2328(%rbp)
    movq $0, %rax
    movq %rax, -2336(%rbp)
    movq -2272(%rbp), %rdi # t413 # t413
    movq -2328(%rbp), %rsi # t420 # t420
    movq -2336(%rbp), %rdx # t421 # t421
    movq $0, %rax
    call memstoreb
    movq %rax, -2344(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -2352(%rbp)
    movq $2, %rax
    movq %rax, -2360(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    subq -2360(%rbp), %rax
    movq %rax, -2368(%rbp)
    movq $16, %rax
    movq %rax, -2376(%rbp)
    movq -2368(%rbp), %rax # t425 # t425
    imulq -2376(%rbp), %rax
    movq %rax, -2384(%rbp)
    movq $2, %rax
    movq %rax, -2392(%rbp)
    movq -648(%rbp), %rax # y1 # y1
    addq -2392(%rbp), %rax
    movq %rax, -2400(%rbp)
    movq -2384(%rbp), %rax # t427 # t427
    addq -2400(%rbp), %rax
    movq %rax, -2408(%rbp)
    movq $0, %rax
    movq %rax, -2416(%rbp)
    movq -2352(%rbp), %rdi # t423 # t423
    movq -2408(%rbp), %rsi # t430 # t430
    movq -2416(%rbp), %rdx # t431 # t431
    movq $0, %rax
    call memstoreb
    movq %rax, -2424(%rbp)
    movq -16(%rbp), %rax # score # score
    movq %rax, -2432(%rbp)
    movq $0, %rax
    movq %rax, -2440(%rbp)
    movq -16(%rbp), %rax # score # score
    movq %rax, -2448(%rbp)
    movq $0, %rax
    movq %rax, -2456(%rbp)
    movq -2448(%rbp), %rdi # t435 # t435
    movq -2456(%rbp), %rsi # t436 # t436
    movq $0, %rax
    call mematl
    movq %rax, -2464(%rbp)
    movq $100, %rax
    movq %rax, -2472(%rbp)
    movq -2464(%rbp), %rax # t437 # t437
    addq -2472(%rbp), %rax
    movq %rax, -2480(%rbp)
    movq -2432(%rbp), %rdi # t433 # t433
    movq -2440(%rbp), %rsi # t434 # t434
    movq -2480(%rbp), %rdx # t439 # t439
    movq $0, %rax
    call memstorel
    movq %rax, -2488(%rbp)
    movq $0, %rax
    movq %rax, -2496(%rbp)
    leave
    ret
    jmp sublabel_end_406
sublabel_else_406: 
sublabel_end_406: 
for_post_368: 
    movq $1, %rax
    movq %rax, -2504(%rbp)
    movq -640(%rbp), %rax # x1 # x1
    addq -2504(%rbp), %rax
    movq %rax, -2512(%rbp)
# load t443 to x1
    movq -2512(%rbp), %rcx
    movq %rcx, -640(%rbp)
    jmp for_start_367
for_end_367: 
for_post_364: 
    movq $1, %rax
    movq %rax, -2520(%rbp)
    movq -648(%rbp), %rax # y1 # y1
    addq -2520(%rbp), %rax
    movq %rax, -2528(%rbp)
# load t445 to y1
    movq -2528(%rbp), %rcx
    movq %rcx, -648(%rbp)
    jmp for_start_363
for_end_363: 
    movq $0, %rax
    movq %rax, -2536(%rbp)
    leave
    ret
.globl move_blocks
move_blocks:
    pushq %rbp
    movq %rsp, %rbp
    subq $416, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq $0, %rax
    movq %rax, -16(%rbp)
for_start_447: 
    movq $11, %rax
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rax # x # x
    movq -24(%rbp), %rdi # t449 # t449
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -32(%rbp)
    movq -32(%rbp), %rax # t450 # t450
    cmpq $0, %rax
    je for_end_447
    movq $0, %rax
    movq %rax, -40(%rbp)
for_start_451: 
    movq $15, %rax
    movq %rax, -48(%rbp)
    movq -40(%rbp), %rax # y # y
    movq -48(%rbp), %rdi # t453 # t453
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -56(%rbp)
    movq -56(%rbp), %rax # t454 # t454
    cmpq $0, %rax
    je for_end_451
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -64(%rbp)
    movq $16, %rax
    movq %rax, -72(%rbp)
    movq -16(%rbp), %rax # x # x
    imulq -72(%rbp), %rax
    movq %rax, -80(%rbp)
    addq -40(%rbp), %rax
    movq %rax, -88(%rbp)
    movq -64(%rbp), %rdi # t455 # t455
    movq -88(%rbp), %rsi # t458 # t458
    movq $0, %rax
    call mematb
    movq %rax, -96(%rbp)
    movq %rax, -104(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -112(%rbp)
    movq $16, %rax
    movq %rax, -120(%rbp)
    movq -16(%rbp), %rax # x # x
    imulq -120(%rbp), %rax
    movq %rax, -128(%rbp)
    movq $1, %rax
    movq %rax, -136(%rbp)
    movq -40(%rbp), %rax # y # y
    addq -136(%rbp), %rax
    movq %rax, -144(%rbp)
    movq -128(%rbp), %rax # t462 # t462
    addq -144(%rbp), %rax
    movq %rax, -152(%rbp)
    movq -112(%rbp), %rdi # t460 # t460
    movq -152(%rbp), %rsi # t465 # t465
    movq $0, %rax
    call mematb
    movq %rax, -160(%rbp)
    movq %rax, -168(%rbp)
    movq $0, %rax
    movq %rax, -176(%rbp)
    movq -168(%rbp), %rsi # color2 # color2
    movq -176(%rbp), %rdi # t467 # t467
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -184(%rbp)
    movq $0, %rax
    movq %rax, -192(%rbp)
    movq -104(%rbp), %rax # color1 # color1
    movq -192(%rbp), %rdi # t469 # t469
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -200(%rbp)
    movq -184(%rbp), %rsi # t468 # t468
    cmpq $0, %rsi
    setne %al
    movq -200(%rbp), %rdi # t470 # t470
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -208(%rbp)
    cmpq $0, %rax
    je sublabel_else_472
sublabel_if_472: 
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -216(%rbp)
    movq $16, %rax
    movq %rax, -224(%rbp)
    movq -16(%rbp), %rax # x # x
    imulq -224(%rbp), %rax
    movq %rax, -232(%rbp)
    addq -40(%rbp), %rax
    movq %rax, -240(%rbp)
    movq $0, %rax
    movq %rax, -248(%rbp)
    movq -216(%rbp), %rdi # t473 # t473
    movq -240(%rbp), %rsi # t476 # t476
    movq -248(%rbp), %rdx # t477 # t477
    movq $0, %rax
    call memstoreb
    movq %rax, -256(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -264(%rbp)
    movq $16, %rax
    movq %rax, -272(%rbp)
    movq -16(%rbp), %rax # x # x
    imulq -272(%rbp), %rax
    movq %rax, -280(%rbp)
    movq $1, %rax
    movq %rax, -288(%rbp)
    movq -40(%rbp), %rax # y # y
    addq -288(%rbp), %rax
    movq %rax, -296(%rbp)
    movq -280(%rbp), %rax # t481 # t481
    addq -296(%rbp), %rax
    movq %rax, -304(%rbp)
    movq -264(%rbp), %rdi # t479 # t479
    movq -304(%rbp), %rsi # t484 # t484
    movq -104(%rbp), %rdx # color1 # color1
    movq $0, %rax
    call memstoreb
    movq %rax, -312(%rbp)
    movq $0, %rax
    movq %rax, -320(%rbp)
    leave
    ret
    jmp sublabel_end_472
sublabel_else_472: 
sublabel_end_472: 
for_post_452: 
    movq $1, %rax
    movq %rax, -328(%rbp)
    movq -40(%rbp), %rax # y # y
    addq -328(%rbp), %rax
    movq %rax, -336(%rbp)
# load t488 to y
    movq -336(%rbp), %rcx
    movq %rcx, -40(%rbp)
    jmp for_start_451
for_end_451: 
for_post_448: 
    movq $1, %rax
    movq %rax, -344(%rbp)
    movq -16(%rbp), %rax # x # x
    addq -344(%rbp), %rax
    movq %rax, -352(%rbp)
# load t490 to x
    movq -352(%rbp), %rcx
    movq %rcx, -16(%rbp)
    jmp for_start_447
for_end_447: 
    movq $0, %rax
    movq %rax, -360(%rbp)
    leave
    ret
.globl allocate_grid
allocate_grid:
    pushq %rbp
    movq %rsp, %rbp
    subq $144, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq -8(%rbp), %rax # w # w
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rax # h # h
    movq %rax, -32(%rbp)
    movq -24(%rbp), %rax # t492 # t492
    imulq -32(%rbp), %rax
    movq %rax, -40(%rbp)
    movq -40(%rbp), %rdi # t494 # t494
    movq $0, %rax
    call malloc
    movq %rax, -48(%rbp)
    movq %rax, -56(%rbp)
    movq -8(%rbp), %rax # w # w
    movq %rax, -64(%rbp)
    movq -16(%rbp), %rax # h # h
    movq %rax, -72(%rbp)
    movq -64(%rbp), %rax # t496 # t496
    imulq -72(%rbp), %rax
    movq %rax, -80(%rbp)
    movq -56(%rbp), %rdi # grid # grid
    movq -80(%rbp), %rsi # t498 # t498
    movq $0, %rax
    call memclr
    movq %rax, -88(%rbp)
    movq -56(%rbp), %rax # grid # grid
    leave
    ret
.globl allocate_block
allocate_block:
    pushq %rbp
    movq %rsp, %rbp
    subq $112, %rsp
    movq $0, %rcx
    movq $11, %rax
    movq %rax, -8(%rbp)
    movq -8(%rbp), %rdi # t500 # t500
    movq $0, %rax
    call malloc
    movq %rax, -16(%rbp)
    movq %rax, -24(%rbp)
    movq $10, %rax
    movq %rax, -32(%rbp)
    movq -24(%rbp), %rdi # block # block
    movq -32(%rbp), %rsi # t502 # t502
    movq $0, %rax
    call memclr
    movq %rax, -40(%rbp)
    movq -24(%rbp), %rax # block # block
    leave
    ret
.globl setup_block
setup_block:
    pushq %rbp
    movq %rsp, %rbp
    subq $480, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq $0, %rax
    movq %rax, -16(%rbp)
    movq $1, %rax
    movq %rax, -24(%rbp)
    movq $0, %rax
    call rand
    movq %rax, -32(%rbp)
    movq $3, %rax
    movq %rax, -40(%rbp)
    movq -32(%rbp), %rax # t506 # t506
    cqto
    movq -40(%rbp), %rdi # t507 # t507
    idivq %rdi
    movq %rdx, -48(%rbp)
    movq -24(%rbp), %rax # t505 # t505
    addq -48(%rbp), %rax
    movq %rax, -56(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -16(%rbp), %rsi # t504 # t504
    movq -56(%rbp), %rdx # t509 # t509
    movq $0, %rax
    call memstoreb
    movq %rax, -64(%rbp)
    movq $1, %rax
    movq %rax, -72(%rbp)
    movq $11, %rax
    movq %rax, -80(%rbp)
    movq $2, %rax
    movq %rax, -88(%rbp)
    movq -80(%rbp), %rax # t512 # t512
    cqto
    idivq -88(%rbp)
    movq %rax, -96(%rbp)
    movq $1, %rax
    movq %rax, -104(%rbp)
    movq -96(%rbp), %rax # t514 # t514
    subq -104(%rbp), %rax
    movq %rax, -112(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -72(%rbp), %rsi # t511 # t511
    movq -112(%rbp), %rdx # t516 # t516
    movq $0, %rax
    call memstoreb
    movq %rax, -120(%rbp)
    movq $2, %rax
    movq %rax, -128(%rbp)
    movq $0, %rax
    movq %rax, -136(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -128(%rbp), %rsi # t518 # t518
    movq -136(%rbp), %rdx # t519 # t519
    movq $0, %rax
    call memstoreb
    movq %rax, -144(%rbp)
    movq $3, %rax
    movq %rax, -152(%rbp)
    movq $1, %rax
    movq %rax, -160(%rbp)
    movq $0, %rax
    call rand
    movq %rax, -168(%rbp)
    movq $3, %rax
    movq %rax, -176(%rbp)
    movq -168(%rbp), %rax # t523 # t523
    cqto
    movq -176(%rbp), %rdi # t524 # t524
    idivq %rdi
    movq %rdx, -184(%rbp)
    movq -160(%rbp), %rax # t522 # t522
    addq -184(%rbp), %rax
    movq %rax, -192(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -152(%rbp), %rsi # t521 # t521
    movq -192(%rbp), %rdx # t526 # t526
    movq $0, %rax
    call memstoreb
    movq %rax, -200(%rbp)
    movq $4, %rax
    movq %rax, -208(%rbp)
    movq $11, %rax
    movq %rax, -216(%rbp)
    movq $2, %rax
    movq %rax, -224(%rbp)
    movq -216(%rbp), %rax # t529 # t529
    cqto
    idivq -224(%rbp)
    movq %rax, -232(%rbp)
    movq $1, %rax
    movq %rax, -240(%rbp)
    movq -232(%rbp), %rax # t531 # t531
    subq -240(%rbp), %rax
    movq %rax, -248(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -208(%rbp), %rsi # t528 # t528
    movq -248(%rbp), %rdx # t533 # t533
    movq $0, %rax
    call memstoreb
    movq %rax, -256(%rbp)
    movq $5, %rax
    movq %rax, -264(%rbp)
    movq $1, %rax
    movq %rax, -272(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -264(%rbp), %rsi # t535 # t535
    movq -272(%rbp), %rdx # t536 # t536
    movq $0, %rax
    call memstoreb
    movq %rax, -280(%rbp)
    movq $6, %rax
    movq %rax, -288(%rbp)
    movq $1, %rax
    movq %rax, -296(%rbp)
    movq $0, %rax
    call rand
    movq %rax, -304(%rbp)
    movq $3, %rax
    movq %rax, -312(%rbp)
    movq -304(%rbp), %rax # t540 # t540
    cqto
    movq -312(%rbp), %rdi # t541 # t541
    idivq %rdi
    movq %rdx, -320(%rbp)
    movq -296(%rbp), %rax # t539 # t539
    addq -320(%rbp), %rax
    movq %rax, -328(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -288(%rbp), %rsi # t538 # t538
    movq -328(%rbp), %rdx # t543 # t543
    movq $0, %rax
    call memstoreb
    movq %rax, -336(%rbp)
    movq $7, %rax
    movq %rax, -344(%rbp)
    movq $11, %rax
    movq %rax, -352(%rbp)
    movq $2, %rax
    movq %rax, -360(%rbp)
    movq -352(%rbp), %rax # t546 # t546
    cqto
    idivq -360(%rbp)
    movq %rax, -368(%rbp)
    movq $1, %rax
    movq %rax, -376(%rbp)
    movq -368(%rbp), %rax # t548 # t548
    subq -376(%rbp), %rax
    movq %rax, -384(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -344(%rbp), %rsi # t545 # t545
    movq -384(%rbp), %rdx # t550 # t550
    movq $0, %rax
    call memstoreb
    movq %rax, -392(%rbp)
    movq $8, %rax
    movq %rax, -400(%rbp)
    movq $2, %rax
    movq %rax, -408(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -400(%rbp), %rsi # t552 # t552
    movq -408(%rbp), %rdx # t553 # t553
    movq $0, %rax
    call memstoreb
    movq %rax, -416(%rbp)
    movq $0, %rax
    movq %rax, -424(%rbp)
    leave
    ret
.globl set_block_in_grid
set_block_in_grid:
    pushq %rbp
    movq %rsp, %rbp
    subq $368, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq $0, %rax
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -24(%rbp), %rsi # t556 # t556
    movq $0, %rax
    call mematb
    movq %rax, -32(%rbp)
    movq %rax, -40(%rbp)
    movq $1, %rax
    movq %rax, -48(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -48(%rbp), %rsi # t558 # t558
    movq $0, %rax
    call mematb
    movq %rax, -56(%rbp)
    movq %rax, -64(%rbp)
    movq $2, %rax
    movq %rax, -72(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -72(%rbp), %rsi # t560 # t560
    movq $0, %rax
    call mematb
    movq %rax, -80(%rbp)
    movq %rax, -88(%rbp)
    movq $2, %rax
    movq %rax, -96(%rbp)
    movq -88(%rbp), %rax # by # by
    movq -96(%rbp), %rdi # t562 # t562
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -104(%rbp)
    movq -104(%rbp), %rax # t563 # t563
    cmpq $0, %rax
    je sublabel_else_564
sublabel_if_564: 
    movq $1, %rax
    movq %rax, -112(%rbp)
    leave
    ret
    jmp sublabel_end_564
sublabel_else_564: 
sublabel_end_564: 
    movq $16, %rax
    movq %rax, -120(%rbp)
    movq -64(%rbp), %rax # bx # bx
    imulq -120(%rbp), %rax
    movq %rax, -128(%rbp)
    addq -88(%rbp), %rax
    movq %rax, -136(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -136(%rbp), %rsi # t568 # t568
    movq -40(%rbp), %rdx # val # val
    movq $0, %rax
    call memstoreb
    movq %rax, -144(%rbp)
    movq $3, %rax
    movq %rax, -152(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -152(%rbp), %rsi # t570 # t570
    movq $0, %rax
    call mematb
    movq %rax, -160(%rbp)
# load t571 to val
    movq -160(%rbp), %rcx
    movq %rcx, -40(%rbp)
    movq $4, %rax
    movq %rax, -168(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -168(%rbp), %rsi # t572 # t572
    movq $0, %rax
    call mematb
    movq %rax, -176(%rbp)
# load t573 to bx
    movq -176(%rbp), %rcx
    movq %rcx, -64(%rbp)
    movq $5, %rax
    movq %rax, -184(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -184(%rbp), %rsi # t574 # t574
    movq $0, %rax
    call mematb
    movq %rax, -192(%rbp)
# load t575 to by
    movq -192(%rbp), %rcx
    movq %rcx, -88(%rbp)
    movq $16, %rax
    movq %rax, -200(%rbp)
    movq -64(%rbp), %rax # bx # bx
    imulq -200(%rbp), %rax
    movq %rax, -208(%rbp)
    addq -88(%rbp), %rax
    movq %rax, -216(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -216(%rbp), %rsi # t578 # t578
    movq -40(%rbp), %rdx # val # val
    movq $0, %rax
    call memstoreb
    movq %rax, -224(%rbp)
    movq $6, %rax
    movq %rax, -232(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -232(%rbp), %rsi # t580 # t580
    movq $0, %rax
    call mematb
    movq %rax, -240(%rbp)
# load t581 to val
    movq -240(%rbp), %rcx
    movq %rcx, -40(%rbp)
    movq $7, %rax
    movq %rax, -248(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -248(%rbp), %rsi # t582 # t582
    movq $0, %rax
    call mematb
    movq %rax, -256(%rbp)
# load t583 to bx
    movq -256(%rbp), %rcx
    movq %rcx, -64(%rbp)
    movq $8, %rax
    movq %rax, -264(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -264(%rbp), %rsi # t584 # t584
    movq $0, %rax
    call mematb
    movq %rax, -272(%rbp)
# load t585 to by
    movq -272(%rbp), %rcx
    movq %rcx, -88(%rbp)
    movq $16, %rax
    movq %rax, -280(%rbp)
    movq -64(%rbp), %rax # bx # bx
    imulq -280(%rbp), %rax
    movq %rax, -288(%rbp)
    addq -88(%rbp), %rax
    movq %rax, -296(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -296(%rbp), %rsi # t588 # t588
    movq -40(%rbp), %rdx # val # val
    movq $0, %rax
    call memstoreb
    movq %rax, -304(%rbp)
    movq $0, %rax
    movq %rax, -312(%rbp)
    leave
    ret
.globl move_block_down
move_block_down:
    pushq %rbp
    movq %rsp, %rbp
    subq $432, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq $7, %rax
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -24(%rbp), %rsi # t591 # t591
    movq $0, %rax
    call mematb
    movq %rax, -32(%rbp)
    movq %rax, -40(%rbp)
    movq $8, %rax
    movq %rax, -48(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -48(%rbp), %rsi # t593 # t593
    movq $0, %rax
    call mematb
    movq %rax, -56(%rbp)
    movq $1, %rax
    movq %rax, -64(%rbp)
    movq -56(%rbp), %rax # t594 # t594
    addq -64(%rbp), %rax
    movq %rax, -72(%rbp)
    movq %rax, -80(%rbp)
    movq $16, %rax
    movq %rax, -88(%rbp)
    movq -40(%rbp), %rax # bx # bx
    imulq -88(%rbp), %rax
    movq %rax, -96(%rbp)
    addq -80(%rbp), %rax
    movq %rax, -104(%rbp)
    movq %rax, -112(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -112(%rbp), %rsi # pos # pos
    movq $0, %rax
    call mematb
    movq %rax, -120(%rbp)
    movq %rax, -128(%rbp)
    movq $8, %rax
    movq %rax, -136(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -136(%rbp), %rsi # t601 # t601
    movq $0, %rax
    call mematb
    movq %rax, -144(%rbp)
    movq $15, %rax
    movq %rax, -152(%rbp)
    movq -144(%rbp), %rax # t602 # t602
    movq -152(%rbp), %rdi # t603 # t603
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -160(%rbp)
    movq $0, %rax
    movq %rax, -168(%rbp)
    movq -128(%rbp), %rsi # test_block # test_block
    movq -168(%rbp), %rdi # t605 # t605
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -176(%rbp)
    movq -160(%rbp), %rsi # t604 # t604
    cmpq $0, %rsi
    setne %al
    movq -176(%rbp), %rdi # t606 # t606
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -184(%rbp)
    cmpq $0, %rax
    je sublabel_else_608
sublabel_if_608: 
    movq $2, %rax
    movq %rax, -192(%rbp)
    movq $2, %rax
    movq %rax, -200(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -200(%rbp), %rsi # t610 # t610
    movq $0, %rax
    call mematb
    movq %rax, -208(%rbp)
    movq $1, %rax
    movq %rax, -216(%rbp)
    movq -208(%rbp), %rax # t611 # t611
    addq -216(%rbp), %rax
    movq %rax, -224(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -192(%rbp), %rsi # t609 # t609
    movq -224(%rbp), %rdx # t613 # t613
    movq $0, %rax
    call memstoreb
    movq %rax, -232(%rbp)
    movq $5, %rax
    movq %rax, -240(%rbp)
    movq $5, %rax
    movq %rax, -248(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -248(%rbp), %rsi # t616 # t616
    movq $0, %rax
    call mematb
    movq %rax, -256(%rbp)
    movq $1, %rax
    movq %rax, -264(%rbp)
    movq -256(%rbp), %rax # t617 # t617
    addq -264(%rbp), %rax
    movq %rax, -272(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -240(%rbp), %rsi # t615 # t615
    movq -272(%rbp), %rdx # t619 # t619
    movq $0, %rax
    call memstoreb
    movq %rax, -280(%rbp)
    movq $8, %rax
    movq %rax, -288(%rbp)
    movq $8, %rax
    movq %rax, -296(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -296(%rbp), %rsi # t622 # t622
    movq $0, %rax
    call mematb
    movq %rax, -304(%rbp)
    movq $1, %rax
    movq %rax, -312(%rbp)
    movq -304(%rbp), %rax # t623 # t623
    addq -312(%rbp), %rax
    movq %rax, -320(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -288(%rbp), %rsi # t621 # t621
    movq -320(%rbp), %rdx # t625 # t625
    movq $0, %rax
    call memstoreb
    movq %rax, -328(%rbp)
    jmp sublabel_end_608
sublabel_else_608: 
    movq -8(%rbp), %rdi # grid # grid
    movq -16(%rbp), %rsi # block # block
    movq $0, %rax
    call set_block_in_grid
    movq %rax, -336(%rbp)
    movq $1, %rax
    movq %rax, -344(%rbp)
    movq -336(%rbp), %rsi # t627 # t627
    movq -344(%rbp), %rdi # t628 # t628
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -352(%rbp)
    movq -352(%rbp), %rax # t629 # t629
    cmpq $0, %rax
    je sublabel_else_630
sublabel_if_630: 
    movq $1, %rax
    movq %rax, -360(%rbp)
    leave
    ret
    jmp sublabel_end_630
sublabel_else_630: 
sublabel_end_630: 
    movq -16(%rbp), %rdi # block # block
    movq $0, %rax
    call setup_block
    movq %rax, -368(%rbp)
sublabel_end_608: 
    movq $0, %rax
    movq %rax, -376(%rbp)
    leave
    ret
.globl swap_colors
swap_colors:
    pushq %rbp
    movq %rsp, %rbp
    subq $192, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq $0, %rax
    movq %rax, -16(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -16(%rbp), %rsi # t634 # t634
    movq $0, %rax
    call mematb
    movq %rax, -24(%rbp)
    movq %rax, -32(%rbp)
    movq $3, %rax
    movq %rax, -40(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -40(%rbp), %rsi # t636 # t636
    movq $0, %rax
    call mematb
    movq %rax, -48(%rbp)
    movq %rax, -56(%rbp)
    movq $6, %rax
    movq %rax, -64(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -64(%rbp), %rsi # t638 # t638
    movq $0, %rax
    call mematb
    movq %rax, -72(%rbp)
    movq %rax, -80(%rbp)
    movq $0, %rax
    movq %rax, -88(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -88(%rbp), %rsi # t640 # t640
    movq -80(%rbp), %rdx # c3 # c3
    movq $0, %rax
    call memstoreb
    movq %rax, -96(%rbp)
    movq $3, %rax
    movq %rax, -104(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -104(%rbp), %rsi # t642 # t642
    movq -32(%rbp), %rdx # c1 # c1
    movq $0, %rax
    call memstoreb
    movq %rax, -112(%rbp)
    movq $6, %rax
    movq %rax, -120(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -120(%rbp), %rsi # t644 # t644
    movq -56(%rbp), %rdx # c2 # c2
    movq $0, %rax
    call memstoreb
    movq %rax, -128(%rbp)
    movq $0, %rax
    movq %rax, -136(%rbp)
    leave
    ret
.globl init
init:
    pushq %rbp
    movq %rsp, %rbp
    subq $1680, %rsp
    movq $0, %rcx
    movq $0, %rax
    call sdl_init
    movq %rax, -8(%rbp)
    leaq t648(%rip), %rax
    movq %rax, -16(%rbp)
    movq $1440, %rax
    movq %rax, -24(%rbp)
    movq $1080, %rax
    movq %rax, -32(%rbp)
    movq -16(%rbp), %rdi # t648 # t648
    movq -24(%rbp), %rsi # t649 # t649
    movq -32(%rbp), %rdx # t650 # t650
    movq $0, %rax
    call sdl_create
    movq %rax, -40(%rbp)
    movq $0, %rax
    movq %rax, -48(%rbp)
    movq -48(%rbp), %rdi # t652 # t652
    movq $0, %rax
    call time
    movq %rax, -56(%rbp)
    movq -56(%rbp), %rdi # t653 # t653
    movq $0, %rax
    call srand
    movq %rax, -64(%rbp)
    movq $11, %rax
    movq %rax, -72(%rbp)
    movq $16, %rax
    movq %rax, -80(%rbp)
    movq -72(%rbp), %rdi # t655 # t655
    movq -80(%rbp), %rsi # t656 # t656
    movq $0, %rax
    call allocate_grid
    movq %rax, -88(%rbp)
    movq %rax, -96(%rbp)
    movq $0, %rax
    call allocate_block
    movq %rax, -104(%rbp)
    movq %rax, -112(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq $0, %rax
    call setup_block
    movq %rax, -120(%rbp)
    movq $0, %rax
    call sdl_getticks
    movq %rax, -128(%rbp)
    movq %rax, -136(%rbp)
    movq $0, %rax
    call sdl_getticks
    movq %rax, -144(%rbp)
    movq %rax, -152(%rbp)
    movq $8, %rax
    movq %rax, -160(%rbp)
    movq $2, %rax
    movq %rax, -168(%rbp)
    movq -160(%rbp), %rax # t662 # t662
    imulq -168(%rbp), %rax
    movq %rax, -176(%rbp)
    movq -176(%rbp), %rdi # t664 # t664
    movq $0, %rax
    call malloc
    movq %rax, -184(%rbp)
    movq %rax, -192(%rbp)
    movq $0, %rax
    movq %rax, -200(%rbp)
    movq $0, %rax
    movq %rax, -208(%rbp)
    movq -192(%rbp), %rdi # score # score
    movq -200(%rbp), %rsi # t666 # t666
    movq -208(%rbp), %rdx # t667 # t667
    movq $0, %rax
    call memstorel
    movq %rax, -216(%rbp)
    movq $0, %rax
    movq %rax, -224(%rbp)
    leaq t669(%rip), %rax
    movq %rax, -232(%rbp)
    movq -232(%rbp), %rdi # t669 # t669
    movq $0, %rax
    call sdl_loadtex
    movq %rax, -240(%rbp)
    movq %rax, -248(%rbp)
while_start_671: 
    movq $0, %rax
    call sdl_pump
    movq %rax, -256(%rbp)
    movq -224(%rbp), %rax # game_over # game_over
    movq %rax, -264(%rbp)
    movq $0, %rax
    movq %rax, -272(%rbp)
    movq -264(%rbp), %rsi # t673 # t673
    movq -272(%rbp), %rdi # t674 # t674
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -280(%rbp)
    movq -256(%rbp), %rsi # t672 # t672
    cmpq $0, %rsi
    setne %al
    movq -280(%rbp), %rdi # t675 # t675
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -288(%rbp)
    cmpq $0, %rax
    je while_end_671
    movq $0, %rax
    call sdl_clear
    movq %rax, -296(%rbp)
    movq $0, %rax
    movq %rax, -304(%rbp)
    movq $0, %rax
    movq %rax, -312(%rbp)
    movq $1440, %rax
    movq %rax, -320(%rbp)
    movq $1080, %rax
    movq %rax, -328(%rbp)
    movq -248(%rbp), %rdi # puzzle_bg # puzzle_bg
    movq -304(%rbp), %rsi # t678 # t678
    movq -312(%rbp), %rdx # t679 # t679
    movq -320(%rbp), %rcx # t680 # t680
    movq -328(%rbp), %r8 # t681 # t681
    movq $0, %rax
    call sdl_copytex
    movq %rax, -336(%rbp)
    movq -96(%rbp), %rdi # grid # grid
    movq -112(%rbp), %rsi # block # block
    movq $0, %rax
    call draw_grid
    movq %rax, -344(%rbp)
    movq $255, %rax
    movq %rax, -352(%rbp)
    movq $255, %rax
    movq %rax, -360(%rbp)
    movq $255, %rax
    movq %rax, -368(%rbp)
    movq $255, %rax
    movq %rax, -376(%rbp)
    movq -352(%rbp), %rdi # t684 # t684
    movq -360(%rbp), %rsi # t685 # t685
    movq -368(%rbp), %rdx # t686 # t686
    movq -376(%rbp), %rcx # t687 # t687
    movq $0, %rax
    call sdl_settextcolor
    movq %rax, -384(%rbp)
    movq $25, %rax
    movq %rax, -392(%rbp)
    movq $25, %rax
    movq %rax, -400(%rbp)
    leaq t691(%rip), %rax
    movq %rax, -408(%rbp)
    movq $0, %rax
    movq %rax, -416(%rbp)
    movq -192(%rbp), %rdi # score # score
    movq -416(%rbp), %rsi # t692 # t692
    movq $0, %rax
    call mematl
    movq %rax, -424(%rbp)
    movq -424(%rbp), %rdi # t693 # t693
    movq $0, %rax
    call str
    movq %rax, -432(%rbp)
    addq $22, -440(%rbp)
    movq $0, %rcx
    movq %rcx, -440(%rbp)
    addq $10, -440(%rbp)
    addq $1, -440(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -440(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -448(%rbp)
    movq -408(%rbp), %rsi # t691 # t691
    call strcpy
    movq -432(%rbp), %rsi # t694 # t694
    call strcat
    movq -392(%rbp), %rdi # t689 # t689
    movq -400(%rbp), %rsi # t690 # t690
    movq -448(%rbp), %rdx # t695 # t695
    movq $0, %rax
    call sdl_printtext
    movq %rax, -456(%rbp)
    movq -96(%rbp), %rdi # grid # grid
    movq -192(%rbp), %rsi # score # score
    movq $0, %rax
    call check_blocks
    movq %rax, -464(%rbp)
    movq -96(%rbp), %rdi # grid # grid
    movq $0, %rax
    call move_blocks
    movq %rax, -472(%rbp)
    movq $0, %rax
    call sdl_flip
    movq %rax, -480(%rbp)
    movq $0, %rax
    call sdl_getticks
    movq %rax, -488(%rbp)
    movq %rax, -496(%rbp)
    subq -152(%rbp), %rax
    movq %rax, -504(%rbp)
    movq $750, %rax
    movq %rax, -512(%rbp)
    movq -504(%rbp), %rax # t701 # t701
    movq -512(%rbp), %rdi # t702 # t702
    cmpq %rdi, %rax
    setge %cl
    movzbq %cl, %rdx
    movq %rdx, -520(%rbp)
    movq -520(%rbp), %rax # t703 # t703
    cmpq $0, %rax
    je sublabel_else_704
sublabel_if_704: 
    movq -96(%rbp), %rdi # grid # grid
    movq -112(%rbp), %rsi # block # block
    movq $0, %rax
    call move_block_down
    movq %rax, -528(%rbp)
    movq $1, %rax
    movq %rax, -536(%rbp)
    movq -528(%rbp), %rsi # t705 # t705
    movq -536(%rbp), %rdi # t706 # t706
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -544(%rbp)
    movq -544(%rbp), %rax # t707 # t707
    cmpq $0, %rax
    je sublabel_else_708
sublabel_if_708: 
    movq $1, %rcx # here
    movq %rcx, -224(%rbp)
    jmp sublabel_end_708
sublabel_else_708: 
sublabel_end_708: 
# load ctime to update_time
    movq -496(%rbp), %rcx
    movq %rcx, -152(%rbp)
    jmp sublabel_end_704
sublabel_else_704: 
sublabel_end_704: 
    movq $0, %rax
    call sdl_getticks
    movq %rax, -552(%rbp)
    movq %rax, -560(%rbp)
    subq -136(%rbp), %rax
    movq %rax, -568(%rbp)
    movq $100, %rax
    movq %rax, -576(%rbp)
    movq -568(%rbp), %rax # t710 # t710
    movq -576(%rbp), %rdi # t711 # t711
    cmpq %rdi, %rax
    setge %cl
    movzbq %cl, %rdx
    movq %rdx, -584(%rbp)
    movq -584(%rbp), %rax # t712 # t712
    cmpq $0, %rax
    je sublabel_else_713
sublabel_if_713: 
# load current_time to prev_time
    movq -560(%rbp), %rcx
    movq %rcx, -136(%rbp)
    movq $7, %rax
    movq %rax, -592(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -592(%rbp), %rsi # t714 # t714
    movq $0, %rax
    call mematb
    movq %rax, -600(%rbp)
    movq $1, %rax
    movq %rax, -608(%rbp)
    movq -600(%rbp), %rax # t715 # t715
    addq -608(%rbp), %rax
    movq %rax, -616(%rbp)
    movq %rax, -624(%rbp)
    movq $8, %rax
    movq %rax, -632(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -632(%rbp), %rsi # t718 # t718
    movq $0, %rax
    call mematb
    movq %rax, -640(%rbp)
    movq %rax, -648(%rbp)
    movq $16, %rax
    movq %rax, -656(%rbp)
    movq -624(%rbp), %rax # bx # bx
    imulq -656(%rbp), %rax
    movq %rax, -664(%rbp)
    addq -648(%rbp), %rax
    movq %rax, -672(%rbp)
    movq %rax, -680(%rbp)
    movq -96(%rbp), %rdi # grid # grid
    movq -680(%rbp), %rsi # pos # pos
    movq $0, %rax
    call mematb
    movq %rax, -688(%rbp)
    movq %rax, -696(%rbp)
    movq $79, %rax
    movq %rax, -704(%rbp)
    movq -704(%rbp), %rdi # t724 # t724
    movq $0, %rax
    call sdl_keydown
    movq %rax, -712(%rbp)
    movq $0, %rax
    movq %rax, -720(%rbp)
    movq -696(%rbp), %rsi # test_block # test_block
    movq -720(%rbp), %rdi # t726 # t726
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -728(%rbp)
    movq -712(%rbp), %rsi # t725 # t725
    cmpq $0, %rsi
    setne %al
    movq -728(%rbp), %rdi # t727 # t727
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -736(%rbp)
    movq $1, %rax
    movq %rax, -744(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -744(%rbp), %rsi # t729 # t729
    movq $0, %rax
    call mematb
    movq %rax, -752(%rbp)
    movq $1440, %rax
    movq %rax, -760(%rbp)
    movq $32, %rax
    movq %rax, -768(%rbp)
    movq $4, %rax
    movq %rax, -776(%rbp)
    movq -768(%rbp), %rax # t732 # t732
    imulq -776(%rbp), %rax
    movq %rax, -784(%rbp)
    movq -760(%rbp), %rax # t731 # t731
    cqto
    idivq -784(%rbp)
    movq %rax, -792(%rbp)
    movq $1, %rax
    movq %rax, -800(%rbp)
    movq -792(%rbp), %rax # t735 # t735
    subq -800(%rbp), %rax
    movq %rax, -808(%rbp)
    movq -752(%rbp), %rax # t730 # t730
    movq -808(%rbp), %rdi # t737 # t737
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -816(%rbp)
    movq -736(%rbp), %rsi # t728 # t728
    cmpq $0, %rsi
    setne %al
    movq -816(%rbp), %rdi # t738 # t738
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -824(%rbp)
    cmpq $0, %rax
    je sublabel_else_740
sublabel_if_740: 
    movq $1, %rax
    movq %rax, -832(%rbp)
    movq $1, %rax
    movq %rax, -840(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -840(%rbp), %rsi # t742 # t742
    movq $0, %rax
    call mematb
    movq %rax, -848(%rbp)
    movq $1, %rax
    movq %rax, -856(%rbp)
    movq -848(%rbp), %rax # t743 # t743
    addq -856(%rbp), %rax
    movq %rax, -864(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -832(%rbp), %rsi # t741 # t741
    movq -864(%rbp), %rdx # t745 # t745
    movq $0, %rax
    call memstoreb
    movq %rax, -872(%rbp)
    movq $4, %rax
    movq %rax, -880(%rbp)
    movq $4, %rax
    movq %rax, -888(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -888(%rbp), %rsi # t748 # t748
    movq $0, %rax
    call mematb
    movq %rax, -896(%rbp)
    movq $1, %rax
    movq %rax, -904(%rbp)
    movq -896(%rbp), %rax # t749 # t749
    addq -904(%rbp), %rax
    movq %rax, -912(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -880(%rbp), %rsi # t747 # t747
    movq -912(%rbp), %rdx # t751 # t751
    movq $0, %rax
    call memstoreb
    movq %rax, -920(%rbp)
    movq $7, %rax
    movq %rax, -928(%rbp)
    movq $7, %rax
    movq %rax, -936(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -936(%rbp), %rsi # t754 # t754
    movq $0, %rax
    call mematb
    movq %rax, -944(%rbp)
    movq $1, %rax
    movq %rax, -952(%rbp)
    movq -944(%rbp), %rax # t755 # t755
    addq -952(%rbp), %rax
    movq %rax, -960(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -928(%rbp), %rsi # t753 # t753
    movq -960(%rbp), %rdx # t757 # t757
    movq $0, %rax
    call memstoreb
    movq %rax, -968(%rbp)
    jmp sublabel_end_740
sublabel_else_740: 
    movq $7, %rax
    movq %rax, -976(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -976(%rbp), %rsi # t759 # t759
    movq $0, %rax
    call mematb
    movq %rax, -984(%rbp)
    movq $1, %rax
    movq %rax, -992(%rbp)
    movq -984(%rbp), %rax # t760 # t760
    subq -992(%rbp), %rax
    movq %rax, -1000(%rbp)
# load t762 to bx
    movq -1000(%rbp), %rcx
    movq %rcx, -624(%rbp)
    movq $8, %rax
    movq %rax, -1008(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -1008(%rbp), %rsi # t763 # t763
    movq $0, %rax
    call mematb
    movq %rax, -1016(%rbp)
# load t764 to by
    movq -1016(%rbp), %rcx
    movq %rcx, -648(%rbp)
    movq $16, %rax
    movq %rax, -1024(%rbp)
    movq -624(%rbp), %rax # bx # bx
    imulq -1024(%rbp), %rax
    movq %rax, -1032(%rbp)
    addq -648(%rbp), %rax
    movq %rax, -1040(%rbp)
# load t767 to pos
    movq -1040(%rbp), %rcx
    movq %rcx, -680(%rbp)
    movq -96(%rbp), %rdi # grid # grid
    movq -680(%rbp), %rsi # pos # pos
    movq $0, %rax
    call mematb
    movq %rax, -1048(%rbp)
# load t768 to test_block
    movq -1048(%rbp), %rcx
    movq %rcx, -696(%rbp)
    movq $80, %rax
    movq %rax, -1056(%rbp)
    movq -1056(%rbp), %rdi # t769 # t769
    movq $0, %rax
    call sdl_keydown
    movq %rax, -1064(%rbp)
    movq $0, %rax
    movq %rax, -1072(%rbp)
    movq -696(%rbp), %rsi # test_block # test_block
    movq -1072(%rbp), %rdi # t771 # t771
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -1080(%rbp)
    movq -1064(%rbp), %rsi # t770 # t770
    cmpq $0, %rsi
    setne %al
    movq -1080(%rbp), %rdi # t772 # t772
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -1088(%rbp)
    movq $1, %rax
    movq %rax, -1096(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -1096(%rbp), %rsi # t774 # t774
    movq $0, %rax
    call mematb
    movq %rax, -1104(%rbp)
    movq $0, %rax
    movq %rax, -1112(%rbp)
    movq -1104(%rbp), %rax # t775 # t775
    movq -1112(%rbp), %rdi # t776 # t776
    cmpq %rdi, %rax
    setg %cl
    movzbq %cl, %rdx
    movq %rdx, -1120(%rbp)
    movq -1088(%rbp), %rsi # t773 # t773
    cmpq $0, %rsi
    setne %al
    movq -1120(%rbp), %rdi # t777 # t777
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -1128(%rbp)
    cmpq $0, %rax
    je sublabel_else_779
sublabel_if_779: 
    movq $1, %rax
    movq %rax, -1136(%rbp)
    movq $1, %rax
    movq %rax, -1144(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -1144(%rbp), %rsi # t781 # t781
    movq $0, %rax
    call mematb
    movq %rax, -1152(%rbp)
    movq $1, %rax
    movq %rax, -1160(%rbp)
    movq -1152(%rbp), %rax # t782 # t782
    subq -1160(%rbp), %rax
    movq %rax, -1168(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -1136(%rbp), %rsi # t780 # t780
    movq -1168(%rbp), %rdx # t784 # t784
    movq $0, %rax
    call memstoreb
    movq %rax, -1176(%rbp)
    movq $4, %rax
    movq %rax, -1184(%rbp)
    movq $4, %rax
    movq %rax, -1192(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -1192(%rbp), %rsi # t787 # t787
    movq $0, %rax
    call mematb
    movq %rax, -1200(%rbp)
    movq $1, %rax
    movq %rax, -1208(%rbp)
    movq -1200(%rbp), %rax # t788 # t788
    subq -1208(%rbp), %rax
    movq %rax, -1216(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -1184(%rbp), %rsi # t786 # t786
    movq -1216(%rbp), %rdx # t790 # t790
    movq $0, %rax
    call memstoreb
    movq %rax, -1224(%rbp)
    movq $7, %rax
    movq %rax, -1232(%rbp)
    movq $7, %rax
    movq %rax, -1240(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -1240(%rbp), %rsi # t793 # t793
    movq $0, %rax
    call mematb
    movq %rax, -1248(%rbp)
    movq $1, %rax
    movq %rax, -1256(%rbp)
    movq -1248(%rbp), %rax # t794 # t794
    subq -1256(%rbp), %rax
    movq %rax, -1264(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -1232(%rbp), %rsi # t792 # t792
    movq -1264(%rbp), %rdx # t796 # t796
    movq $0, %rax
    call memstoreb
    movq %rax, -1272(%rbp)
    jmp sublabel_end_779
sublabel_else_779: 
    movq $81, %rax
    movq %rax, -1280(%rbp)
    movq -1280(%rbp), %rdi # t798 # t798
    movq $0, %rax
    call sdl_keydown
    movq %rax, -1288(%rbp)
    cmpq $0, %rax
    je sublabel_else_800
sublabel_if_800: 
    movq -96(%rbp), %rdi # grid # grid
    movq -112(%rbp), %rsi # block # block
    movq $0, %rax
    call move_block_down
    movq %rax, -1296(%rbp)
    jmp sublabel_end_800
sublabel_else_800: 
    movq $82, %rax
    movq %rax, -1304(%rbp)
    movq -1304(%rbp), %rdi # t802 # t802
    movq $0, %rax
    call sdl_keydown
    movq %rax, -1312(%rbp)
    cmpq $0, %rax
    je sublabel_else_804
sublabel_if_804: 
    movq -112(%rbp), %rdi # block # block
    movq $0, %rax
    call swap_colors
    movq %rax, -1320(%rbp)
    jmp sublabel_end_804
sublabel_else_804: 
sublabel_end_804: 
sublabel_end_800: 
sublabel_end_779: 
sublabel_end_740: 
    jmp sublabel_end_713
sublabel_else_713: 
sublabel_end_713: 
    jmp while_start_671
while_end_671: 
    leaq t806(%rip), %rax
    movq %rax, -1328(%rbp)
    movq -1328(%rbp), %rdi # t806 # t806
    movq $0, %rax
    call sdl_loadtex
    movq %rax, -1336(%rbp)
    movq %rax, -1344(%rbp)
while_start_808: 
    movq $0, %rax
    call sdl_pump
    movq %rax, -1352(%rbp)
    cmpq $0, %rax
    je while_end_808
    movq $0, %rax
    call sdl_clear
    movq %rax, -1360(%rbp)
    movq $0, %rax
    movq %rax, -1368(%rbp)
    movq $0, %rax
    movq %rax, -1376(%rbp)
    movq $1440, %rax
    movq %rax, -1384(%rbp)
    movq $1080, %rax
    movq %rax, -1392(%rbp)
    movq -1344(%rbp), %rdi # game_over_screen # game_over_screen
    movq -1368(%rbp), %rsi # t811 # t811
    movq -1376(%rbp), %rdx # t812 # t812
    movq -1384(%rbp), %rcx # t813 # t813
    movq -1392(%rbp), %r8 # t814 # t814
    movq $0, %rax
    call sdl_copytex
    movq %rax, -1400(%rbp)
    movq $0, %rax
    movq %rax, -1408(%rbp)
    movq $0, %rax
    movq %rax, -1416(%rbp)
    movq $0, %rax
    movq %rax, -1424(%rbp)
    movq $255, %rax
    movq %rax, -1432(%rbp)
    movq -1408(%rbp), %rdi # t816 # t816
    movq -1416(%rbp), %rsi # t817 # t817
    movq -1424(%rbp), %rdx # t818 # t818
    movq -1432(%rbp), %rcx # t819 # t819
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -1440(%rbp)
    movq $255, %rax
    movq %rax, -1448(%rbp)
    movq $255, %rax
    movq %rax, -1456(%rbp)
    movq $255, %rax
    movq %rax, -1464(%rbp)
    movq $255, %rax
    movq %rax, -1472(%rbp)
    movq -1448(%rbp), %rdi # t821 # t821
    movq -1456(%rbp), %rsi # t822 # t822
    movq -1464(%rbp), %rdx # t823 # t823
    movq -1472(%rbp), %rcx # t824 # t824
    movq $0, %rax
    call sdl_settextcolor
    movq %rax, -1480(%rbp)
    movq $45, %rax
    movq %rax, -1488(%rbp)
    movq $45, %rax
    movq %rax, -1496(%rbp)
    leaq t828(%rip), %rax
    movq %rax, -1504(%rbp)
    movq $0, %rax
    movq %rax, -1512(%rbp)
    movq -192(%rbp), %rdi # score # score
    movq -1512(%rbp), %rsi # t829 # t829
    movq $0, %rax
    call mematl
    movq %rax, -1520(%rbp)
    movq -1520(%rbp), %rdi # t830 # t830
    movq $0, %rax
    call str
    movq %rax, -1528(%rbp)
    addq $22, -440(%rbp)
    movq $0, %rcx
    addq $20, -440(%rbp)
    addq $1, -440(%rbp)
    movq $1, %rsi
    xorq %rax, %rax
    movq -440(%rbp), %rdi # counter # counter
    call calloc
    movq %rax, %rdi
    movq %rdi, -1536(%rbp)
    movq -1504(%rbp), %rsi # t828 # t828
    call strcpy
    movq -1528(%rbp), %rsi # t831 # t831
    call strcat
    movq -1488(%rbp), %rdi # t826 # t826
    movq -1496(%rbp), %rsi # t827 # t827
    movq -1536(%rbp), %rdx # t832 # t832
    movq $0, %rax
    call sdl_printtext
    movq %rax, -1544(%rbp)
    movq $0, %rax
    call sdl_flip
    movq %rax, -1552(%rbp)
    jmp while_start_808
while_end_808: 
    movq -248(%rbp), %rdi # puzzle_bg # puzzle_bg
    movq $0, %rax
    call sdl_destroytex
    movq %rax, -1560(%rbp)
    movq -1344(%rbp), %rdi # game_over_screen # game_over_screen
    movq $0, %rax
    call sdl_destroytex
    movq %rax, -1568(%rbp)
    movq -96(%rbp), %rdi # grid # grid
    movq $0, %rax
    call free
    movq %rax, -1576(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq $0, %rax
    call free
    movq %rax, -1584(%rbp)
    movq -192(%rbp), %rdi # score # score
    movq $0, %rax
    call free
    movq %rax, -1592(%rbp)
    movq $0, %rax
    call sdl_release
    movq %rax, -1600(%rbp)
    movq $0, %rax
    call sdl_quit
    movq %rax, -1608(%rbp)
    movq $0, %rax
    movq %rax, -1616(%rbp)
    movq -1536(%rbp), %rdi # t832 # t832
    call free # local variable: t832
    movq -448(%rbp), %rdi # t695 # t695
    call free # local variable: t695
    movq -1616(%rbp), %rax # t842 # t842
    leave
    ret
.section .note.GNU-stack,"",@progbits

