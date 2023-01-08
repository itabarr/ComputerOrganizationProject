	add $s2 ,$zero, $imm, 0x7FFFF       # $s2 = biggest number that can be saved in memory signed bit
	add $s1, $zero, $imm, 4096			# $s1 = 4096, initialize reg value with memory size
	add $t0, $zero, $imm, 0				# $t0 = 0 , initalize reg value with fib0
	add $t1, $zero, $imm, 1				# $t1 = 1 , initalize reg value with fib1
	.word 256 0							# mem[256] = 0 , initalize memory value)
	.word 257 1							# mem[275] = 1 , initalize memory value)
	add $s0, $zero, $imm , 258			# $s0 = 258 , initalize memory adress value)
	beq $imm, $zero, $zero, fib 		# jump to fib label, start doing function 
fib:
	add $t2, $t1, $t0, 0 				# $t2 = $t1 + $t0 , do fib
	bgt $imm, $t2, $s2, exit			# if $t2 > $s2 (0x7FFFF), there is overflow -> exit
	sw  $t2, $s0, $zero, 0 				# mem[$s0] = $t2 , write fib to memory
	add $t0 , $t1, $zero, 0   			# $t0 = $t1 , progress values
	add $t1 , $t2, $zero, 0 			# $t1 = $t2 , progress values
	add $s0, $s0, $imm, 1				# $s0 = $s0 + 1 , progress memory value
	beq $imm, $s0, $s1, exit			# if $s0 == 4096, jump to exit (exceeding last line)
	beq $imm, $zero, $zero, fib 		# jump to fib label, to redo fib
exit:
	halt $zero, $zero, $zero, 0 		# exit programm