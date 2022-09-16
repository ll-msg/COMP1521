# read a line and print whether it is a palindrom

main:
    la   $a0, str0       # printf("Enter a line of input: ");
    li   $v0, 4
    syscall

    la   $a0, line
    la   $a1, 256
    li   $v0, 8          # fgets(buffer, 256, stdin)
    syscall              #

    li   $t0, 0
main_loop:
    la   $t1, line
    add  $t1, $t1, $t0
    lb   $t2, ($t1)

    beq  $t2, 0, end_loop
    addi $t0, $t0, 1
    
    j    main_loop

end_loop:
    li   $t3, 0             # j = 0
    addi $t4, $t0, -2       # k = i -2

main_loop_2:
    bge  $t3, $t4, end_loop_2

main_if:
    la   $t2, line
    add  $t5, $t3, $t2
    add  $t6, $t4, $t2

    lb   $t7, ($t5)
    lb   $t8, ($t6)

    beq  $t7, $t8, end_if

    la   $a0, not_palindrome
    li   $v0, 4
    syscall

    li   $v0, 0          # return 0
    jr   $ra

end_if:
    addi $t3, $t3, 1
    addi $t4, $t4, -1

    j    main_loop_2

end_loop_2:
    la   $a0, palindrome
    li   $v0, 4
    syscall

    li   $v0, 0          # return 0
    jr   $ra

.data
str0:
    .asciiz "Enter a line of input: "
palindrome:
    .asciiz "palindrome\n"
not_palindrome:
    .asciiz "not palindrome\n"


# line of input stored here
line:
    .space 256

