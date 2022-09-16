main:
        la $t0, numbers # int *p = &numbers[0]
        addi $t1, $t0, 16 # int *q = &numbers[4]
loop:
        bgt $t0, $t1, end
        lw $a0, 0($t0)  # number 0 is brought into $a0
        li $v0, 1
        syscall
        li $a0, '\n'
        li $v0, 11
        syscall

        addi $t0, $t0, 4
        j loop
end:
        li $v0, 0
        jr $ra

.data
numbers:
        .word 3, 9, 27, 81, 243