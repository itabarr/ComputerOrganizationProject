#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY_LEN 50
#define MAX_VALUE_LEN 100
#define MAX_LINE_SIZE 300
#define FILE_SIZE 4096
#define MAX_LABEL_SIZE 30
#define MAX_DIR_LEN 1024

// A node in the directory
typedef struct Node {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
    struct Node* next;
} Node;

// The directory
typedef struct Directory {
    Node* head;
} Directory;

// Struct methods
Node* create_node(char* key, char* value);
Directory* create_directory();
void add_key_value(Directory* directory, char* key, char* value);
char* search_by_key(Directory* directory, char* key);

// Lookup tables
int register_to_int(char* line , int n_register);
int opcode_to_int(char* line);
int get_last_word_address(Directory *words);

// Strings methods
int is_label(char *line);
int is_word(char *line);
int is_imm(char* line);
int is_hex_num(char* str);
void remove_comment(char *line);
void get_label(char *line , char* label);
char* get_n_substring(char *str, int n);
void parse_opcode_line(char *line , char *hexline);
void parse_imm_const(char *imm_const , char *hex_line,Directory* labels );
void int_to_fixed_length_binary(int n, char org_binary_string[], int length);
char four_bit_string_to_hex(char *four_bit_string);
int four_bit_string_to_int(char* str);

// Main methods
void create_label_and_words_directories(char *input_filename, Directory* labels, Directory* words);
void create_memin_file(char *input_filename , char *output_filename , Directory* labels);
void add_words_to_memin(char *input_filename, Directory* words);

// Other methods
int min(int a, int b);
int get_file_len(char *input_filename);

//TODO: Improve big function in the end
//TODO: Handle words


int main(int argc, char* argv[]) {
    char input_filename[MAX_DIR_LEN], output_filename[MAX_DIR_LEN];

    if (argc!=3 && argc!=1){
        printf("Error: Zero or Two arguments are required.\n");
        return 1;
    }

    if (argc == 1){
        strcpy(input_filename,"fib.asm");
        strcpy(output_filename,"memin.txt");
    }

    if (argc == 3){
        strcpy(input_filename,argv[1]);
        strcpy(output_filename,argv[2]);
    }

    remove(output_filename);
    Directory *labels = create_directory();
    Directory *words =  create_directory();
    create_label_and_words_directories(input_filename , labels , words);
    create_memin_file(input_filename , output_filename, labels);
    add_words_to_memin(output_filename, words);

    return 0;
}


// Creates a new node with the given key and value
Node* create_node(char* key, char* value) {
    Node* node = (Node*) malloc(sizeof(Node));
    strcpy(node->key, key);
    strcpy(node->value, value);
    node->next = NULL;
    return node;
}

// Creates a new directory
Directory* create_directory() {
    Directory* directory = (Directory*) malloc(sizeof(Directory));
    directory->head = NULL;
    return directory;
}

// Adds a key-value pair to the directory
void add_key_value(Directory* directory, char* key, char* value) {
    Node* node = create_node(key, value);
    node->next = directory->head;
    directory->head = node;
}

// Searches for a value in the directory by key
// Returns the value if found, or NULL if not found
char* search_by_key(Directory* directory, char* key) {
    Node* current = directory->head;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}

// Do first iteration to get labels directory
void create_label_and_words_directories(char *input_filename , Directory* labels , Directory* words){
    // Open file for reading and writing
    FILE *input = fopen(input_filename, "r");

    char i = 0;
    int j = 0;
    char tmp_key[MAX_KEY_LEN];
    char tmp_value[MAX_VALUE_LEN];
    char line[MAX_LINE_SIZE];
    char *ptmp;
    int hex_to_int;

    while (fgets(line, sizeof line, input)) {
        remove_comment(line);

        if (is_label(line)){
            get_label(line,tmp_key);
            itoa(j,tmp_value,10);
            add_key_value(labels,tmp_key,tmp_value);
            printf("%s %s\n",tmp_key,tmp_value);
        }

        
        else if(is_word(line)){
            ptmp = get_n_substring(line, 1);
            strcpy(tmp_key,ptmp);
        
            hex_to_int = strtol(tmp_key,NULL,0);
            itoa(hex_to_int,tmp_key,10);

            ptmp = get_n_substring(line,2);
            strcpy(tmp_value,ptmp);

            if (!is_hex_num(tmp_value)){
                //imm = atoi(tmp_value);
                hex_to_int = strtol(tmp_value,NULL,0);
                sprintf(tmp_value,"%05X", hex_to_int & 0xFFFFF);
            }

            else{
                strcpy(tmp_value , tmp_value+2);
            }

            add_key_value(words,tmp_key,tmp_value);
            printf("%s %s\n",tmp_key,tmp_value);
        }

        else{

            if (is_imm(line)){
                j = j+2;
            }

            else{
                j++;
            }
        }
        //printf("%d\n", i);
        i++;
    }

}

