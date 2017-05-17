	 .data
	 .byte 1 
         .text
         .global __start
 __start:
	lui $t0, 0xDEAD
	lui $t0, 0xBEEF
	lui $v0, 0xBEEF
