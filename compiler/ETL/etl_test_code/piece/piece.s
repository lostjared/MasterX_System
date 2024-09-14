.section .data
t470: .asciz "MasterPiece"
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
    subq $432, %rsp
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
    movq $0, %rax
    movq %rax, -48(%rbp)
    movq $0, %rax
    movq %rax, -56(%rbp)
    movq $255, %rax
    movq %rax, -64(%rbp)
    movq -40(%rbp), %rdi # t4 # t4
    movq -48(%rbp), %rsi # t5 # t5
    movq -56(%rbp), %rdx # t6 # t6
    movq -64(%rbp), %rcx # t7 # t7
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -72(%rbp)
    movq $0, %rax
    movq %rax, -80(%rbp)
    leave
    ret
    jmp sublabel_end_3
sublabel_else_3: 
sublabel_end_3: 
    movq -8(%rbp), %rax # color # color
    movq %rax, -88(%rbp)
    movq $1, %rax
    movq %rax, -96(%rbp)
    movq -88(%rbp), %rsi # t10 # t10
    movq -96(%rbp), %rdi # t11 # t11
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -104(%rbp)
    movq -104(%rbp), %rax # t12 # t12
    cmpq $0, %rax
    je sublabel_else_13
sublabel_if_13: 
    movq $255, %rax
    movq %rax, -112(%rbp)
    movq $0, %rax
    movq %rax, -120(%rbp)
    movq $0, %rax
    movq %rax, -128(%rbp)
    movq $255, %rax
    movq %rax, -136(%rbp)
    movq -112(%rbp), %rdi # t14 # t14
    movq -120(%rbp), %rsi # t15 # t15
    movq -128(%rbp), %rdx # t16 # t16
    movq -136(%rbp), %rcx # t17 # t17
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -144(%rbp)
    movq $0, %rax
    movq %rax, -152(%rbp)
    leave
    ret
    jmp sublabel_end_13
sublabel_else_13: 
sublabel_end_13: 
    movq -8(%rbp), %rax # color # color
    movq %rax, -160(%rbp)
    movq $2, %rax
    movq %rax, -168(%rbp)
    movq -160(%rbp), %rsi # t20 # t20
    movq -168(%rbp), %rdi # t21 # t21
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -176(%rbp)
    movq -176(%rbp), %rax # t22 # t22
    cmpq $0, %rax
    je sublabel_else_23
sublabel_if_23: 
    movq $0, %rax
    movq %rax, -184(%rbp)
    movq $255, %rax
    movq %rax, -192(%rbp)
    movq $0, %rax
    movq %rax, -200(%rbp)
    movq $255, %rax
    movq %rax, -208(%rbp)
    movq -184(%rbp), %rdi # t24 # t24
    movq -192(%rbp), %rsi # t25 # t25
    movq -200(%rbp), %rdx # t26 # t26
    movq -208(%rbp), %rcx # t27 # t27
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -216(%rbp)
    movq $0, %rax
    movq %rax, -224(%rbp)
    leave
    ret
    jmp sublabel_end_23
sublabel_else_23: 
sublabel_end_23: 
    movq -8(%rbp), %rax # color # color
    movq %rax, -232(%rbp)
    movq $3, %rax
    movq %rax, -240(%rbp)
    movq -232(%rbp), %rsi # t30 # t30
    movq -240(%rbp), %rdi # t31 # t31
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -248(%rbp)
    movq -248(%rbp), %rax # t32 # t32
    cmpq $0, %rax
    je sublabel_else_33
sublabel_if_33: 
    movq $0, %rax
    movq %rax, -256(%rbp)
    movq $0, %rax
    movq %rax, -264(%rbp)
    movq $255, %rax
    movq %rax, -272(%rbp)
    movq $255, %rax
    movq %rax, -280(%rbp)
    movq -256(%rbp), %rdi # t34 # t34
    movq -264(%rbp), %rsi # t35 # t35
    movq -272(%rbp), %rdx # t36 # t36
    movq -280(%rbp), %rcx # t37 # t37
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -288(%rbp)
    movq $0, %rax
    movq %rax, -296(%rbp)
    leave
    ret
    jmp sublabel_end_33
sublabel_else_33: 
sublabel_end_33: 
    movq -8(%rbp), %rax # color # color
    movq %rax, -304(%rbp)
    movq $4, %rax
    movq %rax, -312(%rbp)
    movq -304(%rbp), %rsi # t40 # t40
    movq -312(%rbp), %rdi # t41 # t41
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -320(%rbp)
    movq -320(%rbp), %rax # t42 # t42
    cmpq $0, %rax
    je sublabel_else_43
sublabel_if_43: 
    movq $255, %rax
    movq %rax, -328(%rbp)
    movq $0, %rax
    movq %rax, -336(%rbp)
    movq $255, %rax
    movq %rax, -344(%rbp)
    movq $25, %rax
    movq %rax, -352(%rbp)
    movq -328(%rbp), %rdi # t44 # t44
    movq -336(%rbp), %rsi # t45 # t45
    movq -344(%rbp), %rdx # t46 # t46
    movq -352(%rbp), %rcx # t47 # t47
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -360(%rbp)
    movq $0, %rax
    movq %rax, -368(%rbp)
    leave
    ret
    jmp sublabel_end_43
sublabel_else_43: 
sublabel_end_43: 
    movq $0, %rax
    movq %rax, -376(%rbp)
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
    movq -32(%rbp), %rax # t52 # t52
    addq -40(%rbp), %rax
    movq %rax, -48(%rbp)
    movq -24(%rbp), %rdi # t51 # t51
    movq -48(%rbp), %rsi # t54 # t54
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
    movq -80(%rbp), %rax # t57 # t57
    addq -88(%rbp), %rax
    movq %rax, -96(%rbp)
    movq -72(%rbp), %rdi # t56 # t56
    movq -96(%rbp), %rsi # t59 # t59
    movq $0, %rax
    call mematb
    movq %rax, -104(%rbp)
    movq %rax, -112(%rbp)
    movq $32, %rax
    movq %rax, -120(%rbp)
    movq $4, %rax
    movq %rax, -128(%rbp)
    movq -120(%rbp), %rax # t61 # t61
    imulq -128(%rbp), %rax
    movq %rax, -136(%rbp)
    movq %rax, -144(%rbp)
    movq $16, %rax
    movq %rax, -152(%rbp)
    movq $4, %rax
    movq %rax, -160(%rbp)
    movq -152(%rbp), %rax # t64 # t64
    imulq -160(%rbp), %rax
    movq %rax, -168(%rbp)
    movq %rax, -176(%rbp)
    movq -8(%rbp), %rax # block # block
    movq %rax, -184(%rbp)
    movq -16(%rbp), %rax # index # index
    movq %rax, -192(%rbp)
    movq -184(%rbp), %rdi # t67 # t67
    movq -192(%rbp), %rsi # t68 # t68
    movq $0, %rax
    call mematb
    movq %rax, -200(%rbp)
    movq -200(%rbp), %rdi # t69 # t69
    movq $0, %rax
    call set_block_color
    movq %rax, -208(%rbp)
    movq -144(%rbp), %rax # x # x
    imulq -64(%rbp), %rax
    movq %rax, -216(%rbp)
    movq $20, %rax
    movq %rax, -224(%rbp)
    movq -216(%rbp), %rax # t71 # t71
    addq -224(%rbp), %rax
    movq %rax, -232(%rbp)
    movq -176(%rbp), %rax # y # y
    imulq -112(%rbp), %rax
    movq %rax, -240(%rbp)
    movq $30, %rax
    movq %rax, -248(%rbp)
    movq -240(%rbp), %rax # t74 # t74
    addq -248(%rbp), %rax
    movq %rax, -256(%rbp)
    movq $32, %rax
    movq %rax, -264(%rbp)
    movq $4, %rax
    movq %rax, -272(%rbp)
    movq -264(%rbp), %rax # t77 # t77
    imulq -272(%rbp), %rax
    movq %rax, -280(%rbp)
    movq $16, %rax
    movq %rax, -288(%rbp)
    movq $4, %rax
    movq %rax, -296(%rbp)
    movq -288(%rbp), %rax # t80 # t80
    imulq -296(%rbp), %rax
    movq %rax, -304(%rbp)
    movq -232(%rbp), %rdi # t73 # t73
    movq -256(%rbp), %rsi # t76 # t76
    movq -280(%rbp), %rdx # t79 # t79
    movq -304(%rbp), %rcx # t82 # t82
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
    subq $512, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq $0, %rax
    movq %rax, -24(%rbp)
    movq $0, %rax
    movq %rax, -32(%rbp)
    movq $0, %rax
    movq %rax, -40(%rbp)
    movq $255, %rax
    movq %rax, -48(%rbp)
    movq -24(%rbp), %rdi # t85 # t85
    movq -32(%rbp), %rsi # t86 # t86
    movq -40(%rbp), %rdx # t87 # t87
    movq -48(%rbp), %rcx # t88 # t88
    movq $0, %rax
    call sdl_setcolor
    movq %rax, -56(%rbp)
    movq $0, %rax
    movq %rax, -64(%rbp)
    movq $0, %rax
    movq %rax, -72(%rbp)
    movq $1440, %rax
    movq %rax, -80(%rbp)
    movq $1080, %rax
    movq %rax, -88(%rbp)
    movq -64(%rbp), %rdi # t90 # t90
    movq -72(%rbp), %rsi # t91 # t91
    movq -80(%rbp), %rdx # t92 # t92
    movq -88(%rbp), %rcx # t93 # t93
    movq $0, %rax
    call sdl_fillrect
    movq %rax, -96(%rbp)
    movq $32, %rax
    movq %rax, -104(%rbp)
    movq $4, %rax
    movq %rax, -112(%rbp)
    movq -104(%rbp), %rax # t95 # t95
    imulq -112(%rbp), %rax
    movq %rax, -120(%rbp)
    movq %rax, -128(%rbp)
    movq $16, %rax
    movq %rax, -136(%rbp)
    movq $4, %rax
    movq %rax, -144(%rbp)
    movq -136(%rbp), %rax # t98 # t98
    imulq -144(%rbp), %rax
    movq %rax, -152(%rbp)
    movq %rax, -160(%rbp)
    movq $20, %rax
    movq %rax, -168(%rbp)
    movq $30, %rax
    movq %rax, -176(%rbp)
    movq $0, %rax
    movq %rax, -184(%rbp)
