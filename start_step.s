.data
    width: .word 0        @ int
    .balign 4
    height: .word 0       @ int
    .balign 4
    coeff: .word 0        @ fixed
    .balign 4
    matrix: .word 0       @ fixed*
    .balign 4
    diff_matrix: .word 0  @ fixed*
    .balign 4

.global start, step

.text

@ void start(int szer, int wys, fixed *M, fixed coeff);
start:
    @ arguments
    @ r0 - width
    @ r1 - height
    @ r2 - matrix pointer
    @ r3 - coeff

    str lr, [sp, #-4]!
    stmdb sp!, {r4}

    ldr r4, =width
    str r0, [r4]
    ldr r4, =height
    str r1, [r4]
    ldr r4, =matrix
    str r2, [r4]
    ldr r4, =coeff
    str r3, [r4]

    mul r3, r0, r1
    lsl r3, #2
    add r2, r3
    ldr r4, =diff_matrix
    str r2, [r4]

    ldmia sp!, {r4}
    ldr lr, [sp], #4
    bx lr

@ ------------------------------------------------------------

@ fixed get_diff(fixed this, uint32_t r, uint32_t c);
get_diff:
    @ arguments
    @ r0 - this
    @ r1 - row
    @ r2 - column

    str lr, [sp, #-4]!
    stmdb sp!, {r4}

    cmp r1, #0
    blt .return_0
    cmp r2, #0
    blt .return_0

    ldr r3, =height
    ldr r4, [r3]
    cmp r1, r4
    bge .return_0

    ldr r3, =width
    ldr r4, [r3]
    cmp r2, r4
    bge .return_0

    @ r0 = value to subtract, r1 = row, r2 = column, r4 = width
    mul r3, r1, r4
    add r3, r2
    lsl r3, #2

    @ r0 = value to subtract, r3 = cell offset in matrix
    ldr r4, =matrix
    ldr r4, [r4]
    add r4, r3
    ldr r3, [r4]

    @ r0 = value to subtract, r3 = cell value
    sub r3, r0
    mov r0, r3

    ldmia sp!, {r4}
    ldr lr, [sp], #4
    bx lr

.return_0:
    mov r0, #0
    ldmia sp!, {r4}
    ldr lr, [sp], #4
    bx lr

@ ------------------------------------------------------------

@ fixed get_delta(fixed this, uint32_t r, uint32_t c);
get_delta:
    @ arguments
    @ r0 - this
    @ r1 - row
    @ r2 - column

    str lr, [sp, #-4]!
    stmdb sp!, {r4-r7}

    mov r4, r0
    mov r5, r1
    mov r6, r2
    mov r7, #0
    @ r4 = this, r5 = row, r6 = column, r7 will hold sum of the 5 differences

    @ r - 1, c
    sub r1, #1
    bl get_diff
    add r7, r0

    @ r - 1, c - 1
    mov r0, r4
    mov r1, r5
    mov r2, r6
    sub r1, #1
    sub r2, #1
    bl get_diff
    add r7, r0

    @ r, c - 1
    mov r0, r4
    mov r1, r5
    mov r2, r6
    sub r2, #1
    bl get_diff
    add r7, r0

    @ r + 1, c - 1
    mov r0, r4
    mov r1, r5
    mov r2, r6
    add r1, #1
    sub r2, #1
    bl get_diff
    add r7, r0

    @ r + 1, c
    mov r0, r4
    mov r1, r5
    mov r2, r6
    add r1, #1
    bl get_diff
    add r7, r0

    @ r7 = sum of the 5 differences

    ldr r3, =coeff
    ldr r4, [r3]

    @ r4 = coefficient, r7 = sum of the 5 differences

    cmp r7, #0
    bge .delta_positive

    mov r6, #0
    sub r6, r7
    mul r6, r4
    lsr r6, #8
    mov r7, #0
    sub r7, r6
    b .return_delta

.delta_positive:
    mul r7, r4
    lsr r7, #8

.return_delta:
    mov r0, r7
    ldmia sp!, {r4-r7}
    ldr lr, [sp], #4
    bx lr

@ ------------------------------------------------------------

@ void calculate_diffs_for_step();
calculate_diffs_for_step:
    str lr, [sp, #-4]!
    stmdb sp!, {r4-r10}

    ldr r3, =width
    ldr r5, [r3]
    ldr r3, =height
    ldr r6, [r3]
    ldr r3, =matrix
    ldr r9, [r3]
    ldr r3, =diff_matrix
    ldr r10, [r3]

    mov r7, #1

    @ r5 = width, r6 = height, r7 = column, r8 = row, r9 = matrix ptr, r10 = diff_matrix ptr

.loop_columns:
    mov r8, #0
.loop_rows:
    mul r1, r8, r5
    add r1, r7
    lsl r1, #2
    mov r4, r1
    mov r2, r9
    add r2, r4
    ldr r0, [r2]

    @ r0 = this

    mov r1, r8
    mov r2, r7

    bl get_delta

    mov r2, r10
    add r2, r4
    str r0, [r2]

    add r8, #1
    cmp r8, r6
    blt .loop_rows

    add r7, #1
    cmp r7, r5
    blt .loop_columns

    ldmia sp!, {r4-r10}
    ldr lr, [sp], #4
    bx lr

@ ------------------------------------------------------------

@ void apply_diffs(fixed T[]);
apply_diffs:
    @ arguments
    @ r0 - T array

    str lr, [sp, #-4]!
    stmdb sp!, {r4-r10}

    mov r4, r0

    ldr r3, =width
    ldr r5, [r3]
    ldr r3, =height
    ldr r6, [r3]
    mov r7, #1

    ldr r3, =matrix
    ldr r9, [r3]
    ldr r3, =diff_matrix
    ldr r10, [r3]

    mul ip, r5, r6
    mov r8, #0

    @ r4 = T[], r5 = width, r6 = height, r8 = i, r9 = matrix ptr, r10 = diff_matrix ptr, ip = number of cells

.loop_matrix_cells:
    mov r1, r8
    lsl r1, #2

    mov r0, r10
    add r0, r1
    ldr r2, [r0]
    @ r2 = diff_matrix[i]

    mov r0, r9
    add r0, r1
    ldr r3, [r0]
    add r3, r2
    @ r3 = matrix[i] + diff_matrix[i]
    str r3, [r0]

    add r8, #1
    cmp r8, ip
    blt .loop_matrix_cells

    mov r10, #0
    @ r4 = T[], r5 = width, r6 = height, r9 = matrix ptr, r10 = row

.loop_new_values:
    mov r1, r10
    lsl r1, #2
    mov r0, r4
    add r0, r1
    ldr r2, [r0]
    @ r2 = T[i]

    mov r3, r10
    mul r1, r3, r5
    lsl r1, #2
    mov r0, r9
    add r0, r1
    str r2, [r0]

    add r10, #1
    cmp r10, r6
    blt .loop_new_values

    ldmia sp!, {r4-r10}
    ldr lr, [sp], #4
    bx lr

@ ------------------------------------------------------------

@ void step(fixed T[]);
step:
    @ arguments
    @ r0 - T array

    str lr, [sp, #-4]!
    stmdb sp!, {r4}

    mov r4, r0

    bl calculate_diffs_for_step

    mov r0, r4
    bl apply_diffs

    ldmia sp!, {r4}
    ldr lr, [sp], #4
    bx lr

