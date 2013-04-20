
# factorial.s - the mips-assembly programm, that computes the factorial of
#               a defenitly value, placed in the data section as input_value,
#               and saves result into $v0 register
# author Alexander Kravtcov

.data
    input_value: .word 10 # will be compute F(10)

.text
.global __start
 __start:
    la  $t0, input_value 
    lw  $t1, ($t0)    # [$t1] = input_value[0]

    beq $t1, $zero, save_factorial_of_zero

    addi $t2, $zero, 1 # counter = 1
    addi $t3, $zero, 1 # 1! = 1

  begin_loop:
    beq $t1, $t2, end_of_loop
    
    move $t4, $zero # counter for multiply = 0
    move $t5, $t3 # temp current value

      # multiply as [t2]-times addition with current value
      multiply_begin:
        beq $t2, $t4, end_of_multiply
        
        add $t3, $t3, $t5

        addi $t4, $t4, 1 # counter++
        j multiply_begin

      end_of_multiply:

    addi $t2, $t2, 1 # counter++
    j begin_loop    

  end_of_loop:
    move $v0, $t3

  save_factorial_of_zero:
    move $v0, $zero
    
