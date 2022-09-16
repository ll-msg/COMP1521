# Read a number n and print the first n tetrahedral numbers
# https://en.wikipedia.org/wiki/Tetrahedral_number

main:                  # int main(void) {

    la   $a0, prompt   # printf("Enter how many: ");
    li   $v0, 4
    syscall

    li   $v0, 5         # scanf("%d", number);
    syscall

    li   $t0, 1         # n = 1
    move $t1, $v0

loop1:
    bgt  $t0, $t1, end
    li   $t2, 0         # total = 0
    li   $t3, 1         # j = 1

loop2:
    bgt  $t3, $t0, end_loop2
    li   $t4, 1         # i = 1

loop3:
    bgt  $t4, $t3, end_loop3
    add  $t2, $t2, $t4
    addi $t4, $t4, 1
    j    loop3

end_loop3:
    addi $t3, $t3, 1
    j    loop2

end_loop2:
    move $a0, $t2
    li   $v0, 1
    syscall
    li   $a0, '\n'      # printf("%c", '\n');
    li   $v0, 11
    syscall
    addi $t0, $t0, 1
    j    loop1

end:
    jr   $ra           # return

    .data
prompt:
    .asciiz "Enter how many: "
