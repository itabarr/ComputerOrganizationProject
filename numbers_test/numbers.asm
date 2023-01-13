add $t0, $imm, $imm, 1              #line 1 should be 00001
add $t0, $imm, $imm, -1             #line 3 should be FFFFF
add $t0, $imm, $imm, 0x0            #line 5 should be 00000
add $t0, $imm, $imm, 0xFFFFF        #line 7 should be FFFFF
add $t0, $imm, $imm, 0xFFFF         #line 9 should be 0FFFF   
.word 99 0                          #line 99 should be 00000  (0)
.word 100 1                         #line 100 should be 00001 (1)
.word 101 -1                        #line 101 should be FFFFF (-11)
.word 102 23455                     #line 102 should be 05B9F (23455)
.word 103 -20077                    #line 104 should be FB193 (-20077)
.word 104 0x0                       #line 104 should be 00000
.word 105 0x10                      #line 105 should be 00010
.word 0x6A -1                       #line 106 (0006A) should be FFFFF (-1)
.word 0x0006B 1                     #line 107 (0006B) should be 00001 (1)
halt $zero, $zero, $zero, 0