// Do second iteration and write output file
void create_memin_file(char *input_filename , char *output_filename, Directory* labels){
    remove(output_filename);
    
    // Open file for reading and writing
    FILE *input = fopen(input_filename, "r");
    FILE *output = fopen(output_filename, "w");

    int i = 0;
    char line[MAX_LINE_SIZE];
    char* imm_str;
    char* word_add;
    char* word_val;
    char hexline[6];
    hexline[5] = '\0';

    while (fgets(line, sizeof line, input)) {

        if (!is_label(line) && !is_word(line)){
            parse_opcode_line(line , hexline);
            fprintf(output, "%s\n", hexline);

            if (is_imm(line)){
                imm_str = get_n_substring(line , 4);
                parse_imm_const(imm_str , hexline ,labels);
                fprintf(output, "%s\n", hexline);
                //printf("%s\n" , imm_str);
            }
            
            printf("%s %d\n" , hexline,i);
        }

        
        i++;
    }
    fclose(input);
    fclose(output);

}

//Add words to memin file
void add_words_to_memin(char *input_filename, Directory* words){

    // init variables
    int i = get_file_len(input_filename); 
    int max_address = get_last_word_address(words);
    int last_row = min(FILE_SIZE-1, max_address);
    char key[5]; 
    char *word;

    // Open file for reading and writing
    FILE *input = fopen(input_filename , "a");
    
    while (i <= last_row){
        itoa(i, key, 10);
        word = search_by_key(words, key);

        if (word !=NULL){
            fprintf(input, "%s\n", word);
        }

        else{
            fprintf(input, "00000\n");
        }
            

        i++;
    }

    fclose(input);
    

}

// Check if line is a label line
int is_label(char *line){
    char *c = strchr(line, ':') ;
    if (c != NULL){
        return 1;
    }
    return 0;
}

// Check if line is a word line
int is_word(char *line){
    char* p = get_n_substring(line , 0);
    if (strlen(p) == 4){
        if (!strncmp("word",p,4)){
            return 1;
        }  
    }
    else if (strlen(p) == 5){
        if (!strncmp(".word",p,5)){
            return 1;
        }
        
    }

    return 0;
}

// Check if line is imm
int is_imm(char* line){

    int register_1 , register_2, register_3;
    register_1 = register_to_int(line,1);
    register_2 = register_to_int(line,2);
    register_3 = register_to_int(line,3);

    if (register_1 == 1 || register_2 == 1 || register_3 ==1){
        return 1;
    }

    return 0;
    
}

// Check if imm const is label
int is_imm_label(char* imm_const){
    int i = 0;
    if (imm_const[0] == '-'){
        i++;
    }
    if (imm_const[0+i] < '0' || imm_const[0+i] > '9') {
        return 1;
    }

    return 0;
}

// Check if a number string is in hex format
int is_hex_num(char* str){
    char *px = strchr(str, 'x') ;
    char *pX = strchr(str, 'X') ;

    if (px == NULL && pX == NULL){
        return 0;
    }

    return 1;
}

// Ignore comments by replacing first '#' with '/0' 
void remove_comment(char *line){
    char *comment = strchr(line, '#') ;
    if (strchr(line, '#') != NULL) {
        *comment = '\0'; 
    } 
}

// Set label string as the current label in line 
void get_label(char *line , char* label){
    char *tmp_label;
    int label_size=0;
    int i=0;

    while (i<MAX_LINE_SIZE){
        if (*(line+i) == ' ' || *(line+i) == '\t'){
            i++;
        } 
        else if(*(line+i) == ':'){
            strncpy(label , tmp_label , label_size);
            *(label+label_size) = '\0';
            return;
        }
        else{
            tmp_label = line;
            label_size++;
            i++;
        }
    }
    

}

// Get the n-th substring inside a string (ignoring spaces and tabs) 
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

// Get file length
int get_file_len(char *input_filename){
    FILE *input = fopen(input_filename, "r");

    int len = 0;
    char line[MAX_LINE_SIZE];

    while (fgets(line, sizeof line, input)) {
        len++;
    }

    fclose(input);
    return len;

}

// Get max address of words 
int get_last_word_address(Directory *words){

    Node *word = words ->head;
    int max_row = 0;
    int row;

    while (word !=NULL){

        row = atoi(word -> key);

        if (row > max_row){
            max_row = row;
        }

        word = word -> next;

    }
    return max_row;
}

