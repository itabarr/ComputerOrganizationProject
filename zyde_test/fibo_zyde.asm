.word 0x100 0x0								#mem[256]=0
.word 257 1									#mem[257]=1
add $t0, $zero, $zero, 0					# fib = 0
add $a0, $zero, $imm, 256					#addr = 256
lw $t1, $a0, $zero, 0						#first = 0
lw $t2, $a0, $imm, 1						#second = 1
add $s0, $zero, $imm, 4096					#stop = 4096
add $s1, $zero, $imm, 524287				#overflow=2^19-1
add $a0, $a0, $imm, 1						#addr=257
LOOP:
bgt $imm, $a0, $s0, END						#if(addr>stop) break
add $t0, $t1, $t2, 0						#fib = first+second
bgt $imm, $t0, $s1, END						#if(fib>overflow) break
add $a0, $a0, $imm, 1						#addr++
sw $t0, $a0, $zero, 0						#mem[addr]=fib
add $t1, $t2, $zero, 0						#first=second
add $t2, $t0, $imm, 0					    #second=fib
jal $ra, $imm, $zero, LOOP				#jump to loop
END:
halt $zero, $zero, $zero, 0 		# exit programm