for_start_101: 
    movq $11, %rax
    movq %rax, -192(%rbp)
    movq -184(%rbp), %rax # x # x
    movq -192(%rbp), %rdi # t103 # t103
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -200(%rbp)
    movq -200(%rbp), %rax # t104 # t104
    cmpq $0, %rax
    je for_end_101
    movq $0, %rax
    movq %rax, -208(%rbp)
for_start_105: 
    movq $16, %rax
    movq %rax, -216(%rbp)
    movq -208(%rbp), %rax # y # y
    movq -216(%rbp), %rdi # t107 # t107
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -224(%rbp)
    movq -224(%rbp), %rax # t108 # t108
    cmpq $0, %rax
    je for_end_105
    movq -184(%rbp), %rax # x # x
    imulq -128(%rbp), %rax
    movq %rax, -232(%rbp)
    movq -168(%rbp), %rax # offset_left # offset_left
    movq %rax, -240(%rbp)
    movq -232(%rbp), %rax # t109 # t109
    addq -240(%rbp), %rax
    movq %rax, -248(%rbp)
    movq %rax, -256(%rbp)
    movq -208(%rbp), %rax # y # y
    imulq -160(%rbp), %rax
    movq %rax, -264(%rbp)
    movq -176(%rbp), %rax # offset_top # offset_top
    movq %rax, -272(%rbp)
    movq -264(%rbp), %rax # t112 # t112
    addq -272(%rbp), %rax
    movq %rax, -280(%rbp)
    movq %rax, -288(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -296(%rbp)
    movq $16, %rax
    movq %rax, -304(%rbp)
    movq -184(%rbp), %rax # x # x
    imulq -304(%rbp), %rax
    movq %rax, -312(%rbp)
    addq -208(%rbp), %rax
    movq %rax, -320(%rbp)
    movq -296(%rbp), %rdi # t115 # t115
    movq -320(%rbp), %rsi # t118 # t118
    movq $0, %rax
    call mematb
    movq %rax, -328(%rbp)
    movq %rax, -336(%rbp)
    movq -336(%rbp), %rdi # color # color
    movq $0, %rax
    call set_block_color
    movq %rax, -344(%rbp)
    movq -256(%rbp), %rdi # grid_x # grid_x
    movq -288(%rbp), %rsi # grid_y # grid_y
    movq -128(%rbp), %rdx # block_size_w # block_size_w
    movq -160(%rbp), %rcx # block_size_h # block_size_h
    movq $0, %rax
    call sdl_fillrect
    movq %rax, -352(%rbp)
for_post_106: 
    movq $1, %rax
    movq %rax, -360(%rbp)
    movq -208(%rbp), %rax # y # y
    addq -360(%rbp), %rax
    movq %rax, -368(%rbp)
# load t123 to y
    movq -368(%rbp), %rcx
    movq %rcx, -208(%rbp)
    jmp for_start_105
for_end_105: 
for_post_102: 
    movq $1, %rax
    movq %rax, -376(%rbp)
    movq -184(%rbp), %rax # x # x
    addq -376(%rbp), %rax
    movq %rax, -384(%rbp)
# load t125 to x
    movq -384(%rbp), %rcx
    movq %rcx, -184(%rbp)
    jmp for_start_101
for_end_101: 
    movq -16(%rbp), %rax # block # block
    movq %rax, -392(%rbp)
    movq $0, %rax
    movq %rax, -400(%rbp)
    movq -392(%rbp), %rdi # t126 # t126
    movq -400(%rbp), %rsi # t127 # t127
    movq $0, %rax
    call set_block_at
    movq %rax, -408(%rbp)
    movq -16(%rbp), %rax # block # block
    movq %rax, -416(%rbp)
    movq $3, %rax
    movq %rax, -424(%rbp)
    movq -416(%rbp), %rdi # t129 # t129
    movq -424(%rbp), %rsi # t130 # t130
    movq $0, %rax
    call set_block_at
    movq %rax, -432(%rbp)
    movq -16(%rbp), %rax # block # block
    movq %rax, -440(%rbp)
    movq $6, %rax
    movq %rax, -448(%rbp)
    movq -440(%rbp), %rdi # t132 # t132
    movq -448(%rbp), %rsi # t133 # t133
    movq $0, %rax
    call set_block_at
    movq %rax, -456(%rbp)
    movq $0, %rax
    movq %rax, -464(%rbp)
    leave
    ret
.globl check_blocks
check_blocks:
    pushq %rbp
    movq %rsp, %rbp
    subq $1216, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq $0, %rax
    movq %rax, -16(%rbp)
    movq $0, %rax
    movq %rax, -24(%rbp)
    movq $0, %rax
    movq %rax, -32(%rbp)
for_start_136: 
    movq $11, %rax
    movq %rax, -40(%rbp)
    movq -32(%rbp), %rax # x # x
    movq -40(%rbp), %rdi # t138 # t138
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -48(%rbp)
    movq -48(%rbp), %rax # t139 # t139
    cmpq $0, %rax
    je for_end_136
    movq $0, %rax
    movq %rax, -56(%rbp)
for_start_140: 
    movq $14, %rax
    movq %rax, -64(%rbp)
    movq -56(%rbp), %rax # y # y
    movq -64(%rbp), %rdi # t142 # t142
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -72(%rbp)
    movq -72(%rbp), %rax # t143 # t143
    cmpq $0, %rax
    je for_end_140
# load x to bx
    movq -32(%rbp), %rcx
    movq %rcx, -16(%rbp)
# load y to by
    movq -56(%rbp), %rcx
    movq %rcx, -24(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -80(%rbp)
    movq $16, %rax
    movq %rax, -88(%rbp)
    movq -16(%rbp), %rax # bx # bx
    imulq -88(%rbp), %rax
    movq %rax, -96(%rbp)
    addq -24(%rbp), %rax
    movq %rax, -104(%rbp)
    movq -80(%rbp), %rdi # t144 # t144
    movq -104(%rbp), %rsi # t147 # t147
    movq $0, %rax
    call mematb
    movq %rax, -112(%rbp)
    movq %rax, -120(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -128(%rbp)
    movq $16, %rax
    movq %rax, -136(%rbp)
    movq -16(%rbp), %rax # bx # bx
    imulq -136(%rbp), %rax
    movq %rax, -144(%rbp)
    movq $1, %rax
    movq %rax, -152(%rbp)
    movq -24(%rbp), %rax # by # by
    addq -152(%rbp), %rax
    movq %rax, -160(%rbp)
    movq -144(%rbp), %rax # t151 # t151
    addq -160(%rbp), %rax
    movq %rax, -168(%rbp)
    movq -128(%rbp), %rdi # t149 # t149
    movq -168(%rbp), %rsi # t154 # t154
    movq $0, %rax
    call mematb
    movq %rax, -176(%rbp)
    movq %rax, -184(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -192(%rbp)
    movq $16, %rax
    movq %rax, -200(%rbp)
    movq -16(%rbp), %rax # bx # bx
    imulq -200(%rbp), %rax
    movq %rax, -208(%rbp)
    movq $2, %rax
    movq %rax, -216(%rbp)
    movq -24(%rbp), %rax # by # by
    addq -216(%rbp), %rax
    movq %rax, -224(%rbp)
    movq -208(%rbp), %rax # t158 # t158
    addq -224(%rbp), %rax
    movq %rax, -232(%rbp)
    movq -192(%rbp), %rdi # t156 # t156
    movq -232(%rbp), %rsi # t161 # t161
    movq $0, %rax
    call mematb
    movq %rax, -240(%rbp)
    movq %rax, -248(%rbp)
    movq $0, %rax
    movq %rax, -256(%rbp)
    movq -120(%rbp), %rax # color1 # color1
    movq -256(%rbp), %rdi # t163 # t163
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -264(%rbp)
    movq $0, %rax
    movq %rax, -272(%rbp)
    movq -184(%rbp), %rax # color2 # color2
    movq -272(%rbp), %rdi # t165 # t165
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -280(%rbp)
    movq -264(%rbp), %rsi # t164 # t164
    cmpq $0, %rsi
    setne %al
    movq -280(%rbp), %rdi # t166 # t166
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -288(%rbp)
    movq $0, %rax
    movq %rax, -296(%rbp)
    movq -248(%rbp), %rax # color3 # color3
    movq -296(%rbp), %rdi # t168 # t168
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -304(%rbp)
    movq -288(%rbp), %rsi # t167 # t167
    cmpq $0, %rsi
    setne %al
    movq -304(%rbp), %rdi # t169 # t169
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -312(%rbp)
    movq -120(%rbp), %rsi # color1 # color1
    movq -184(%rbp), %rdi # color2 # color2
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -320(%rbp)
    movq -312(%rbp), %rsi # t170 # t170
    cmpq $0, %rsi
    setne %al
    movq -320(%rbp), %rdi # t171 # t171
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -328(%rbp)
    movq -120(%rbp), %rsi # color1 # color1
    movq -248(%rbp), %rdi # color3 # color3
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -336(%rbp)
    movq -328(%rbp), %rsi # t172 # t172
    cmpq $0, %rsi
    setne %al
    movq -336(%rbp), %rdi # t173 # t173
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -344(%rbp)
    cmpq $0, %rax
    je sublabel_else_175
sublabel_if_175: 
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -352(%rbp)
    movq $16, %rax
    movq %rax, -360(%rbp)
    movq -16(%rbp), %rax # bx # bx
    imulq -360(%rbp), %rax
    movq %rax, -368(%rbp)
    addq -24(%rbp), %rax
    movq %rax, -376(%rbp)
    movq $0, %rax
    movq %rax, -384(%rbp)
    movq -352(%rbp), %rdi # t176 # t176
    movq -376(%rbp), %rsi # t179 # t179
    movq -384(%rbp), %rdx # t180 # t180
    movq $0, %rax
    call memstoreb
    movq %rax, -392(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -400(%rbp)
    movq $16, %rax
    movq %rax, -408(%rbp)
    movq -16(%rbp), %rax # bx # bx
    imulq -408(%rbp), %rax
    movq %rax, -416(%rbp)
    movq $1, %rax
    movq %rax, -424(%rbp)
    movq -24(%rbp), %rax # by # by
    addq -424(%rbp), %rax
    movq %rax, -432(%rbp)
    movq -416(%rbp), %rax # t184 # t184
    addq -432(%rbp), %rax
    movq %rax, -440(%rbp)
    movq $0, %rax
    movq %rax, -448(%rbp)
    movq -400(%rbp), %rdi # t182 # t182
    movq -440(%rbp), %rsi # t187 # t187
    movq -448(%rbp), %rdx # t188 # t188
    movq $0, %rax
    call memstoreb
    movq %rax, -456(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -464(%rbp)
    movq $16, %rax
    movq %rax, -472(%rbp)
    movq -16(%rbp), %rax # bx # bx
    imulq -472(%rbp), %rax
    movq %rax, -480(%rbp)
    movq $2, %rax
    movq %rax, -488(%rbp)
    movq -24(%rbp), %rax # by # by
    addq -488(%rbp), %rax
    movq %rax, -496(%rbp)
    movq -480(%rbp), %rax # t192 # t192
    addq -496(%rbp), %rax
    movq %rax, -504(%rbp)
    movq $0, %rax
    movq %rax, -512(%rbp)
    movq -464(%rbp), %rdi # t190 # t190
    movq -504(%rbp), %rsi # t195 # t195
    movq -512(%rbp), %rdx # t196 # t196
    movq $0, %rax
    call memstoreb
    movq %rax, -520(%rbp)
    movq $0, %rax
    movq %rax, -528(%rbp)
    leave
    ret
    jmp sublabel_end_175
sublabel_else_175: 
sublabel_end_175: 
for_post_141: 
    movq $1, %rax
    movq %rax, -536(%rbp)
    movq -56(%rbp), %rax # y # y
    addq -536(%rbp), %rax
    movq %rax, -544(%rbp)
# load t200 to y
    movq -544(%rbp), %rcx
    movq %rcx, -56(%rbp)
    jmp for_start_140
for_end_140: 
for_post_137: 
    movq $1, %rax
    movq %rax, -552(%rbp)
    movq -32(%rbp), %rax # x # x
    addq -552(%rbp), %rax
    movq %rax, -560(%rbp)
# load t202 to x
    movq -560(%rbp), %rcx
    movq %rcx, -32(%rbp)
    jmp for_start_136
for_end_136: 
    movq $0, %rax
    movq %rax, -568(%rbp)
for_start_203: 
    movq -568(%rbp), %rax # y1 # y1
    movq %rax, -576(%rbp)
    movq $16, %rax
    movq %rax, -584(%rbp)
    movq -576(%rbp), %rax # t205 # t205
    movq -584(%rbp), %rdi # t206 # t206
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -592(%rbp)
    movq -592(%rbp), %rax # t207 # t207
    cmpq $0, %rax
    je for_end_203
    movq $0, %rax
    movq %rax, -600(%rbp)
for_start_208: 
    movq -600(%rbp), %rax # x1 # x1
    movq %rax, -608(%rbp)
    movq $9, %rax
    movq %rax, -616(%rbp)
    movq -608(%rbp), %rax # t210 # t210
    movq -616(%rbp), %rdi # t211 # t211
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -624(%rbp)
    movq -624(%rbp), %rax # t212 # t212
    cmpq $0, %rax
    je for_end_208
    movq -600(%rbp), %rax # x1 # x1
    movq %rax, -632(%rbp)
# load t213 to bx
    movq -632(%rbp), %rcx
    movq %rcx, -16(%rbp)
    movq -568(%rbp), %rax # y1 # y1
    movq %rax, -640(%rbp)
# load t214 to by
    movq -640(%rbp), %rcx
    movq %rcx, -24(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -648(%rbp)
    movq $16, %rax
    movq %rax, -656(%rbp)
    movq -16(%rbp), %rax # bx # bx
    imulq -656(%rbp), %rax
    movq %rax, -664(%rbp)
    addq -24(%rbp), %rax
    movq %rax, -672(%rbp)
    movq -648(%rbp), %rdi # t215 # t215
    movq -672(%rbp), %rsi # t218 # t218
    movq $0, %rax
    call mematb
    movq %rax, -680(%rbp)
    movq %rax, -688(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -696(%rbp)
    movq $1, %rax
    movq %rax, -704(%rbp)
    movq -16(%rbp), %rax # bx # bx
    addq -704(%rbp), %rax
    movq %rax, -712(%rbp)
    movq $16, %rax
    movq %rax, -720(%rbp)
    movq -712(%rbp), %rax # t222 # t222
    imulq -720(%rbp), %rax
    movq %rax, -728(%rbp)
    addq -24(%rbp), %rax
    movq %rax, -736(%rbp)
    movq -696(%rbp), %rdi # t220 # t220
    movq -736(%rbp), %rsi # t225 # t225
    movq $0, %rax
    call mematb
    movq %rax, -744(%rbp)
    movq %rax, -752(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -760(%rbp)
    movq $2, %rax
    movq %rax, -768(%rbp)
    movq -16(%rbp), %rax # bx # bx
    addq -768(%rbp), %rax
    movq %rax, -776(%rbp)
    movq $16, %rax
    movq %rax, -784(%rbp)
    movq -776(%rbp), %rax # t229 # t229
    imulq -784(%rbp), %rax
    movq %rax, -792(%rbp)
    addq -24(%rbp), %rax
    movq %rax, -800(%rbp)
    movq -760(%rbp), %rdi # t227 # t227
    movq -800(%rbp), %rsi # t232 # t232
    movq $0, %rax
    call mematb
    movq %rax, -808(%rbp)
    movq %rax, -816(%rbp)
    movq $0, %rax
    movq %rax, -824(%rbp)
    movq -688(%rbp), %rax # color1x # color1x
    movq -824(%rbp), %rdi # t234 # t234
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -832(%rbp)
    movq $0, %rax
    movq %rax, -840(%rbp)
    movq -752(%rbp), %rax # color2x # color2x
    movq -840(%rbp), %rdi # t236 # t236
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -848(%rbp)
    movq -832(%rbp), %rsi # t235 # t235
    cmpq $0, %rsi
    setne %al
    movq -848(%rbp), %rdi # t237 # t237
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -856(%rbp)
    movq $0, %rax
    movq %rax, -864(%rbp)
    movq -816(%rbp), %rax # color3x # color3x
    movq -864(%rbp), %rdi # t239 # t239
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -872(%rbp)
    movq -856(%rbp), %rsi # t238 # t238
    cmpq $0, %rsi
    setne %al
    movq -872(%rbp), %rdi # t240 # t240
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -880(%rbp)
    movq -688(%rbp), %rsi # color1x # color1x
    movq -752(%rbp), %rdi # color2x # color2x
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -888(%rbp)
    movq -880(%rbp), %rsi # t241 # t241
    cmpq $0, %rsi
    setne %al
    movq -888(%rbp), %rdi # t242 # t242
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -896(%rbp)
    movq -688(%rbp), %rsi # color1x # color1x
    movq -816(%rbp), %rdi # color3x # color3x
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -904(%rbp)
    movq -896(%rbp), %rsi # t243 # t243
    cmpq $0, %rsi
    setne %al
    movq -904(%rbp), %rdi # t244 # t244
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -912(%rbp)
    cmpq $0, %rax
    je sublabel_else_246
sublabel_if_246: 
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -920(%rbp)
    movq $16, %rax
    movq %rax, -928(%rbp)
    movq -16(%rbp), %rax # bx # bx
    imulq -928(%rbp), %rax
    movq %rax, -936(%rbp)
    addq -24(%rbp), %rax
    movq %rax, -944(%rbp)
    movq $0, %rax
    movq %rax, -952(%rbp)
    movq -920(%rbp), %rdi # t247 # t247
    movq -944(%rbp), %rsi # t250 # t250
    movq -952(%rbp), %rdx # t251 # t251
    movq $0, %rax
    call memstoreb
    movq %rax, -960(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -968(%rbp)
    movq $1, %rax
    movq %rax, -976(%rbp)
    movq -16(%rbp), %rax # bx # bx
    addq -976(%rbp), %rax
    movq %rax, -984(%rbp)
    movq $16, %rax
    movq %rax, -992(%rbp)
    movq -984(%rbp), %rax # t255 # t255
    imulq -992(%rbp), %rax
    movq %rax, -1000(%rbp)
    addq -24(%rbp), %rax
    movq %rax, -1008(%rbp)
    movq $0, %rax
    movq %rax, -1016(%rbp)
    movq -968(%rbp), %rdi # t253 # t253
    movq -1008(%rbp), %rsi # t258 # t258
    movq -1016(%rbp), %rdx # t259 # t259
    movq $0, %rax
    call memstoreb
    movq %rax, -1024(%rbp)
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -1032(%rbp)
    movq $2, %rax
    movq %rax, -1040(%rbp)
    movq -16(%rbp), %rax # bx # bx
    addq -1040(%rbp), %rax
    movq %rax, -1048(%rbp)
    movq $16, %rax
    movq %rax, -1056(%rbp)
    movq -1048(%rbp), %rax # t263 # t263
    imulq -1056(%rbp), %rax
    movq %rax, -1064(%rbp)
    addq -24(%rbp), %rax
    movq %rax, -1072(%rbp)
    movq $0, %rax
    movq %rax, -1080(%rbp)
    movq -1032(%rbp), %rdi # t261 # t261
    movq -1072(%rbp), %rsi # t266 # t266
    movq -1080(%rbp), %rdx # t267 # t267
    movq $0, %rax
    call memstoreb
    movq %rax, -1088(%rbp)
    movq $0, %rax
    movq %rax, -1096(%rbp)
    leave
    ret
    jmp sublabel_end_246
sublabel_else_246: 
sublabel_end_246: 
for_post_209: 
    movq -600(%rbp), %rax # x1 # x1
    movq %rax, -1104(%rbp)
    movq $1, %rax
    movq %rax, -1112(%rbp)
    movq -1104(%rbp), %rax # t270 # t270
    addq -1112(%rbp), %rax
    movq %rax, -1120(%rbp)
# load t272 to x1
    movq -1120(%rbp), %rcx
    movq %rcx, -600(%rbp)
    jmp for_start_208
for_end_208: 
for_post_204: 
    movq -568(%rbp), %rax # y1 # y1
    movq %rax, -1128(%rbp)
    movq $1, %rax
    movq %rax, -1136(%rbp)
    movq -1128(%rbp), %rax # t273 # t273
    addq -1136(%rbp), %rax
    movq %rax, -1144(%rbp)
# load t275 to y1
    movq -1144(%rbp), %rcx
    movq %rcx, -568(%rbp)
    jmp for_start_203
for_end_203: 
    movq $0, %rax
    movq %rax, -1152(%rbp)
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
for_start_277: 
    movq $11, %rax
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rax # x # x
    movq -24(%rbp), %rdi # t279 # t279
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -32(%rbp)
    movq -32(%rbp), %rax # t280 # t280
    cmpq $0, %rax
    je for_end_277
    movq $0, %rax
    movq %rax, -40(%rbp)
for_start_281: 
    movq $15, %rax
    movq %rax, -48(%rbp)
    movq -40(%rbp), %rax # y # y
    movq -48(%rbp), %rdi # t283 # t283
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -56(%rbp)
    movq -56(%rbp), %rax # t284 # t284
    cmpq $0, %rax
    je for_end_281
    movq -8(%rbp), %rax # grid # grid
    movq %rax, -64(%rbp)
    movq $16, %rax
    movq %rax, -72(%rbp)
    movq -16(%rbp), %rax # x # x
    imulq -72(%rbp), %rax
    movq %rax, -80(%rbp)
    addq -40(%rbp), %rax
    movq %rax, -88(%rbp)
    movq -64(%rbp), %rdi # t285 # t285
    movq -88(%rbp), %rsi # t288 # t288
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
    movq -128(%rbp), %rax # t292 # t292
    addq -144(%rbp), %rax
    movq %rax, -152(%rbp)
    movq -112(%rbp), %rdi # t290 # t290
    movq -152(%rbp), %rsi # t295 # t295
    movq $0, %rax
    call mematb
    movq %rax, -160(%rbp)
    movq %rax, -168(%rbp)
    movq $0, %rax
    movq %rax, -176(%rbp)
    movq -168(%rbp), %rsi # color2 # color2
    movq -176(%rbp), %rdi # t297 # t297
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -184(%rbp)
    movq $0, %rax
    movq %rax, -192(%rbp)
    movq -104(%rbp), %rax # color1 # color1
    movq -192(%rbp), %rdi # t299 # t299
    cmpq %rdi, %rax
    setne %cl
    movzbq %cl, %rdx
    movq %rdx, -200(%rbp)
    movq -184(%rbp), %rsi # t298 # t298
    cmpq $0, %rsi
    setne %al
    movq -200(%rbp), %rdi # t300 # t300
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -208(%rbp)
    cmpq $0, %rax
    je sublabel_else_302
sublabel_if_302: 
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
    movq -216(%rbp), %rdi # t303 # t303
    movq -240(%rbp), %rsi # t306 # t306
    movq -248(%rbp), %rdx # t307 # t307
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
    movq -280(%rbp), %rax # t311 # t311
    addq -296(%rbp), %rax
    movq %rax, -304(%rbp)
    movq -264(%rbp), %rdi # t309 # t309
    movq -304(%rbp), %rsi # t314 # t314
    movq -104(%rbp), %rdx # color1 # color1
    movq $0, %rax
    call memstoreb
    movq %rax, -312(%rbp)
    movq $0, %rax
    movq %rax, -320(%rbp)
    leave
    ret
    jmp sublabel_end_302
sublabel_else_302: 
sublabel_end_302: 
for_post_282: 
    movq $1, %rax
    movq %rax, -328(%rbp)
    movq -40(%rbp), %rax # y # y
    addq -328(%rbp), %rax
    movq %rax, -336(%rbp)
# load t318 to y
    movq -336(%rbp), %rcx
    movq %rcx, -40(%rbp)
    jmp for_start_281
for_end_281: 
for_post_278: 
    movq $1, %rax
    movq %rax, -344(%rbp)
    movq -16(%rbp), %rax # x # x
    addq -344(%rbp), %rax
    movq %rax, -352(%rbp)
# load t320 to x
    movq -352(%rbp), %rcx
    movq %rcx, -16(%rbp)
    jmp for_start_277
for_end_277: 
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
    movq -24(%rbp), %rax # t322 # t322
    imulq -32(%rbp), %rax
    movq %rax, -40(%rbp)
    movq -40(%rbp), %rdi # t324 # t324
    movq $0, %rax
    call malloc
    movq %rax, -48(%rbp)
    movq %rax, -56(%rbp)
    movq -8(%rbp), %rax # w # w
    movq %rax, -64(%rbp)
    movq -16(%rbp), %rax # h # h
    movq %rax, -72(%rbp)
    movq -64(%rbp), %rax # t326 # t326
    imulq -72(%rbp), %rax
    movq %rax, -80(%rbp)
    movq -56(%rbp), %rdi # grid # grid
    movq -80(%rbp), %rsi # t328 # t328
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
    movq -8(%rbp), %rdi # t330 # t330
    movq $0, %rax
    call malloc
    movq %rax, -16(%rbp)
    movq %rax, -24(%rbp)
    movq $10, %rax
    movq %rax, -32(%rbp)
    movq -24(%rbp), %rdi # block # block
    movq -32(%rbp), %rsi # t332 # t332
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
    movq -32(%rbp), %rax # t336 # t336
    cqto
    movq -40(%rbp), %rdi # t337 # t337
    idivq %rdi
    movq %rdx, -48(%rbp)
    movq -24(%rbp), %rax # t335 # t335
    addq -48(%rbp), %rax
    movq %rax, -56(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -16(%rbp), %rsi # t334 # t334
    movq -56(%rbp), %rdx # t339 # t339
    movq $0, %rax
    call memstoreb
    movq %rax, -64(%rbp)
    movq $1, %rax
    movq %rax, -72(%rbp)
    movq $11, %rax
    movq %rax, -80(%rbp)
    movq $2, %rax
    movq %rax, -88(%rbp)
    movq -80(%rbp), %rax # t342 # t342
    cqto
    idivq -88(%rbp)
    movq %rax, -96(%rbp)
    movq $1, %rax
    movq %rax, -104(%rbp)
    movq -96(%rbp), %rax # t344 # t344
    subq -104(%rbp), %rax
    movq %rax, -112(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -72(%rbp), %rsi # t341 # t341
    movq -112(%rbp), %rdx # t346 # t346
    movq $0, %rax
    call memstoreb
    movq %rax, -120(%rbp)
    movq $2, %rax
    movq %rax, -128(%rbp)
    movq $0, %rax
    movq %rax, -136(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -128(%rbp), %rsi # t348 # t348
    movq -136(%rbp), %rdx # t349 # t349
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
    movq -168(%rbp), %rax # t353 # t353
    cqto
    movq -176(%rbp), %rdi # t354 # t354
    idivq %rdi
    movq %rdx, -184(%rbp)
    movq -160(%rbp), %rax # t352 # t352
    addq -184(%rbp), %rax
    movq %rax, -192(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -152(%rbp), %rsi # t351 # t351
    movq -192(%rbp), %rdx # t356 # t356
    movq $0, %rax
    call memstoreb
    movq %rax, -200(%rbp)
    movq $4, %rax
    movq %rax, -208(%rbp)
    movq $11, %rax
    movq %rax, -216(%rbp)
    movq $2, %rax
    movq %rax, -224(%rbp)
    movq -216(%rbp), %rax # t359 # t359
    cqto
    idivq -224(%rbp)
    movq %rax, -232(%rbp)
    movq $1, %rax
    movq %rax, -240(%rbp)
    movq -232(%rbp), %rax # t361 # t361
    subq -240(%rbp), %rax
    movq %rax, -248(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -208(%rbp), %rsi # t358 # t358
    movq -248(%rbp), %rdx # t363 # t363
    movq $0, %rax
    call memstoreb
    movq %rax, -256(%rbp)
    movq $5, %rax
    movq %rax, -264(%rbp)
    movq $1, %rax
    movq %rax, -272(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -264(%rbp), %rsi # t365 # t365
    movq -272(%rbp), %rdx # t366 # t366
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
    movq -304(%rbp), %rax # t370 # t370
    cqto
    movq -312(%rbp), %rdi # t371 # t371
    idivq %rdi
    movq %rdx, -320(%rbp)
    movq -296(%rbp), %rax # t369 # t369
    addq -320(%rbp), %rax
    movq %rax, -328(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -288(%rbp), %rsi # t368 # t368
    movq -328(%rbp), %rdx # t373 # t373
    movq $0, %rax
    call memstoreb
    movq %rax, -336(%rbp)
    movq $7, %rax
    movq %rax, -344(%rbp)
    movq $11, %rax
    movq %rax, -352(%rbp)
    movq $2, %rax
    movq %rax, -360(%rbp)
    movq -352(%rbp), %rax # t376 # t376
    cqto
    idivq -360(%rbp)
    movq %rax, -368(%rbp)
    movq $1, %rax
    movq %rax, -376(%rbp)
    movq -368(%rbp), %rax # t378 # t378
    subq -376(%rbp), %rax
    movq %rax, -384(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -344(%rbp), %rsi # t375 # t375
    movq -384(%rbp), %rdx # t380 # t380
    movq $0, %rax
    call memstoreb
    movq %rax, -392(%rbp)
    movq $8, %rax
    movq %rax, -400(%rbp)
    movq $2, %rax
    movq %rax, -408(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -400(%rbp), %rsi # t382 # t382
    movq -408(%rbp), %rdx # t383 # t383
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
    subq $336, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq $0, %rax
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -24(%rbp), %rsi # t386 # t386
    movq $0, %rax
    call mematb
    movq %rax, -32(%rbp)
    movq %rax, -40(%rbp)
    movq $1, %rax
    movq %rax, -48(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -48(%rbp), %rsi # t388 # t388
    movq $0, %rax
    call mematb
    movq %rax, -56(%rbp)
    movq %rax, -64(%rbp)
    movq $2, %rax
    movq %rax, -72(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -72(%rbp), %rsi # t390 # t390
    movq $0, %rax
    call mematb
    movq %rax, -80(%rbp)
    movq %rax, -88(%rbp)
    movq $16, %rax
    movq %rax, -96(%rbp)
    movq -64(%rbp), %rax # bx # bx
    imulq -96(%rbp), %rax
    movq %rax, -104(%rbp)
    addq -88(%rbp), %rax
    movq %rax, -112(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -112(%rbp), %rsi # t394 # t394
    movq -40(%rbp), %rdx # val # val
    movq $0, %rax
    call memstoreb
    movq %rax, -120(%rbp)
    movq $3, %rax
    movq %rax, -128(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -128(%rbp), %rsi # t396 # t396
    movq $0, %rax
    call mematb
    movq %rax, -136(%rbp)
# load t397 to val
    movq -136(%rbp), %rcx
    movq %rcx, -40(%rbp)
    movq $4, %rax
    movq %rax, -144(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -144(%rbp), %rsi # t398 # t398
    movq $0, %rax
    call mematb
    movq %rax, -152(%rbp)
# load t399 to bx
    movq -152(%rbp), %rcx
    movq %rcx, -64(%rbp)
    movq $5, %rax
    movq %rax, -160(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -160(%rbp), %rsi # t400 # t400
    movq $0, %rax
    call mematb
    movq %rax, -168(%rbp)
# load t401 to by
    movq -168(%rbp), %rcx
    movq %rcx, -88(%rbp)
    movq $16, %rax
    movq %rax, -176(%rbp)
    movq -64(%rbp), %rax # bx # bx
    imulq -176(%rbp), %rax
    movq %rax, -184(%rbp)
    addq -88(%rbp), %rax
    movq %rax, -192(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -192(%rbp), %rsi # t404 # t404
    movq -40(%rbp), %rdx # val # val
    movq $0, %rax
    call memstoreb
    movq %rax, -200(%rbp)
    movq $6, %rax
    movq %rax, -208(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -208(%rbp), %rsi # t406 # t406
    movq $0, %rax
    call mematb
    movq %rax, -216(%rbp)
# load t407 to val
    movq -216(%rbp), %rcx
    movq %rcx, -40(%rbp)
    movq $7, %rax
    movq %rax, -224(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -224(%rbp), %rsi # t408 # t408
    movq $0, %rax
    call mematb
    movq %rax, -232(%rbp)
# load t409 to bx
    movq -232(%rbp), %rcx
    movq %rcx, -64(%rbp)
    movq $8, %rax
    movq %rax, -240(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -240(%rbp), %rsi # t410 # t410
    movq $0, %rax
    call mematb
    movq %rax, -248(%rbp)
# load t411 to by
    movq -248(%rbp), %rcx
    movq %rcx, -88(%rbp)
    movq $16, %rax
    movq %rax, -256(%rbp)
    movq -64(%rbp), %rax # bx # bx
    imulq -256(%rbp), %rax
    movq %rax, -264(%rbp)
    addq -88(%rbp), %rax
    movq %rax, -272(%rbp)
    movq -8(%rbp), %rdi # grid # grid
    movq -272(%rbp), %rsi # t414 # t414
    movq -40(%rbp), %rdx # val # val
    movq $0, %rax
    call memstoreb
    movq %rax, -280(%rbp)
    movq $0, %rax
    movq %rax, -288(%rbp)
    leave
    ret
.globl move_block_down
move_block_down:
    pushq %rbp
    movq %rsp, %rbp
    subq $400, %rsp
    movq $0, %rcx
    movq %rdi, -8(%rbp)
    movq %rsi, -16(%rbp)
    movq $7, %rax
    movq %rax, -24(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -24(%rbp), %rsi # t417 # t417
    movq $0, %rax
    call mematb
    movq %rax, -32(%rbp)
    movq %rax, -40(%rbp)
    movq $8, %rax
    movq %rax, -48(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -48(%rbp), %rsi # t419 # t419
    movq $0, %rax
    call mematb
    movq %rax, -56(%rbp)
    movq $1, %rax
    movq %rax, -64(%rbp)
    movq -56(%rbp), %rax # t420 # t420
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
    movq -136(%rbp), %rsi # t427 # t427
    movq $0, %rax
    call mematb
    movq %rax, -144(%rbp)
    movq $15, %rax
    movq %rax, -152(%rbp)
    movq -144(%rbp), %rax # t428 # t428
    movq -152(%rbp), %rdi # t429 # t429
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -160(%rbp)
    movq $0, %rax
    movq %rax, -168(%rbp)
    movq -128(%rbp), %rsi # test_block # test_block
    movq -168(%rbp), %rdi # t431 # t431
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -176(%rbp)
    movq -160(%rbp), %rsi # t430 # t430
    cmpq $0, %rsi
    setne %al
    movq -176(%rbp), %rdi # t432 # t432
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -184(%rbp)
    cmpq $0, %rax
    je sublabel_else_434
sublabel_if_434: 
    movq $2, %rax
    movq %rax, -192(%rbp)
    movq $2, %rax
    movq %rax, -200(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -200(%rbp), %rsi # t436 # t436
    movq $0, %rax
    call mematb
    movq %rax, -208(%rbp)
    movq $1, %rax
    movq %rax, -216(%rbp)
    movq -208(%rbp), %rax # t437 # t437
    addq -216(%rbp), %rax
    movq %rax, -224(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -192(%rbp), %rsi # t435 # t435
    movq -224(%rbp), %rdx # t439 # t439
    movq $0, %rax
    call memstoreb
    movq %rax, -232(%rbp)
    movq $5, %rax
    movq %rax, -240(%rbp)
    movq $5, %rax
    movq %rax, -248(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -248(%rbp), %rsi # t442 # t442
    movq $0, %rax
    call mematb
    movq %rax, -256(%rbp)
    movq $1, %rax
    movq %rax, -264(%rbp)
    movq -256(%rbp), %rax # t443 # t443
    addq -264(%rbp), %rax
    movq %rax, -272(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -240(%rbp), %rsi # t441 # t441
    movq -272(%rbp), %rdx # t445 # t445
    movq $0, %rax
    call memstoreb
    movq %rax, -280(%rbp)
    movq $8, %rax
    movq %rax, -288(%rbp)
    movq $8, %rax
    movq %rax, -296(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -296(%rbp), %rsi # t448 # t448
    movq $0, %rax
    call mematb
    movq %rax, -304(%rbp)
    movq $1, %rax
    movq %rax, -312(%rbp)
    movq -304(%rbp), %rax # t449 # t449
    addq -312(%rbp), %rax
    movq %rax, -320(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq -288(%rbp), %rsi # t447 # t447
    movq -320(%rbp), %rdx # t451 # t451
    movq $0, %rax
    call memstoreb
    movq %rax, -328(%rbp)
    jmp sublabel_end_434
sublabel_else_434: 
    movq -8(%rbp), %rdi # grid # grid
    movq -16(%rbp), %rsi # block # block
    movq $0, %rax
    call set_block_in_grid
    movq %rax, -336(%rbp)
    movq -16(%rbp), %rdi # block # block
    movq $0, %rax
    call setup_block
    movq %rax, -344(%rbp)
sublabel_end_434: 
    movq $0, %rax
    movq %rax, -352(%rbp)
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
    movq -16(%rbp), %rsi # t456 # t456
    movq $0, %rax
    call mematb
    movq %rax, -24(%rbp)
    movq %rax, -32(%rbp)
    movq $3, %rax
    movq %rax, -40(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -40(%rbp), %rsi # t458 # t458
    movq $0, %rax
    call mematb
    movq %rax, -48(%rbp)
    movq %rax, -56(%rbp)
    movq $6, %rax
    movq %rax, -64(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -64(%rbp), %rsi # t460 # t460
    movq $0, %rax
    call mematb
    movq %rax, -72(%rbp)
    movq %rax, -80(%rbp)
    movq $0, %rax
    movq %rax, -88(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -88(%rbp), %rsi # t462 # t462
    movq -80(%rbp), %rdx # c3 # c3
    movq $0, %rax
    call memstoreb
    movq %rax, -96(%rbp)
    movq $3, %rax
    movq %rax, -104(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -104(%rbp), %rsi # t464 # t464
    movq -32(%rbp), %rdx # c1 # c1
    movq $0, %rax
    call memstoreb
    movq %rax, -112(%rbp)
    movq $6, %rax
    movq %rax, -120(%rbp)
    movq -8(%rbp), %rdi # block # block
    movq -120(%rbp), %rsi # t466 # t466
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
    subq $1136, %rsp
    movq $0, %rcx
    movq $0, %rax
    call sdl_init
    movq %rax, -8(%rbp)
    leaq t470(%rip), %rax
    movq %rax, -16(%rbp)
    movq $1440, %rax
    movq %rax, -24(%rbp)
    movq $1080, %rax
    movq %rax, -32(%rbp)
    movq -16(%rbp), %rdi # t470 # t470
    movq -24(%rbp), %rsi # t471 # t471
    movq -32(%rbp), %rdx # t472 # t472
    movq $0, %rax
    call sdl_create
    movq %rax, -40(%rbp)
    movq $0, %rax
    movq %rax, -48(%rbp)
    movq -48(%rbp), %rdi # t474 # t474
    movq $0, %rax
    call time
    movq %rax, -56(%rbp)
    movq -56(%rbp), %rdi # t475 # t475
    movq $0, %rax
    call srand
    movq %rax, -64(%rbp)
    movq $11, %rax
    movq %rax, -72(%rbp)
    movq $16, %rax
    movq %rax, -80(%rbp)
    movq -72(%rbp), %rdi # t477 # t477
    movq -80(%rbp), %rsi # t478 # t478
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
while_start_484: 
    movq $0, %rax
    call sdl_pump
    movq %rax, -160(%rbp)
    cmpq $0, %rax
    je while_end_484
    movq $0, %rax
    call sdl_clear
    movq %rax, -168(%rbp)
    movq -96(%rbp), %rdi # grid # grid
    movq -112(%rbp), %rsi # block # block
    movq $0, %rax
    call draw_grid
    movq %rax, -176(%rbp)
    movq -96(%rbp), %rdi # grid # grid
    movq $0, %rax
    call check_blocks
    movq %rax, -184(%rbp)
    movq -96(%rbp), %rdi # grid # grid
    movq $0, %rax
    call move_blocks
    movq %rax, -192(%rbp)
    movq $0, %rax
    call sdl_flip
    movq %rax, -200(%rbp)
    movq $0, %rax
    call sdl_getticks
    movq %rax, -208(%rbp)
    movq %rax, -216(%rbp)
    subq -152(%rbp), %rax
    movq %rax, -224(%rbp)
    movq $750, %rax
    movq %rax, -232(%rbp)
    movq -224(%rbp), %rax # t492 # t492
    movq -232(%rbp), %rdi # t493 # t493
    cmpq %rdi, %rax
    setge %cl
    movzbq %cl, %rdx
    movq %rdx, -240(%rbp)
    movq -240(%rbp), %rax # t494 # t494
    cmpq $0, %rax
    je sublabel_else_495
sublabel_if_495: 
    movq -96(%rbp), %rdi # grid # grid
    movq -112(%rbp), %rsi # block # block
    movq $0, %rax
    call move_block_down
    movq %rax, -248(%rbp)
# load ctime to update_time
    movq -216(%rbp), %rcx
    movq %rcx, -152(%rbp)
    jmp sublabel_end_495
sublabel_else_495: 
sublabel_end_495: 
    movq $0, %rax
    call sdl_getticks
    movq %rax, -256(%rbp)
    movq %rax, -264(%rbp)
    subq -136(%rbp), %rax
    movq %rax, -272(%rbp)
    movq $100, %rax
    movq %rax, -280(%rbp)
    movq -272(%rbp), %rax # t498 # t498
    movq -280(%rbp), %rdi # t499 # t499
    cmpq %rdi, %rax
    setge %cl
    movzbq %cl, %rdx
    movq %rdx, -288(%rbp)
    movq -288(%rbp), %rax # t500 # t500
    cmpq $0, %rax
    je sublabel_else_501
sublabel_if_501: 
# load current_time to prev_time
    movq -264(%rbp), %rcx
    movq %rcx, -136(%rbp)
    movq $7, %rax
    movq %rax, -296(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -296(%rbp), %rsi # t502 # t502
    movq $0, %rax
    call mematb
    movq %rax, -304(%rbp)
    movq $1, %rax
    movq %rax, -312(%rbp)
    movq -304(%rbp), %rax # t503 # t503
    addq -312(%rbp), %rax
    movq %rax, -320(%rbp)
    movq %rax, -328(%rbp)
    movq $8, %rax
    movq %rax, -336(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -336(%rbp), %rsi # t506 # t506
    movq $0, %rax
    call mematb
    movq %rax, -344(%rbp)
    movq %rax, -352(%rbp)
    movq $16, %rax
    movq %rax, -360(%rbp)
    movq -328(%rbp), %rax # bx # bx
    imulq -360(%rbp), %rax
    movq %rax, -368(%rbp)
    addq -352(%rbp), %rax
    movq %rax, -376(%rbp)
    movq %rax, -384(%rbp)
    movq -96(%rbp), %rdi # grid # grid
    movq -384(%rbp), %rsi # pos # pos
    movq $0, %rax
    call mematb
    movq %rax, -392(%rbp)
    movq %rax, -400(%rbp)
    movq $79, %rax
    movq %rax, -408(%rbp)
    movq -408(%rbp), %rdi # t512 # t512
    movq $0, %rax
    call sdl_keydown
    movq %rax, -416(%rbp)
    movq $0, %rax
    movq %rax, -424(%rbp)
    movq -400(%rbp), %rsi # test_block # test_block
    movq -424(%rbp), %rdi # t514 # t514
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -432(%rbp)
    movq -416(%rbp), %rsi # t513 # t513
    cmpq $0, %rsi
    setne %al
    movq -432(%rbp), %rdi # t515 # t515
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -440(%rbp)
    movq $1, %rax
    movq %rax, -448(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -448(%rbp), %rsi # t517 # t517
    movq $0, %rax
    call mematb
    movq %rax, -456(%rbp)
    movq $1440, %rax
    movq %rax, -464(%rbp)
    movq $32, %rax
    movq %rax, -472(%rbp)
    movq $4, %rax
    movq %rax, -480(%rbp)
    movq -472(%rbp), %rax # t520 # t520
    imulq -480(%rbp), %rax
    movq %rax, -488(%rbp)
    movq -464(%rbp), %rax # t519 # t519
    cqto
    idivq -488(%rbp)
    movq %rax, -496(%rbp)
    movq $1, %rax
    movq %rax, -504(%rbp)
    movq -496(%rbp), %rax # t523 # t523
    subq -504(%rbp), %rax
    movq %rax, -512(%rbp)
    movq -456(%rbp), %rax # t518 # t518
    movq -512(%rbp), %rdi # t525 # t525
    cmpq %rdi, %rax
    setl %cl
    movzbq %cl, %rdx
    movq %rdx, -520(%rbp)
    movq -440(%rbp), %rsi # t516 # t516
    cmpq $0, %rsi
    setne %al
    movq -520(%rbp), %rdi # t526 # t526
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -528(%rbp)
    cmpq $0, %rax
    je sublabel_else_528
sublabel_if_528: 
    movq $1, %rax
    movq %rax, -536(%rbp)
    movq $1, %rax
    movq %rax, -544(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -544(%rbp), %rsi # t530 # t530
    movq $0, %rax
    call mematb
    movq %rax, -552(%rbp)
    movq $1, %rax
    movq %rax, -560(%rbp)
    movq -552(%rbp), %rax # t531 # t531
    addq -560(%rbp), %rax
    movq %rax, -568(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -536(%rbp), %rsi # t529 # t529
    movq -568(%rbp), %rdx # t533 # t533
    movq $0, %rax
    call memstoreb
    movq %rax, -576(%rbp)
    movq $4, %rax
    movq %rax, -584(%rbp)
    movq $4, %rax
    movq %rax, -592(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -592(%rbp), %rsi # t536 # t536
    movq $0, %rax
    call mematb
    movq %rax, -600(%rbp)
    movq $1, %rax
    movq %rax, -608(%rbp)
    movq -600(%rbp), %rax # t537 # t537
    addq -608(%rbp), %rax
    movq %rax, -616(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -584(%rbp), %rsi # t535 # t535
    movq -616(%rbp), %rdx # t539 # t539
    movq $0, %rax
    call memstoreb
    movq %rax, -624(%rbp)
    movq $7, %rax
    movq %rax, -632(%rbp)
    movq $7, %rax
    movq %rax, -640(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -640(%rbp), %rsi # t542 # t542
    movq $0, %rax
    call mematb
    movq %rax, -648(%rbp)
    movq $1, %rax
    movq %rax, -656(%rbp)
    movq -648(%rbp), %rax # t543 # t543
    addq -656(%rbp), %rax
    movq %rax, -664(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -632(%rbp), %rsi # t541 # t541
    movq -664(%rbp), %rdx # t545 # t545
    movq $0, %rax
    call memstoreb
    movq %rax, -672(%rbp)
    jmp sublabel_end_528
sublabel_else_528: 
    movq $7, %rax
    movq %rax, -680(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -680(%rbp), %rsi # t547 # t547
    movq $0, %rax
    call mematb
    movq %rax, -688(%rbp)
    movq $1, %rax
    movq %rax, -696(%rbp)
    movq -688(%rbp), %rax # t548 # t548
    subq -696(%rbp), %rax
    movq %rax, -704(%rbp)
# load t550 to bx
    movq -704(%rbp), %rcx
    movq %rcx, -328(%rbp)
    movq $8, %rax
    movq %rax, -712(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -712(%rbp), %rsi # t551 # t551
    movq $0, %rax
    call mematb
    movq %rax, -720(%rbp)
# load t552 to by
    movq -720(%rbp), %rcx
    movq %rcx, -352(%rbp)
    movq $16, %rax
    movq %rax, -728(%rbp)
    movq -328(%rbp), %rax # bx # bx
    imulq -728(%rbp), %rax
    movq %rax, -736(%rbp)
    addq -352(%rbp), %rax
    movq %rax, -744(%rbp)
# load t555 to pos
    movq -744(%rbp), %rcx
    movq %rcx, -384(%rbp)
    movq -96(%rbp), %rdi # grid # grid
    movq -384(%rbp), %rsi # pos # pos
    movq $0, %rax
    call mematb
    movq %rax, -752(%rbp)
# load t556 to test_block
    movq -752(%rbp), %rcx
    movq %rcx, -400(%rbp)
    movq $80, %rax
    movq %rax, -760(%rbp)
    movq -760(%rbp), %rdi # t557 # t557
    movq $0, %rax
    call sdl_keydown
    movq %rax, -768(%rbp)
    movq $0, %rax
    movq %rax, -776(%rbp)
    movq -400(%rbp), %rsi # test_block # test_block
    movq -776(%rbp), %rdi # t559 # t559
    cmpq %rdi, %rsi
    sete %cl
    movzbq %cl, %rdx
    movq %rdx, -784(%rbp)
    movq -768(%rbp), %rsi # t558 # t558
    cmpq $0, %rsi
    setne %al
    movq -784(%rbp), %rdi # t560 # t560
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -792(%rbp)
    movq $1, %rax
    movq %rax, -800(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -800(%rbp), %rsi # t562 # t562
    movq $0, %rax
    call mematb
    movq %rax, -808(%rbp)
    movq $0, %rax
    movq %rax, -816(%rbp)
    movq -808(%rbp), %rax # t563 # t563
    movq -816(%rbp), %rdi # t564 # t564
    cmpq %rdi, %rax
    setg %cl
    movzbq %cl, %rdx
    movq %rdx, -824(%rbp)
    movq -792(%rbp), %rsi # t561 # t561
    cmpq $0, %rsi
    setne %al
    movq -824(%rbp), %rdi # t565 # t565
    cmpq $0, %rdi
    setne %cl
    andb %al, %cl
    movzbq %cl, %rax
    movq %rax, -832(%rbp)
    cmpq $0, %rax
    je sublabel_else_567
sublabel_if_567: 
    movq $1, %rax
    movq %rax, -840(%rbp)
    movq $1, %rax
    movq %rax, -848(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -848(%rbp), %rsi # t569 # t569
    movq $0, %rax
    call mematb
    movq %rax, -856(%rbp)
    movq $1, %rax
    movq %rax, -864(%rbp)
    movq -856(%rbp), %rax # t570 # t570
    subq -864(%rbp), %rax
    movq %rax, -872(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -840(%rbp), %rsi # t568 # t568
    movq -872(%rbp), %rdx # t572 # t572
    movq $0, %rax
    call memstoreb
    movq %rax, -880(%rbp)
    movq $4, %rax
    movq %rax, -888(%rbp)
    movq $4, %rax
    movq %rax, -896(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -896(%rbp), %rsi # t575 # t575
    movq $0, %rax
    call mematb
    movq %rax, -904(%rbp)
    movq $1, %rax
    movq %rax, -912(%rbp)
    movq -904(%rbp), %rax # t576 # t576
    subq -912(%rbp), %rax
    movq %rax, -920(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -888(%rbp), %rsi # t574 # t574
    movq -920(%rbp), %rdx # t578 # t578
    movq $0, %rax
    call memstoreb
    movq %rax, -928(%rbp)
    movq $7, %rax
    movq %rax, -936(%rbp)
    movq $7, %rax
    movq %rax, -944(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -944(%rbp), %rsi # t581 # t581
    movq $0, %rax
    call mematb
    movq %rax, -952(%rbp)
    movq $1, %rax
    movq %rax, -960(%rbp)
    movq -952(%rbp), %rax # t582 # t582
    subq -960(%rbp), %rax
    movq %rax, -968(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq -936(%rbp), %rsi # t580 # t580
    movq -968(%rbp), %rdx # t584 # t584
    movq $0, %rax
    call memstoreb
    movq %rax, -976(%rbp)
    jmp sublabel_end_567
sublabel_else_567: 
    movq $81, %rax
    movq %rax, -984(%rbp)
    movq -984(%rbp), %rdi # t586 # t586
    movq $0, %rax
    call sdl_keydown
    movq %rax, -992(%rbp)
    cmpq $0, %rax
    je sublabel_else_588
sublabel_if_588: 
    movq -96(%rbp), %rdi # grid # grid
    movq -112(%rbp), %rsi # block # block
    movq $0, %rax
    call move_block_down
    movq %rax, -1000(%rbp)
    jmp sublabel_end_588
sublabel_else_588: 
    movq $82, %rax
    movq %rax, -1008(%rbp)
    movq -1008(%rbp), %rdi # t590 # t590
    movq $0, %rax
    call sdl_keydown
    movq %rax, -1016(%rbp)
    cmpq $0, %rax
    je sublabel_else_592
sublabel_if_592: 
    movq -112(%rbp), %rdi # block # block
    movq $0, %rax
    call swap_colors
    movq %rax, -1024(%rbp)
    jmp sublabel_end_592
sublabel_else_592: 
sublabel_end_592: 
sublabel_end_588: 
sublabel_end_567: 
sublabel_end_528: 
    jmp sublabel_end_501
sublabel_else_501: 
sublabel_end_501: 
    jmp while_start_484
while_end_484: 
    movq -96(%rbp), %rdi # grid # grid
    movq $0, %rax
    call free
    movq %rax, -1032(%rbp)
    movq -112(%rbp), %rdi # block # block
    movq $0, %rax
    call free
    movq %rax, -1040(%rbp)
    movq $0, %rax
    call sdl_release
    movq %rax, -1048(%rbp)
    movq $0, %rax
    call sdl_quit
    movq %rax, -1056(%rbp)
    movq $0, %rax
    movq %rax, -1064(%rbp)
    leave
    ret
.section .note.GNU-stack,"",@progbits

