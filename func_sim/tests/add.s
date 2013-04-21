    .data

input_value: .word 5

    .text

    .global __start

__start:
        la   $t2, input_value
        lw   $t1, 0($t2)
        addi $t1, $t1, 1
        addi $t3, $t3, 1
        addu  $t6, $t4, $zero
        add  $t5, $t5, $zero
        add $v0, $zero, $t4
