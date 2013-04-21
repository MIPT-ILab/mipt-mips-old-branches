    .data

input_value: .word 10

    .text

    .global __start

__start:
# initial data loading
        la   $t2, input_value
        lw   $t1, 0($t2)
        addi $t1, $t1, 1
        addu $t3, $zero, $zero
        addi $t4, $zero, 1 # result will store here
# external loop of calculating
follow: addi $t5, $zero, 1 # counter for mult loop
        addi $t3, $t3, 1 # counter for external loop
        beq  $t3, $t1, exit # if t3 = t1 goto exit 
        move $t6, $t4
# internal loop of multiplication
mult:   beq  $t5, $t3, follow
        add  $t4, $t4, $t6
        addi $t5, $t5, 1
        j    mult

exit: move $v0, $t4
