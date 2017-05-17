	 .data
words:
		 .space 1
         .text
         .global __start
 __start:
 	addi  $t0, $t0, 0x1000
 	sll   $t1, $t0, 2
 	srl   $t1, $t0, 2
 	move  $v0, $t1
