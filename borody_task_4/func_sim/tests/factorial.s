    .data
input_value:
    .word 10

    .text
    .global __start
 __start:
    la    $t1, input_value
    lw    $t5, ($t1)
    add   $t4, $zero, 1

factorial:
    beq   $t5, $zero, end

    # Multiply $t4 on $t5
    move  $t2, $zero
    move  $t3, $zero
mul_loop:
    beq   $t2, $t5, mul_loop_end
    add   $t3, $t3, $t4
    addi  $t2, $t2, 1
    j     mul_loop
mul_loop_end:
    move  $t4, $t3

    # Decrement $t5
    addi  $t5, $t5, -1
    j factorial
end:
    move  $v0, $t4
