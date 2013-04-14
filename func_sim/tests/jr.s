	 .data
words:
		 .space 1
         .text
         .global __start
 __start:
 	addi $t0, $t0, 0xDEAD
 	jr $t0
