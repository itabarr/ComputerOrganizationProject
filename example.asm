bne $imm, $t0, $t1, ITAMAR1
    bne $imm, $t0, $t1, ITAMAR2                 
beq $imm, $zero, $zero, 0
    bne $imm, $t0, $t1, 0
    
L2:beq $imm, $zero, $zero, L2
add $t1, $zero, $imm, L3
beq $t1, $zero, $zero, 0
jal $ra, $imm, $zero, L4
halt $zero, $zero, $zero, 0


ITAMAR:halt $zero, $zero, $zero, 0


