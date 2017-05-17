.data
val: .word 10
.text
lw $8, val;     # N=val
add  $9, $0, $0; # i = 0
addi $10, $0, 1;#  fact = 1
addi $11, $0, 1;#  n = 1

do:

addi $11, $11, 1;  # inc(n)
nop;
j mul_fact_and_n_in_fact # fact *= n;
res:

bne $11, $8, do;   # if (N != n) goto do

add  $2, $0, $10;  #return [N] 


mul_fact_and_n_in_fact:
add $24, $0, $0 # i = 0
add  $25, $0, $0 # tmp = 0;
do_mul:
add $25, $25, $11; #tmp += n
addi $24, $24, 1; #i++;
bne $24, $10, do_mul # if( fact != i) goto do_mul
add $10, $0, $25  # fact = tmp;
nop
j res 
