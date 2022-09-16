# Read a number and print positive multiples of 7 or 11 < n

main:                  # int main(void) {

    la   $a0, prompt   # printf("Enter a number: ");
    li   $v0, 4
    syscall

    li   $v0, 5         # scanf("%d", number);
    syscall

    li   $t0, 1         # i = 1;
    move $t5, $v0
    li   $t3, 7
    li   $t4, 11

loop:
    bge  $t0, $t5, end
    
    rem  $t1, $t0, $t3
    rem  $t2, $t0, $t4

    beq  $t1, 0, else
    beq  $t2, 0, else

    addi $t0, $t0, 1
    j    loop

else:
        move $a0, $t0
        li   $v0, 1
        syscall
        li   $a0, '\n'      # printf("%c", '\n');
        li   $v0, 11
        syscall
        addi $t0, $t0, 1
        j    loop

end:
    jr   $ra           # return

    .data
prompt:
    .asciiz "Enter a number: "
