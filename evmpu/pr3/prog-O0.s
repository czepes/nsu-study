leibniz:
        push    {r4, r5, r7, r8, r9, r10, fp, lr}
        vpush.64        {d8}
        sub     sp, sp, #40
        add     r7, sp, #0
        strd    r0, [r7, #8]
        mov     r2, #0
        mov     r3, #0
        strd    r2, [r7, #32]
        vmov.i32        d16, #0  @ di
        vstr.64 d16, [r7, #24]    @ int
        mov     r2, #1
        mov     r3, #0
        strd    r2, [r7, #16]
        b       .L2
.L3:
        ldrd    r0, [r7, #16]
        bl      __aeabi_l2d
        vmov    d8, r0, r1
        ldrd    r2, [r7, #24]
        adds    r1, r2, r2
        str     r1, [r7]
        adcs    r3, r3, r3
        str     r3, [r7, #4]
        ldrd    r2, [r7]
        adds    r10, r2, #1
        adc     fp, r3, #0
        mov     r0, r10
        mov     r1, fp
        bl      __aeabi_l2d
        vmov    d17, r0, r1
        vdiv.f64        d16, d8, d17
        vldr.64 d17, [r7, #32]
        vadd.f64        d16, d17, d16
        vstr.64 d16, [r7, #32]
        ldrd    r2, [r7, #16]
        movs    r1, #0
        negs    r4, r2
        sbc     r5, r1, r3
        strd    r4, [r7, #16]
        ldrd    r2, [r7, #24]
        adds    r8, r2, #1
        adc     r9, r3, #0
        strd    r8, [r7, #24]
.L2:
        ldrd    r0, [r7, #24]
        ldrd    r2, [r7, #8]
        cmp     r0, r2
        sbcs    r3, r1, r3
        blt     .L3
        vldr.64 d16, [r7, #32]
        vmov.f64        d17, #4.0e+0
        vmul.f64        d16, d16, d17
        vmov.f64        d0, d16
        adds    r7, r7, #40
        mov     sp, r7
        vldm    sp!, {d8}
        pop     {r4, r5, r7, r8, r9, r10, fp, pc}
.LC0:
        .ascii  "Missing argument N [long long]\000"
.LC1:
        .ascii  "Leibniz precision = %lld\000"
.LC2:
        .ascii  "Calculated PI = %.19g\012\000"
.LC3:
        .ascii  "Calculation took %.5g sec.\012\000"
main:
        push    {r7, lr}
        sub     sp, sp, #80
        add     r7, sp, #0
        str     r0, [r7, #4]
        str     r1, [r7]
        movs    r0, #2
        bl      sysconf
        str     r0, [r7, #76]
        ldr     r3, [r7, #4]
        cmp     r3, #1
        bgt     .L6
        movw    r0, #:lower16:.LC0
        movt    r0, #:upper16:.LC0
        bl      printf
        movs    r3, #1
        b       .L8
.L6:
        ldr     r3, [r7]
        adds    r3, r3, #4
        ldr     r3, [r3]
        mov     r0, r3
        bl      atoll
        strd    r0, [r7, #64]
        ldrd    r2, [r7, #64]
        movw    r0, #:lower16:.LC1
        movt    r0, #:upper16:.LC1
        bl      printf
        add     r3, r7, #24
        mov     r0, r3
        bl      times
        ldrd    r0, [r7, #64]
        bl      leibniz
        vstr.64 d0, [r7, #56]
        add     r3, r7, #8
        mov     r0, r3
        bl      times
        ldr     r2, [r7, #8]
        ldr     r3, [r7, #24]
        subs    r3, r2, r3
        str     r3, [r7, #52]
        ldr     r3, [r7, #52]
        vmov    s15, r3 @ int
        vcvt.f64.s32    d18, s15
        ldr     r3, [r7, #76]
        vmov    s15, r3 @ int
        vcvt.f64.s32    d17, s15
        vdiv.f64        d16, d18, d17
        vstr.64 d16, [r7, #40]
        ldrd    r2, [r7, #56]
        movw    r0, #:lower16:.LC2
        movt    r0, #:upper16:.LC2
        bl      printf
        ldrd    r2, [r7, #40]
        movw    r0, #:lower16:.LC3
        movt    r0, #:upper16:.LC3
        bl      printf
        movs    r3, #0
.L8:
        mov     r0, r3
        adds    r7, r7, #80
        mov     sp, r7
        pop     {r7, pc}
