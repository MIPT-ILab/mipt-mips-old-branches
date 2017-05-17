.data 

input_value: .word 10 

.text 

.global __start 

__start:
      la    $t0, input_value
      lw    $t0, 0($t0)   # $t0 - factorial to calculate
      addi  $t1, $zero, 1 # Current result

loop: beq   $t0, $zero, ret
      addu  $t2, $zero, $t0
      addi  $t0, $t0, -1
      addu  $t3, $zero, $t1
mul:  addi  $t2, $t2, -1
      beq   $t2, $zero, loop
      addu  $t1, $t1, $t3
      j     mul

ret:  addu  $v0, $zero, $t1