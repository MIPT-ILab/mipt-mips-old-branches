    .data

input_value: .word 10

    .text

    .global __start

__start:
# initial data loading
    la   $t0, input_value
    lw   $t1, ($t0)
    move $t2, $zero
    move $t3, $zero
    move $t4, $zero # result will store here
    move $t5, $zero
    beq  $t1, $zero, exit # if t1 = 0 F = 0
    addi $t3, $t3, 1
    addi $t4, $t4, 1
next_loop:
    addi $t5, $t5, 1
    beq  $t1, $t5, exit # if t1 = t5 goto exit
    add  $t2, $t3, $t2
    move $t3, $t4
    move $t4, $t2
    j    next_loop 

exit:
    move $v0, $t4
