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
        testq   %rdi, %rdi
        jle     .L4
        leaq    1(%rdi,%rdi), %rcx
        movl    $1, %eax
        movl    $1, %edx
        pxor    %xmm0, %xmm0
.L3:
        pxor    %xmm1, %xmm1
        pxor    %xmm2, %xmm2
        cvtsi2sdq       %rdx, %xmm1
        cvtsi2sdq       %rax, %xmm2
        divsd   %xmm2, %xmm1
        negq    %rdx
        addq    $2, %rax
        addsd   %xmm1, %xmm0
        cmpq    %rax, %rcx
        jne     .L3
        mulsd   .LC1(%rip), %xmm0
        ret
.L4:
        pxor    %xmm0, %xmm0
        ret

.globl main
main:
        pushq   %r12
        movq    %rsi, %r12
        pushq   %rbp
        movl    %edi, %ebp
        movl    $2, %edi
        pushq   %rbx
        subq    $80, %rsp
        call    sysconf
        cmpl    $1, %ebp
        jle     .L15
        movq    8(%r12), %rdi
        xorl    %esi, %esi
        movl    $10, %edx
        movq    %rax, %rbx
        call    strtoll
        movl    $.LC3, %edi
        movq    %rax, %rbp
        movq    %rax, %rsi
        xorl    %eax, %eax
        call    printf
        leaq    16(%rsp), %rdi
        call    times
        testq   %rbp, %rbp
        jle     .L12
        leaq    1(%rbp,%rbp), %rcx
        movl    $1, %eax
        pxor    %xmm0, %xmm0
        movl    $1, %edx
.L11:
        pxor    %xmm1, %xmm1
        pxor    %xmm2, %xmm2
        cvtsi2sdq       %rdx, %xmm1
        cvtsi2sdq       %rax, %xmm2
        divsd   %xmm2, %xmm1
        negq    %rdx
        addq    $2, %rax
        addsd   %xmm1, %xmm0
        cmpq    %rax, %rcx
        jne     .L11
.L10:
        mulsd   .LC1(%rip), %xmm0
        leaq    48(%rsp), %rdi
        movsd   %xmm0, 8(%rsp)
        call    times
        movq    48(%rsp), %rax
        pxor    %xmm1, %xmm1
        subq    16(%rsp), %rax
        pxor    %xmm2, %xmm2
        cvtsi2sdq       %rax, %xmm1
        movsd   8(%rsp), %xmm0
        movl    $.LC4, %edi
        cvtsi2sdq       %rbx, %xmm2
        divsd   %xmm2, %xmm1
        movl    $1, %eax
        movsd   %xmm1, (%rsp)
        call    printf
        movsd   (%rsp), %xmm0
        movl    $.LC5, %edi
        movl    $1, %eax
        call    printf
        xorl    %eax, %eax
.L7:
        addq    $80, %rsp
        popq    %rbx
        popq    %rbp
        popq    %r12
        ret
.L15:
        movl    $.LC2, %edi
        xorl    %eax, %eax
        call    printf
        movl    $1, %eax
        jmp     .L7
.L12:
        pxor    %xmm0, %xmm0
        jmp     .L10
