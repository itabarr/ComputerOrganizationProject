#include <stdio.h>
#include <string.h>


int opcode_to_binary(char line[]) {
    if      (!strncmp("add",line,3)){ return 0; }
    else if (!strncmp("sub",line,3)){ return 1; }
    else if (!strncmp("mul", line, 3)){ return 2; }
    else if (!strncmp("and", line, 3)){ return 3; }
    else if (!strncmp("or", line, 3)){ return 4; }
    else if (!strncmp("xor", line, 3)){ return 5; }
    else if (!strncmp("sll", line, 3)){ return 6; }
    else if (!strncmp("sra", line, 3)){ return 7; }
    else if (!strncmp("srl", line, 3)){ return 8; }
    else if (!strncmp("beq", line, 3)){ return 9; }
    else if (!strncmp("bne", line, 3)){ return 10; }
    else if (!strncmp("blt", line, 3)){ return 11; }
    else if (!strncmp("bgt", line, 3)){ return 12; }
    else if (!strncmp("ble", line, 3)){ return 13; }
    else if (!strncmp("bge", line, 3)){ return 14; }
    else if (!strncmp("jal", line, 3)){ return 15; }
    else if (!strncmp("lw", line, 2)){ return 16; }
    else if (!strncmp("sw", line, 2)){ return 17; }
    else if (!strncmp("reti", line, 4)){ return 18; }
    else if (!strncmp("in", line, 2)){ return 19; }
    else if (!strncmp("out", line, 3)){ return 20; }
    else if (!strncmp("halt", line, 4)){ return 21; }
    else{return -1;}    

}

int register_to_binary(char line[] , int nRegister) {
    char s[] = " ";
    char tmpstr[20];
    strcpy(tmpstr , line);
    char *p = strtok(tmpstr, s);

    for (int i = 0; i<nRegister; i++ ){
        if (p == NULL) {
            // n-th register does not exist, return error value
            return -1;
        }
        p = strtok(NULL, s);
    }
    

    if      (!strncmp("$zero",p,5)){ return 0; }
    else if (!strncmp("$imm",p,4)){ return 1; }
    else if (!strncmp("$v0", p, 3)){ return 2; }
    else if (!strncmp("$a0", p, 3)){ return 3; }
    else if (!strncmp("$a1", p, 3)){ return 4; }
    else if (!strncmp("$a2", p, 3)){ return 5; }
    else if (!strncmp("a3", p, 3)){ return 6; }
    else if (!strncmp("$t0", p, 3)){ return 7; }
    else if (!strncmp("$t1", p, 3)){ return 8; }
    else if (!strncmp("$t2", p, 3)){ return 9; }
    else if (!strncmp("$s0", p, 3)){ return 10; }
    else if (!strncmp("$s1", p, 3)){ return 11; }
    else if (!strncmp("$s2", p, 3)){ return 12; }
    else if (!strncmp("$gp", p, 3)){ return 13; }
    else if (!strncmp("$sp", p, 3)){ return 14; }
    else if (!strncmp("$ra", p, 3)){ return 15; }
    else{return -1;}    

}


int main() {
    char line[] = "halt $zero $t2 $t1";
    int opcode = opcode_to_binary(line);
    int register_1 = register_to_binary(line, 1);
    int register_2 = register_to_binary(line, 2);
    int register_3 = register_to_binary(line, 3);
    printf("%d%d%d%d", opcode, register_1, register_2, register_3);
    return 0;
}

