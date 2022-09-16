# COMP1521 20T3 final exam Q4 starter code

# This code reads 1 integer and prints it

# Change it to read integers until low is greater or equal to high,
# then print their difference

main:
    li   $t0, 100      # int high = 100;
    li   $t1, 0        # int low = 0;

loop:
    bgt  $t1, $t0, end_loop	# if $t0 > $t1 then target
    
    
    li   $v0, 5        #   scanf("%d", &x);
    syscall

    move $t2, $v0      #   
    
    add	 $t1, $t1, $t2		# low = low + x;
    sub  $t0, $t0, $t2      # high = high - x;

    j	 loop			

end_loop:
    sub  $t3, $t1, $t0      # low - high
    move $a0, $t3
    li   $v0, 1
    syscall
    

    li   $a0, '\n'     #   printf("%c", '\n');
    li   $v0, 11
    syscall

    li   $v0, 0        #   return 0
    jr   $ra
