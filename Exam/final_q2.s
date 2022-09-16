# COMP1521 21T2 ... final exam, question 2

	.data
even_parity_str:	.asciiz "the parity is even\n"
odd_parity_str:		.asciiz "the parity is odd\n"

	.text
main:
	li	$v0, 5
	syscall
	move	$t0, $v0
	# input is in $t0

	# TODO
	li	$t1, 0		# int bit_idx = 0;
	li	$t2, 0		# int n_bits_set = 0;

loop:
	beq		$t1, 32, end_loop	# while (bit_idx != 32) {

	srl		$t3, $t0, $t1
	and		$t3, $t3, 1
	
	add		$t2, $t2, $t3	# n_bits_set = n_bits_set + bit;
	addi	$t1, $t1, 1		# bit_idx++;
	
	j		loop			# }
	
end_loop:
	li		$t5, 2
	div		$t2, $t5
	mfhi	$t4				# n_bits_set % 2

	beq		$t4, 0, print_even
	li		$v0, 4
	la		$a0, odd_parity_str
	syscall

	li		$v0, 0
	jr		$ra

print_even:
	li	$v0, 4
	la	$a0, even_parity_str
	syscall

	li	$v0, 0
	jr	$ra
