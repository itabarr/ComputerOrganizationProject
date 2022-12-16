#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_SIZE 300
#define FILE_SIZE 4096
#define MAX_LABEL_SIZE 30

int _is_label(char* line);
int _is_word(char* line);
int _is_imm(char* line);
char* get_label_name(char* line);
void first_iteration_get_labels(char *input_filename ,char *labels_table_filename);
void second_iteration_get_opcodes(char *input_filename , char *output_filename);
int opcode_to_int(char line[]);
int register_to_int(char line[] , int n_register);
char* remove_empty_chars(char *line);
char* get_n_substring(char *str, int n);
void get_imm_const(char line[] , char imm_cost[]);
void remove_comment(char *line);
void int_to_fixed_length_binary(int n, char org_binary_string[], int length);
char four_bit_string_to_hex(char four_bit_string[]);
int get_label_by_name(char* label_name, char* filename);
void binary_string_to_hex(char binary_string[21] , char hex_string[6] );

int main(int argc, char* argv[]) {

    char input_filename[] = "fib.asm";
    char labels_filename[] = "labels.txt";
    char output_filename[] = "memin.txt";

    first_iteration_get_labels(input_filename , labels_filename);
    second_iteration_get_opcodes(input_filename , output_filename);
    return 0;
}

void first_iteration_get_labels(char *input_filename ,char *labels_filename){
    
    remove(labels_filename);
    
    // Open file for reading and writing
    FILE *input = fopen(input_filename, "r");
    FILE *output = fopen(labels_filename, "w");

    // Check for successful file open
    if (input == NULL) {
        printf("Error: Unable to open file %s\n", input_filename);
        return;
    }

    // Read the file line by line
    int line_index = 0;
    char line_index_str[5];
    char tmpline[MAX_LINE_SIZE];
    char line[MAX_LINE_SIZE];
    char *label;
    char outline[MAX_LABEL_SIZE + 10];
    
    while (fgets(tmpline, sizeof tmpline, input)) {
        strcpy(line, tmpline);
        remove_comment(line);

        if (_is_label(line)){

            label = get_label_name(line);
            itoa(line_index , line_index_str , 10);

            strcpy(outline , label);
            strcpy(outline + strlen(label)+1 ,line_index_str);
            outline[strlen(label)] = ' ';
            outline[strlen(label) + 1 + strlen(line_index_str)] = '\0';
            

            //Update label in file + Print the label to the screen
            fprintf (output, "%s\n", outline);
            printf("%s\n", outline);
        }  

        line_index++;
    }
    fclose(input);
    fclose(output);
    free(label);
}

void second_iteration_get_opcodes(char *input_filename , char *output_filename){
    
    remove(output_filename);
    
    // Open file for reading and writing
    FILE *input = fopen(input_filename, "r");
    FILE *output = fopen(output_filename, "w");

    // Check for successful file open
    if (input == NULL) {
        printf("Error: Unable to open file %s\n", input_filename);
        return;
    }

    // Read the file line by line
    int line_index = 0;
    char tmpline[MAX_LINE_SIZE+1];
    char line[MAX_LINE_SIZE];
    char imm_const[MAX_LABEL_SIZE];
    char hex_opcode[6];

    int opcode;
    int register_1;
    int register_2;
    int register_3;
    int imm;
    

    char binary_opecode_1[9];
    char binary_opecode_2[5];
    char binary_register_1[5];
    char binary_register_2[5];
    char binary_register_3[5];
    char imm_opcode[21];

    while (fgets(tmpline, sizeof tmpline, input)) {
        strcpy(line, tmpline);
        remove_comment(line);
        //str_replace(tmpline, "#", "\0");

        if (!_is_label(line) && !_is_word(line)){
            opcode = opcode_to_int(line);
            int_to_fixed_length_binary(opcode ,binary_opecode_1, 8);
            strncpy(binary_opecode_2 , binary_opecode_1 + 4 , 5);
            binary_opecode_1[4] = '\0';


            register_1 = register_to_int(line,1);
            int_to_fixed_length_binary(register_1 ,binary_register_1, 4);
            register_2 = register_to_int(line,2);
            int_to_fixed_length_binary(register_2 ,binary_register_2, 4);
            register_3 = register_to_int(line,3);
            int_to_fixed_length_binary(register_3 ,binary_register_3, 4);
            
            
            hex_opcode[0] = four_bit_string_to_hex(binary_opecode_1);
            hex_opcode[1] = four_bit_string_to_hex(binary_opecode_2);
            hex_opcode[2] = four_bit_string_to_hex(binary_register_1);
            hex_opcode[3] = four_bit_string_to_hex(binary_register_2);
            hex_opcode[4] = four_bit_string_to_hex(binary_register_3);
            hex_opcode[5] = '\0';

            //Update label in file + Print the label to the screen
            fprintf (output, "%s\n", hex_opcode);
            printf("OPCODE , %d ,%s\n" , line_index , hex_opcode);
            if (register_1 == 1 || register_2 == 1 || register_3 ==1){
                get_imm_const(line , imm_const);
                imm = get_label_by_name(imm_const , "labels.txt");

                if (imm == -1){
                    int_to_fixed_length_binary((atoi(imm_const)),imm_opcode,20);
                    imm_opcode[20] = '\0';
                    binary_string_to_hex(imm_opcode , hex_opcode);
                }

                else{
                    int_to_fixed_length_binary(imm_const,imm_opcode,20);
                    imm_opcode[20] = '\0'; 
                    binary_string_to_hex(imm_opcode , hex_opcode); 
                }
                fprintf(output, "%s\n" ,hex_opcode);
                
            }
        }  
        else if(_is_word(line)){
            fprintf(output, "WORD\n");
            printf("WORD , %d\n" , line_index);
        }
        else if(_is_label(line)){
            //fprintf (output, "LABEL\n");
            printf("LABEL , %d\n" , line_index);
        }

        line_index++;
        
    }

    
    
}

