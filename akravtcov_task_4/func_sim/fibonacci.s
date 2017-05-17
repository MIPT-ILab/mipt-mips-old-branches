# fibonacci.s - the mips-assembly programm, that computes a definitly member of
#               the Fibonacci numbers, placed in the data section as input_value,
#               and saves result into $v0 register
# author Alexander Kravtcov

.data
    input_value: .word 10 # will be compute F(10)

.text
.global __start
 __start:
    la  $t0, input_value 
    lw  $t1, 0($t0)    # [$t1] = input_value[0]

    move $t3, $zero    # F(0) = 0
    addi $t4, $zero, 1 # F(1) = 1

    beq $t1, $t3, save_zero_member
    beq $t1, $t4, save_first_member

    addi $t2, $zero, 1 # counter = 1

  begin_loop:
    beq $t1, $t2, end_of_loop
    
    add $t5, $t3, $t4 # F(i) = F(i-1) + F(i-2)
    move $t3, $t4     # update registers for the next calculation
    move $t4, $t5
    
    # if compile by mips-as without flag -O0, instr "j" will be
    # executed before "addi"
    addi $t2, $t2, 1 # counter++
    j begin_loop    

  end_of_loop:
    move $v0, $t5

  save_zero_member:
    move $v0, $t3
    
  save_first_member:
    move $v0, $t4
    
