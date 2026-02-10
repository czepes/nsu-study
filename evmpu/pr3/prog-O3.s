leibniz:
        push    {r3, r4, r5, r6, r7, r8, r9, lr}
        cmp     r0, #1
        sbcs    r3, r1, #0
        vpush.64        {d8, d9}
        blt     .L4
        adds    r0, r0, r0
        mov     r4, #1
        adcs    r1, r1, r1
        movs    r7, #0
        adds    r8, r0, r4
        vmov.i64        d8, #0  @ float
        adc     r9, r1, #0
        mov     r5, r4
        mov     r6, r7
.L3:
        mov     r0, r5
        mov     r1, r6
        bl      __aeabi_l2d
        vmov    d9, r0, r1
        mov     r0, r4
        mov     r1, r7
        bl      __aeabi_l2d
        vmov    d17, r0, r1
        negs    r5, r5
        vdiv.f64        d16, d9, d17
        sbc     r6, r6, r6, lsl #1
        adds    r4, r4, #2
        adc     r7, r7, #0
        cmp     r9, r7
        it      eq
        cmpeq   r8, r4
        vadd.f64        d8, d8, d16
        bne     .L3
        vmov.f64        d0, #4.0e+0
        vmul.f64        d0, d8, d0
        vldm    sp!, {d8-d9}
        pop     {r3, r4, r5, r6, r7, r8, r9, pc}
.L4:
        vldm    sp!, {d8-d9}
        vmov.i64        d0, #0  @ float
        pop     {r3, r4, r5, r6, r7, r8, r9, pc}
.LC0:
        .ascii  "Missing argument N [long long]\000"
.LC1:
        .ascii  "Leibniz precision = %lld\000"
.LC2:
        .ascii  "Calculated PI = %.19g\012\000"
.LC3:
        .ascii  "Calculation took %.5g sec.\012\000"
main:
        push    {r4, r5, r6, r7, r8, r9, r10, lr}
        mov     r4, r0
        movs    r0, #2
        vpush.64        {d8, d9}
        sub     sp, sp, #32
        mov     r5, r1
        bl      sysconf
        cmp     r4, #1
        ble     .L16
        movs    r2, #10
        mov     r10, r0
        movs    r1, #0
        ldr     r0, [r5, #4]
        bl      __isoc23_strtoll
        mov     r2, r0
        mov     r3, r1
        mov     r8, r0
        movw    r0, #:lower16:.LC1
        movt    r0, #:upper16:.LC1
        mov     r9, r1
        bl      printf
        mov     r0, sp
        bl      times
        cmp     r8, #1
        sbcs    r3, r9, #0
        blt     .L13
        adds    r8, r8, r8
        mov     r4, #1
        adc     r9, r9, r9
        movs    r7, #0
        adds    r8, r8, r4
        vmov.i64        d8, #0  @ float
        adc     r9, r9, #0
        mov     r5, r4
        mov     r6, r7
.L12:
        mov     r0, r5
        mov     r1, r6
        bl      __aeabi_l2d
        vmov    d9, r0, r1
        mov     r0, r4
        mov     r1, r7
        bl      __aeabi_l2d
        vmov    d17, r0, r1
        negs    r5, r5
        vdiv.f64        d16, d9, d17
        sbc     r6, r6, r6, lsl #1
        adds    r4, r4, #2
        adc     r7, r7, #0
        cmp     r9, r7
        it      eq
        cmpeq   r8, r4
        vadd.f64        d8, d8, d16
        bne     .L12
.L11:
        vmov.f64        d16, #4.0e+0
        add     r0, sp, #16
        vmul.f64        d8, d8, d16
        bl      times
        ldr     r0, [sp]
        vmov    s15, r10        @ int
        ldr     r1, [sp, #16]
        vcvt.f64.s32    d17, s15
        subs    r1, r1, r0
        vmov    s15, r1 @ int
        vmov    r2, r3, d8
        movw    r0, #:lower16:.LC2
        movt    r0, #:upper16:.LC2
        vcvt.f64.s32    d16, s15
        vdiv.f64        d8, d16, d17
        bl      printf
        movw    r0, #:lower16:.LC3
        movt    r0, #:upper16:.LC3
        vmov    r2, r3, d8
        bl      printf
        movs    r0, #0
.L8:
        add     sp, sp, #32
        vldm    sp!, {d8-d9}
        pop     {r4, r5, r6, r7, r8, r9, r10, pc}
.L16:
        movw    r0, #:lower16:.LC0
        movt    r0, #:upper16:.LC0
        bl      printf
        movs    r0, #1
        b       .L8
.L13:
        vmov.i64        d8, #0  @ float
        b       .L11