char *get_label_name(char* line){

    // Allocate memory for label
    char *label = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
    
    // Get where the label ends with the ":" char
    char *colon_ptr;
    colon_ptr = strstr(line, ":");
    int colon_index = colon_ptr - line;

    // Copy only the label name to the memory and place "\0" in the end
    strncpy(label, line, colon_index);
    label[colon_index] = '\0';

    // Free unused memory
    free(label + colon_index+1);

    return label;
    
}

void get_imm_const(char line[] , char imm_cost[]){
    
    char *tmp_imm_const;
    tmp_imm_const = get_n_substring(line , 4);
    strcpy(imm_cost , tmp_imm_const);
    

}

int _is_label(char* line){
    

    char* colon_ptr;
    colon_ptr = strstr(line, ":");

    if (colon_ptr == NULL){
        return 0;
    }

    return 1;

}

int _is_word(char* line){
    char* p = get_n_substring(line , 0);
    
    if (strlen(p) == 4){
        if (!strncmp("word",p,4)){
            return 1;
        }
        
    }
     if (strlen(p) == 5){
        if (!strncmp(".word",p,5)){
            return 1;
        }
        
    }

    return 0;

}

int _is_imm_old(char* line){
    char* p = get_n_substring(line , 4);

    if (!strncmp("0",p,4)){
        return 0;
    } 

    return 1;
}

int opcode_to_int(char line[]) {
    char* p = get_n_substring(line , 0);

    if      (!strncmp("add",p,3)){ return 0; }
    else if (!strncmp("sub",p,3)){ return 1; }
    else if (!strncmp("mul", p, 3)){ return 2; }
    else if (!strncmp("and", p, 3)){ return 3; }
    else if (!strncmp("or", p, 3)){ return 4; }
    else if (!strncmp("xor", p, 3)){ return 5; }
    else if (!strncmp("sll", p, 3)){ return 6; }
    else if (!strncmp("sra", p, 3)){ return 7; }
    else if (!strncmp("srl", p, 3)){ return 8; }
    else if (!strncmp("beq", p, 3)){ return 9; }
    else if (!strncmp("bne", p, 3)){ return 10; }
    else if (!strncmp("blt", p, 3)){ return 11; }
    else if (!strncmp("bgt", p, 3)){ return 12; }
    else if (!strncmp("ble", p, 3)){ return 13; }
    else if (!strncmp("bge", p, 3)){ return 14; }
    else if (!strncmp("jal", p, 3)){ return 15; }
    else if (!strncmp("lw", p, 2)){ return 16; }
    else if (!strncmp("sw", p, 2)){ return 17; }
    else if (!strncmp("reti", p, 4)){ return 18; }
    else if (!strncmp("in", p, 2)){ return 19; }
    else if (!strncmp("out", p, 3)){ return 20; }
    else if (!strncmp("halt", p, 4)){ return 21; }
    else if (!strncmp("word", p, 4)){ return 22; }
    else{return -1;}    

}

