	 .data
words:
		 .word 11, 42 
         .text
         .global __start
 __start:
 	la $t1, words 
	lw $t0, ($t1)
	add $t2, $t2, $t0
	lw $t0, 4($t1)
	sw $t0, ($t1)
	add $t3, $t3, $t0
	lw $v0, ($t1)
