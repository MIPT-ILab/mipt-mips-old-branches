    .data
input_value:
    .word 10

    .text
    .global __start
 __start:
    la    $t1, input_value
    lw    $t6, ($t1)
    add   $t4, $zero, 1
    add   $t5, $zero, 0

    addi  $t2, $zero, 1
fibb_loop:
    beq   $t2, $t6, end
    move  $t3, $t4
    add   $t4, $t4, $t5
    move  $t5, $t3
    addi  $t2, $t2, 1
    j fibb_loop

end:
    move  $v0, $t4
