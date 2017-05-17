	 .data
words:
		 .space 1
         .text
         .global __start
 __start:
 	addiu $t0, $t0, 0xDEAD
 	jr $t0
