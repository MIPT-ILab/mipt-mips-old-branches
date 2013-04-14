	 .data
words:
		 .space 1
         .text
         .global __start
 __start:
 	addi  $t0, $t0, 0xDEAD
 	addi  $t1, $t1, 1
 	add   $t3, $t0, $t1
 	sub   $t3, $t1, $t3
 	move  $v0, $t3
