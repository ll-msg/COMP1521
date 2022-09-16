# Sieve of Eratosthenes
# https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
main:

    li $t0, 0           # int i = 0

loop1:
    bge $t0, 1000, end_loop1
    
    la  $t1, prime
    add $t2, $t0, $t1
    li  $t3, 1
    sb  $t3, ($t2)

    addi $t0, $t0, 1    # i++;
    j   loop1

end_loop1:
    li  $t0, 2          # i = 2

loop2:
    bge $t0, 1000, end_loop2

    la  $t1, prime
    add $t2, $t0, $t1
    lb  $t3, ($t2)

    bne $t3, 1, end_if
    move  $a0, $t0
    li  $v0, 1
    syscall

    li   $a0, '\n' 
    li   $v0, 11
    syscall

    mul $t4, $t0, 2     # int j = 2 * i

loop3:
    bge $t4, 1000, end_loop3

    la  $t1, prime
    add $t2, $t4, $t1
    li  $t1, 0
    sb  $t1, ($t2)

    add $t4, $t4, $t0
    j   loop3

end_loop3:
    addi $t0, $t0, 1
    j   loop2

end_if:
    addi $t0, $t0, 1
    j   loop2

end_loop2:

    li $v0, 0           # return 0
    jr $ra

.data
prime:
    .space 1000