int register_to_int(char line[] , int n_register) {
    char* p = get_n_substring(line , n_register);

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

char *remove_empty_chars(char *line){
    int i = 0;
    while(line[i] == ' ' || line[i] == '\t'){
        i++;
    }
    
    return line + i ;
}


char* get_n_substring(char *str, int n) {
    char tmpstr[MAX_LINE_SIZE];
    strcpy(tmpstr , str);
    
    char delim[] = " \t,\n"; 
    char *p = strtok(tmpstr, delim);  // Get first token
    int i = 0;
    while (p != NULL && i < n) {  // Continue until end of string or n substrings retrieved
        p = strtok(NULL, delim);  // Get next token
        i++;
    }
    return p;
}

void remove_comment(char *str){
    int i = 0;
    while (i < MAX_LINE_SIZE){
        if  ('#' == *(str+i)){
            *(str+i) = '\0';
            break;
        }

        if ('\n' == *(str+i)){
            return;
        }
        i++;
        
    }

}

void int_to_fixed_length_binary_old(int n, char org_binary_string[], int length) {
    char binary_string[MAX_LINE_SIZE];

    if (length < 1) {
        printf("Error: length must be at least 1\n");
        
    }

    // Initialize the buffer with zeros
    for (int i = 0; i < length; i++) {
        *(binary_string+i) = '0';
    }
    
    // Convert the integer to binary and store it in the buffer
    int i = 0;
    while (n > 0 && i < length) {
        *(binary_string +length - i - 1) = (n % 2) + '0';
        n = n / 2;
        i++;
    }

    *(binary_string+length) = '\0';

    strncpy(org_binary_string , binary_string ,length+1);
    
}

void int_to_fixed_length_binary(int n, char org_binary_string[], int length) {
    char binary_string[MAX_LINE_SIZE];
    int is_negative = 0;

    if (length < 1) {
        printf("Error: length must be at least 1\n");
        
    }

    // Initialize the buffer with zeros
    for (int i = 0; i < length; i++) {
        *(binary_string+i) = '0';
    }

    // Check if the number is negative and set the flag accordingly
    if (n < 0) {
        is_negative = 1;
        n = -n;
    }

    // Convert the integer to binary and store it in the buffer
    int i = 0;
    while (n > 0 && i < length) {
        *(binary_string +length - i - 1) = (n % 2) + '0';
        n = n / 2;
        i++;
    }

    // If the number is negative, invert the bits and add 1 to get the two's complement representation
    if (is_negative) {
        for (int i = 0; i < length; i++) {
            if (*(binary_string+i) == '0') {
                *(binary_string+i) = '1';
            } else {
                *(binary_string+i) = '0';
            }
        }
        
        // Add 1 to the binary representation to get the two's complement
        int carry = 1;
        for (int i = length - 1; i >= 0; i--) {
            if (*(binary_string+i) == '1' && carry == 1) {
                *(binary_string+i) = '0';
            } else if (*(binary_string+i) == '0' && carry == 1) {
                *(binary_string+i) = '1';
                carry = 0;
            }
        }
    }

    *(binary_string+length) = '\0';

    strncpy(org_binary_string , binary_string ,length+1);
}

char four_bit_string_to_hex(char four_bit_string[]) {
    char hex_characters[] = "0123456789ABCDEF";
    int value = strtol(four_bit_string, NULL, 2);
    return hex_characters[value];
}



int get_label_by_name(char* label_name, char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return -1;  // Return -1 if the file could not be opened
    }

    int index;
    char line[MAX_LABEL_SIZE + 10];
    char tmpline[MAX_LABEL_SIZE + 10];
    char *lookuplabel;
    char *lookupindex;

    while (fgets(line, sizeof line , file)) {
        // Split the line into a name and a number
        strcpy(tmpline , line);

        lookuplabel = get_n_substring(tmpline , 0);
        lookupindex = get_n_substring(tmpline , 1);

        int i = 0;
        while(i < strlen(tmpline)){
            if (tmpline[i] == ' '){
                tmpline[i] = '\0';
            }

            i++;
        }
        index = atoi(lookupindex);

        if (strcmp(label_name, lookuplabel) == 0) {
        // Return the number if the name is found
            fclose(file);
            return index;
        }
    }

    fclose(file);
    return -1;  // Return -1 if the name is not found
}


void binary_string_to_hex(char binary_string[21] , char hex_string[6] ){

    char part1[5];
    part1[4] = '\0'; 
    char part2[5];
    part2[4] = '\0';
    char part3[5];
    part3[4] = '\0';
    char part4[5];
    part4[4] = '\0';
    char part5[5];
    part5[4] = '\0';

    strncpy(part1, binary_string,4);
    strncpy(part2, binary_string+4,4);
    strncpy(part3, binary_string+8,4);
    strncpy(part4, binary_string+12,4);
    strncpy(part5, binary_string+16,4);

    hex_string[0] = four_bit_string_to_hex(part1);
    hex_string[1] = four_bit_string_to_hex(part2);
    hex_string[2] = four_bit_string_to_hex(part3);
    hex_string[3] = four_bit_string_to_hex(part4);
    hex_string[4] = four_bit_string_to_hex(part5);
    hex_string[5] = '\0';

}