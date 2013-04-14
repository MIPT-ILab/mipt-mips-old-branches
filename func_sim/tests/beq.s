	.data
words:
	.space 1

    .text
    .global __start
 __start:
 	addi  $t0, $t0, 0x1
 	addi  $t1, $t1, 0x1
 	addi  $t2, $t2, 0x2
 	beq $t0, $t2, __start
 	beq $t0, $t1, __start