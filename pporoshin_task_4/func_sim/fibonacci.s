.data 

input_value: .word 10 

.text 

.global __start 

__start:
      la    $t0, input_value
      lw    $t0, 0($t0)   # $t0 - nubmer of fibonacci number to calculate
      addi  $t1, $zero, 1 # Previous fibonacci number 
      addi  $t2, $zero, 1 # Current ($t1)th fibonacci number

      addi  $t0, $t0, -1
      beq   $t0, $zero, ret
      addi  $t0, $t0, -1
      beq   $t0, $zero, ret

loop: addu  $t3, $t2, $t1
      addu  $t1, $zero, $t2
      addu  $t2, $zero, $t3
      addi  $t0, $t0, -1
      beq   $t0, $zero, ret
      j     loop

ret:  addu  $v0, $zero, $t2