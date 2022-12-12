#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int opcode_to_binary(char line[]);
int register_to_binary(char line[] , int nRegister) ;
void int_to_fixed_length_binary(int n, int length, char* binary);
void conver_R_format(char *line, char *binaryline);
void read_file_line_by_line(const char *input_filename ,const char *output_filename );
int check_is_imm(char *line);

int main() {
    char input_file[] = "square.asm";
    char output_file[] = "memin.txt";
    read_file_line_by_line(input_file , output_file);

    return 0;
}

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

void int_to_fixed_length_binary(int n, int length, char* binary) {
    // Make sure the buffer is large enough
    if (length < 1) {
        printf("Error: length must be at least 1\n");
        return;
    }

    // Initialize the buffer with zeros
    for (int i = 0; i < length; i++) {
        binary[i] = '0';
    }
    binary[length] = '\0';

    // Convert the integer to binary and store it in the buffer
    int i = 0;
    while (n > 0 && i < length) {
        binary[length - i - 1] = (n % 2) + '0';
        n = n / 2;
        i++;
    }

    binary[length] = '\0';
}

void conver_R_format(char *line, char *binaryline){
    int opcode = opcode_to_binary(line);
    int register_1 = register_to_binary(line, 1);
    int register_2 = register_to_binary(line, 2);
    int register_3 = register_to_binary(line, 3);

    char opcode_binary[8+1];
    char register_1_binary[4+1];
    char register_2_binary[4+1];
    char register_3_binary[4+1];
    int_to_fixed_length_binary(opcode,8,opcode_binary);
    int_to_fixed_length_binary(register_1,4, register_1_binary);
    int_to_fixed_length_binary(register_2,4, register_2_binary);
    int_to_fixed_length_binary(register_2,4, register_3_binary);
    //printf("%s %s %s %s", opcode_binary, register_1_binary, register_2_binary, register_2_binary);

    strcpy(binaryline, opcode_binary);
    strcpy(binaryline+8, register_1_binary);
    strcpy(binaryline+12, register_2_binary);
    strcpy(binaryline+16, register_3_binary);
}

void read_file_line_by_line(const char *input_filename ,const char *output_filename )
{
    // Open file for reading and writing
    FILE *input = fopen(input_filename, "r");
    FILE *output = fopen(output_filename, "w");

    // Check for successful file open
    if (input == NULL) {
        printf("Error: Unable to open file %s\n", input_filename);
        return;
    }

    // Read the file line by line
    char line[1024];
    char binaryline[21];
    char imm_cmd[21] = "00000000000000000000";

    while (fgets(line, sizeof line, input)) {
        int is_imm = check_is_imm(line);
        
        conver_R_format(line, binaryline);
        
        fprintf (output, "%s\n", binaryline);
        if (is_imm == 1){
            fprintf (output, "%s\n", imm_cmd);
        }
        // Print the line to the screen
        printf("%s\n", binaryline);
        printf("%s", line);
        
    }

    // Close the file
    fclose(input);
    fclose(output);
}

int check_is_imm(char *line){
    char s[] = " ";
    char tmp[20];
    strcpy(tmp, line);
    char *p = strtok(tmp, s);

    char c = p[strlen(p)-1];
    if (c == 'i'){
        return 1;
    }

    return 0;
}