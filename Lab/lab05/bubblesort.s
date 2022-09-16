# read 10 numbers into an array
# bubblesort them
# then print the 10 numbers

# i in register $t0
# registers $t1, $t2 & $t3 used to hold temporary results

main:

    li   $t0, 0         # i = 0
loop0:
    bge  $t0, 10, end0  # while (i < 10) {

    li   $v0, 5         #   scanf("%d", &numbers[i]);
    syscall             #

    mul  $t1, $t0, 4    #   calculate &numbers[i]
    la   $t2, numbers   #
    add  $t3, $t1, $t2  #
    sw   $v0, ($t3)     #   store entered number in array

    addi $t0, $t0, 1    #   i++;
    j    loop0          # }
end0:
    li   $t4, 1         # swapped = 1
loop1:
    bne  $t4, 1, end1   # while (swapped) {
    li   $t4, 0         #   swapped = 0
    li   $t0, 1         #   i = 1
loop2:
    bge  $t0, 10, end2  #   while (i < 10) {
    mul  $t1, $t0, 4    #       calculate &numbers[i]
    la   $t2, numbers   #
    add  $s1, $t1, $t2  #

    lw   $t5, ($s1)     #       x = numbers[i]

    sub  $t6, $t0, 1
    mul  $t1, $t6, 4    #       calculate &numbers[i-1]
    la   $t2, numbers   #
    add  $s2, $t1, $t2  #

    lw   $t6, ($s2)     #       y = numbers[i-1] 
    bge  $t5, $t6, else #       if (x < y) {
    
    sw   $t6, ($s1)     #       numbers[i] = y
    sw   $t5, ($s2)     #       numbers[i-1] = x   
   
    li   $t4, 1         #       swapped = 1

    addi $t0, $t0, 1    #       i++;
    j    loop2          #   }
else:
    addi $t0, $t0, 1    #       i++;
    j    loop2          #   }
end2:
    j    loop1          # }
end1:
    li   $t0, 0         # i = 0
loop3:
    bge  $t0, 10, end3  # while (i < 10) {

    mul  $t1, $t0, 4    #   calculate &numbers[i]
    la   $t2, numbers   #
    add  $t3, $t1, $t2  #
    lw   $a0, ($t3)     #   load numbers[i] into $a0
    li   $v0, 1         #   printf("%d", numbers[i])
    syscall

    li   $a0, '\n'      #   printf("%c", '\n');
    li   $v0, 11
    syscall

    addi $t0, $t0, 1    #   i++
    j    loop3          # }
end3:

    jr   $ra            # return

.data

numbers:
    .word 0 0 0 0 0 0 0 0 0 0  # int numbers[10] = {0};

