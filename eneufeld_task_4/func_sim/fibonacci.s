.data
val: .word 10;
.text
lw $8, val;     # N
addi $9, $0, 1; # [n-1]
addi $10, $0, 1;# [n]
addi $11, $0, 2;#  n

do:
add  $12, $9, $10; # tmp = [n] + [n-1]
add  $9, $0, $10;  # [n-1] = [n]
add  $10, $0, $12; # [n] = tmp;
addi $11, $11, 1;  # inc(n)
bne $11, $8, do;   # if (N != m) goto do

add  $2, $0, $10;  #return [N] 
