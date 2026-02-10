.section .rodata
.LC1:
        .long   0
        .long   1074790400
.LC2:
        .string "Missing argument N [long long]\n"
.LC3:
        .string "Leibniz precision = %lld\n"
.LC4:
        .string "Calculated PI = %.19g\n"
.LC5:
        .string "Calculation took %.5g sec.\n"

.section .text
leibniz:
        pushq   %rbp
        movq    %rsp, %rbp
        movq    %rdi, -40(%rbp)
        pxor    %xmm0, %xmm0
        movsd   %xmm0, -8(%rbp)
        movq    $0, -16(%rbp)
        movq    $1, -24(%rbp)
        jmp     .L2
.L3:
        pxor    %xmm0, %xmm0
        cvtsi2sdq       -24(%rbp), %xmm0
        movq    -16(%rbp), %rax
        addq    %rax, %rax
        addq    $1, %rax
        pxor    %xmm1, %xmm1
        cvtsi2sdq       %rax, %xmm1
        divsd   %xmm1, %xmm0
        movsd   -8(%rbp), %xmm1
        addsd   %xmm1, %xmm0
        movsd   %xmm0, -8(%rbp)
        negq    -24(%rbp)
        addq    $1, -16(%rbp)
.L2:
        movq    -16(%rbp), %rax
        cmpq    -40(%rbp), %rax
        jl      .L3
        movsd   -8(%rbp), %xmm1
        movsd   .LC1(%rip), %xmm0
        mulsd   %xmm1, %xmm0
        popq    %rbp
        ret

.globl main
main:
        pushq   %rbp
        movq    %rsp, %rbp
        addq    $-128, %rsp
        movl    %edi, -116(%rbp)
        movq    %rsi, -128(%rbp)
        movl    $2, %edi
        call    sysconf
        movq    %rax, -8(%rbp)
        cmpl    $1, -116(%rbp)
        jg      .L6
        movl    $.LC2, %edi
        movl    $0, %eax
        call    printf
        movl    $1, %eax
        jmp     .L8
.L6:
        movq    -128(%rbp), %rax
        addq    $8, %rax
        movq    (%rax), %rax
        movq    %rax, %rdi
        call    atoll
        movq    %rax, -16(%rbp)
        movq    -16(%rbp), %rax
        movq    %rax, %rsi
        movl    $.LC3, %edi
        movl    $0, %eax
        call    printf
        leaq    -80(%rbp), %rax
        movq    %rax, %rdi
        call    times
        movq    -16(%rbp), %rax
        movq    %rax, %rdi
        call    leibniz
        movq    %xmm0, %rax
        movq    %rax, -24(%rbp)
        leaq    -112(%rbp), %rax
        movq    %rax, %rdi
        call    times
        movq    -112(%rbp), %rdx
        movq    -80(%rbp), %rax
        subq    %rax, %rdx
        movq    %rdx, -32(%rbp)
        pxor    %xmm0, %xmm0
        cvtsi2sdq       -32(%rbp), %xmm0
        pxor    %xmm1, %xmm1
        cvtsi2sdq       -8(%rbp), %xmm1
        divsd   %xmm1, %xmm0
        movsd   %xmm0, -40(%rbp)
        movq    -24(%rbp), %rax
        movq    %rax, %xmm0
        movl    $.LC4, %edi
        movl    $1, %eax
        call    printf
        movq    -40(%rbp), %rax
        movq    %rax, %xmm0
        movl    $.LC5, %edi
        movl    $1, %eax
        call    printf
        movl    $0, %eax
.L8:
        leave
        ret
