#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int opcode_to_binary(char line[]);
int register_to_binary(char line[] , int nRegister) ;
void int_to_fixed_length_binary(int n, int length, char* binary);
void conver_R_format(char *line, char *binaryline);
void get_labels_first_iteration(const char *input_filename ,const char *labels_table_filename);
void read_file_line_by_line(const char *input_filename ,const char *output_filename );
int check_is_imm(char *line);
int is_string_empty(char *str);
void get_label_name(char* line, char *label_name);
int has_label(char* line);


int main(int argc, char* argv[]) {
    char input_file[50], output_file[50];

    if (argc!=3 && argc!=1){
        printf("Error: Zero or Two arguments are required.\n");
        return 1;
    }

    if (argc == 1){
        strcpy(input_file,"example.asm");
        strcpy(output_file,"memin.txt");
    }

    if (argc == 3){
        strcpy(input_file,argv[1]);
        strcpy(output_file,argv[2]);
    }

    get_labels_first_iteration(input_file, "labels.txt");
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
    char tmpstr[100];
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
    while (*line == ' ' || *line == '\t'){
        line = line +1;
    } 
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
    int line_index = 0;
    int is_imm;
    char line[1024];
    char binaryline[21];
    char imm_cmd[21] = "IMM CMND REPLACE";
    int colon_index;
    while (fgets(line, sizeof line, input)) {
        if (is_string_empty(line)){
            continue;
        }

        colon_index = -1;
        if (has_label(line)){
            char *colon_ptr;
            colon_ptr = strchr(line, ':');
            colon_index = colon_ptr - line;
        }
        
        is_imm = check_is_imm(line);
        printf("is imm= %d\n", is_imm);
        conver_R_format(line + colon_index + 1, binaryline);
        fprintf(output, "%s\n", binaryline);
        if (is_imm){
            fprintf(output, "%s\n", imm_cmd);
        } 

        // Print the line to the screen
        printf("%s\n", binaryline);
        printf("%s", line + colon_index + 1);
        
    }

    // Close the file
    fclose(input);
    fclose(output);
}

void get_labels_first_iteration(const char *input_filename ,const char *labels_table_filename )
{
    // Open file for reading and writing
    FILE *input = fopen(input_filename, "r");
    FILE *output = fopen(labels_table_filename, "w");

    // Check for successful file open
    if (input == NULL) {
        printf("Error: Unable to open file %s\n", input_filename);
        return;
    }

    // Read the file line by line
    int line_index = 0;
    char line[1024];
    char binaryline[21];
    char imm_cmd[21] = "00000000000000000000";
    char label[10];
    int label_len;
    char label_line[50];
    char index_string[13];
    int index_string_len;

    while (fgets(line, sizeof line, input)) {
        line_index++;
        if (!has_label(line)){
            continue;
        }
        get_label_name(line, label);
        label_len = strlen(label);
        int_to_fixed_length_binary(line_index-1,12,index_string);
        index_string_len = strlen(index_string);
        
        strcpy(label_line, index_string);
        label_line[index_string_len] = ' ';
        strcpy(label_line + index_string_len+1, label);
        label_line[index_string_len + label_len+1] = '\0';

        fprintf (output, "%s\n", label_line);
        // Print the label to the screen
        printf("%s\n", label);
        
    }

    // Close the file
    fclose(input);
    fclose(output);
}

int is_string_empty(char *str) {
    int i=0;

    while (str[i] != '\n') {
        if (str[i] != ' ' && str[i] != '\t') {
            return 0;
        }
        i++;
    }

    return 1;
}

int check_is_imm(char *line){
    char s[] = " ";
    char tmpstr[100];
    strcpy(tmpstr , line);
    char *p = strtok(tmpstr, s);

    for (int i = 0; i<4; i++ ){
        p = strtok(NULL, s);
    }

    p = strtok(p, "\n");
    if (!strcmp(p , "0")){
        return 0;
    }
    return 1;
}

int has_label(char* line){
    char *colon_ptr;
    colon_ptr = strchr(line, ':');

    if (colon_ptr == NULL){
        return 0;
    }

    return 1;

}
void get_label_name(char* line, char *label_name){

    char *colon_ptr;
    colon_ptr = strchr(line, ':');
    int colon_index = colon_ptr - line;
    strncpy(label_name, line, colon_index);
    label_name[colon_index] = '\0';
    
}

char* get_binary_string(const char* filename, const char* name) {
  // Open the file for reading
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    // Return NULL if the file could not be opened
    return NULL;
  }

  // Allocate memory for the binary string
  char* binary_string = malloc(11);

  // Read each line of the file until the end is reached or the name is found
  char line[128];
  while (fgets(line, sizeof(line), file) != NULL) {
    // Parse the name and binary string from the line
    char parsed_name[128];
    char parsed_binary_string[128];
    if (sscanf(line, "%s %s", parsed_binary_string, parsed_name) == 2) {
      // Check if the parsed name matches the given name
      if (strcmp(parsed_name, name) == 0) {
        // If the names match, copy the parsed binary string to the result and break out of the loop
        strcpy(binary_string, parsed_binary_string);
        break;
      }
    }
  }

  // Close the file
  fclose(file);

  // Return the binary string
  return binary_string;
}