// Registers lookup table according to the project definition
int register_to_int(char* line, int n_register) {
    char* p = get_n_substring(line , n_register);

    if      (!strcmp("$zero",p)){ return 0; }
    else if (!strcmp("$imm",p)){ return 1; }
    else if (!strcmp("$v0", p)){ return 2; }
    else if (!strcmp("$a0", p)){ return 3; }
    else if (!strcmp("$a1", p)){ return 4; }
    else if (!strcmp("$a2", p)){ return 5; }
    else if (!strcmp("a3", p)){ return 6; }
    else if (!strcmp("$t0", p)){ return 7; }
    else if (!strcmp("$t1", p)){ return 8; }
    else if (!strcmp("$t2", p)){ return 9; }
    else if (!strcmp("$s0", p)){ return 10; }
    else if (!strcmp("$s1", p)){ return 11; }
    else if (!strcmp("$s2", p)){ return 12; }
    else if (!strcmp("$gp", p)){ return 13; }
    else if (!strcmp("$sp", p)){ return 14; }
    else if (!strcmp("$ra", p)){ return 15; }
    else{return -1;}    

}

// Opcodes lookup table according to the project definition
int opcode_to_int(char* line) {
    char* p = get_n_substring(line , 0);

    if      (!strcmp("add",p)){ return 0; }
    else if (!strcmp("sub",p)){ return 1; }
    else if (!strcmp("mul", p)){ return 2; }
    else if (!strcmp("and", p)){ return 3; }
    else if (!strcmp("or", p)){ return 4; }
    else if (!strcmp("xor", p)){ return 5; }
    else if (!strcmp("sll", p)){ return 6; }
    else if (!strcmp("sra", p)){ return 7; }
    else if (!strcmp("srl", p)){ return 8; }
    else if (!strcmp("beq", p)){ return 9; }
    else if (!strcmp("bne", p)){ return 10; }
    else if (!strcmp("blt", p)){ return 11; }
    else if (!strcmp("bgt", p)){ return 12; }
    else if (!strcmp("ble", p)){ return 13; }
    else if (!strcmp("bge", p)){ return 14; }
    else if (!strcmp("jal", p)){ return 15; }
    else if (!strcmp("lw", p)){ return 16; }
    else if (!strcmp("sw", p)){ return 17; }
    else if (!strcmp("reti", p)){ return 18; }
    else if (!strcmp("in", p)){ return 19; }
    else if (!strcmp("out", p)){ return 20; }
    else if (!strcmp("halt", p)){ return 21; }
    else if (!strcmp("word", p)){ return 22; }
    else{return -1;}    

}

// Do main opcode parsing to recieve hex format
void parse_opcode_line(char *line , char *hexline){    
    
    int op = opcode_to_int(line);
    int reg1 = register_to_int(line , 1);
    int reg2 = register_to_int(line,2);
    int reg3 = register_to_int(line,3);

    sprintf(hexline, "%0*X", 2 , op);
    sprintf(hexline+2, "%0*X", 1, reg1);
    sprintf(hexline+3, "%0*X", 1, reg2);
    sprintf(hexline+4, "%0*X", 1, reg3);

}

// Do imm const parsing
void parse_imm_const(char *imm_const , char *hex_line , Directory* labels){
    int i=0;
    char binary_string[21];


    if (is_imm_label(imm_const)){
        char* value = search_by_key(labels , imm_const);
        i = atoi(value);
        sprintf(hex_line, "%0*X", 5 , i);

    }

    else if(is_hex_num(imm_const)){
        strcpy(hex_line , imm_const+2);
    }

    else{
        i = atoi(imm_const);
        int_to_fixed_length_binary(i , binary_string , 20);

        hex_line[0] = four_bit_string_to_hex(binary_string);
        hex_line[1] = four_bit_string_to_hex(binary_string+4);
        hex_line[2] = four_bit_string_to_hex(binary_string+8);
        hex_line[3] = four_bit_string_to_hex(binary_string+12);
        hex_line[4] = four_bit_string_to_hex(binary_string+16);
        hex_line[5] = '\0';
    }
    printf("%d\n", i);
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

char four_bit_string_to_hex(char *four_bit_string) {
    char hex_characters[] = "0123456789ABCDEF";
    int value =  four_bit_string_to_int(four_bit_string);
    return hex_characters[value];
}

int four_bit_string_to_int(char* str) {
  int result = 0;
  for (int i = 0; i < 4; i++) {
    result <<= 1;
    if (str[i] == '1') {
      result |= 1;
    }
  }
  return result;
}

// get min of two ints
int min(int a, int b){
    if (a > b){
        return b;
    }
    return a;
}