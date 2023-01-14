#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY_LEN 50          // Max key len size (worst is MAX_LABEL_SIZE) 
#define MAX_VALUE_LEN 8         // Max dictionary value size (worst is '0X00000') 
#define MAX_LINE_SIZE 300       // Max memin file line size (as defined in question)
#define FILE_SIZE 4096          // Max memory length (as defined in question)
#define MAX_LABEL_SIZE 50       // Max label name size (as defined in question)
#define MAX_DIR_LEN 1024        // Max names of the input files

// A node in the dictionary
typedef struct Node {
	char key[MAX_KEY_LEN];
	char value[MAX_VALUE_LEN];
	struct Node* next;
} Node;

// The dictionary
typedef struct Dictionary {
	Node* head;
} Dictionary;

// Struct methods
Node* create_node(char* key, char* value);
Dictionary* create_dictionary();
void add_key_value(Dictionary* dictionary, char* key, char* value);
char* search_by_key(Dictionary* dictionary, char* key);
int get_last_word_address(Dictionary *words);

// Registers and Opcodes converter
int register_to_int(char* line, int n_register);
int opcode_to_int(char* line);

// Strings methods
int is_label(char *line);
int is_word(char *line);
int is_imm(char* line);
int is_empty_line(const char *line);
int is_imm_label(const char *imm_const);
void get_label(char *line, char* label);
char* get_n_substring(char *str, int n);
void parse_opcode_line(char *line, char *hexline);
void parse_imm_const(char *imm_const, char *hex_line, Dictionary* labels);

// Main methods
void create_label_and_words_dictionaries(char *input_filename, Dictionary* labels, Dictionary* words);
void create_memin_file(char *input_filename, char *output_filename, Dictionary* labels);
void add_words_to_memin(char *input_filename, Dictionary* words);

// Other methods
int _min(int a, int b);
int get_file_len(char *input_filename);


// Main function
int main(int argc, char* argv[]) {
	char input_filename[MAX_DIR_LEN], output_filename[MAX_DIR_LEN];

	if (argc != 3 && argc != 1) {
		printf("Error: Zero or Two arguments are required.\n");
		return 1;
	}

	if (argc == 1) {
		strcpy(input_filename, "fib.asm");
		strcpy(output_filename, "memin.txt");
	}

	if (argc == 3) {
		strcpy(input_filename, argv[1]);
		strcpy(output_filename, argv[2]);
	}

	remove(output_filename);
	Dictionary *labels = create_dictionary();
	Dictionary *words = create_dictionary();
	create_label_and_words_dictionaries(input_filename, labels, words);
	create_memin_file(input_filename, output_filename, labels);
	add_words_to_memin(output_filename, words);

	return 0;
}


// Creates a new node with the given key and value
Node* create_node(char* key, char* value) {
	Node* node = (Node*)malloc(sizeof(Node));
	strcpy(node->key, key);
	strcpy(node->value, value);
	node->next = NULL;
	return node;
}

// Creates a new dictionary
Dictionary* create_dictionary() {
	Dictionary* dictionary = (Dictionary*)malloc(sizeof(Dictionary));
	dictionary->head = NULL;
	return dictionary;
}

// Adds a key-value pair to the dictionary
void add_key_value(Dictionary* dictionary, char* key, char* value) {
	//printf("%s\n" , value);
	//printf("%s %s\n", key, value);
	Node* node = create_node(key, value);
	node->next = dictionary->head;
	dictionary->head = node;
}

// Searches for a value in the dictionary by key
// Returns the value if found, or NULL if not found
char* search_by_key(Dictionary* dictionary, char* key) {
	Node* current = dictionary->head;
	while (current != NULL) {
		if (strcmp(current->key, key) == 0) {
			return current->value;
		}
		current = current->next;
	}
	return NULL;
}

// Do first iteration to get labels dictionary
void create_label_and_words_dictionaries(char *input_filename, Dictionary* labels, Dictionary* words) {
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
		//printf("%s\n", line);
		if (is_empty_line(line)) {
			i++;
			continue;
		}

		if (is_label(line)) {
			get_label(line, tmp_key);
			_itoa(j, tmp_value, 10);
			add_key_value(labels, tmp_key, tmp_value);
		}


		else if (is_word(line)) {
			ptmp = get_n_substring(line, 1);
			strcpy(tmp_key, ptmp);

			hex_to_int = strtol(tmp_key, NULL, 0);
			_itoa(hex_to_int, tmp_key, 10);

			ptmp = get_n_substring(line, 2);
			strcpy(tmp_value, ptmp);

			hex_to_int = strtol(tmp_value, NULL, 0);
			sprintf(tmp_value, "%05X", hex_to_int & 0xFFFFF);

			add_key_value(words, tmp_key, tmp_value);
		}

		else {

			if (is_imm(line)) {
				j = j + 2;
			}

			else {
				j++;
			}
		}
		//printf("%d\n", i);
		i++;
	}

}

// Do second iteration and write output file
void create_memin_file(char *input_filename, char *output_filename, Dictionary* labels) {
	remove(output_filename);

	// Open file for reading and writing
	FILE *input = fopen(input_filename, "r");
	FILE *output = fopen(output_filename, "w");

	int i = 0;
	char line[MAX_LINE_SIZE + 1];
	char* imm_str;
	char tmp_imm_str[MAX_LINE_SIZE + 1];
	char hexline[6];
	hexline[5] = '\0';

	while (fgets(line, sizeof line, input)) {
		//printf("%s\n", line);

		if (is_empty_line(line)) {
			i++;
			continue;
		}
		if (!is_label(line) && !is_word(line)) {
			parse_opcode_line(line, hexline);
			fprintf(output, "%s\n", hexline);

			if (is_imm(line)) {
				imm_str = get_n_substring(line, 4);
				strcpy(tmp_imm_str, imm_str);
				parse_imm_const(tmp_imm_str, hexline, labels);
				fprintf(output, "%s\n", hexline);
				//printf("%s\n" , imm_str);
			}
		}
		i++;
		//printf("%d" , i);
	}
	fclose(input);
	fclose(output);

}

//Add words to memin file
void add_words_to_memin(char *input_filename, Dictionary* words) {

	// init variables
	int i = get_file_len(input_filename);
	int max_address = get_last_word_address(words);
	int last_row = _min(FILE_SIZE - 1, max_address);
	char key[5];
	char *word;

	// Open file for reading and writing
	FILE *input = fopen(input_filename, "a");

	while (i < FILE_SIZE) {
		_itoa(i, key, 10);
		word = search_by_key(words, key);

		if (word != NULL) {
			fprintf(input, "%s\n", word);
		}

		else {
			fprintf(input, "00000\n");
		}


		i++;
	}

	fclose(input);

}

// checks if line is empty
int is_empty_line(const char *line) {
	for (int i = 0; (line[i] != '\0') && (line[i] != '#') && (line[i] != '\n'); i++) {
		if (line[i] != ' ' && line[i] != '\t') {
			return 0;
		}
	}
	return 1;
}

// Check if line is a label line
int is_label(char *line) {
	char *c = strchr(line, ':');
	if (c != NULL) {
		return 1;
	}
	return 0;
}

// Check if line is a word line
int is_word(char *line) {
	char* p = get_n_substring(line, 0);
	if (strlen(p) == 4) {
		if (!strncmp("word", p, 4)) {
			return 1;
		}
	}
	else if (strlen(p) == 5) {
		if (!strncmp(".word", p, 5)) {
			return 1;
		}

	}

	return 0;
}

// Check if line is imm
int is_imm(char* line) {

	int register_1, register_2, register_3;
	register_1 = register_to_int(line, 1);
	register_2 = register_to_int(line, 2);
	register_3 = register_to_int(line, 3);

	if (register_1 == 1 || register_2 == 1 || register_3 == 1) {
		return 1;
	}

	return 0;

}

// Check if imm const is label
int is_imm_label(const char *imm_const) {
	int i = 0;
	if (*imm_const == '-') {
		i++;
	}
	if (*(imm_const + i) < '0' || imm_const[0 + i] > '9') {
		return 1;
	}

	return 0;
}


// Set label string as the current label in line 
void get_label(char *line, char* label) {
	char *tmp_label = NULL;
	int label_size = 0;
	int i = 0;

	while (i < MAX_LINE_SIZE) {
		if (*(line + i) == ' ' || *(line + i) == '\t') {
			i++;
		}
		else if (*(line + i) == ':') {
			strncpy(label, tmp_label, label_size);
			*(label + label_size) = '\0';
			return;
		}
		else {
			tmp_label = line;
			label_size++;
			i++;
		}
	}


}

// Get the n-th substring inside a string (ignoring spaces and tabs) 
char* get_n_substring(char *str, int n) {
	char tmpstr[MAX_LINE_SIZE];
	strcpy(tmpstr, str);

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
int get_file_len(char *input_filename) {
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
int get_last_word_address(Dictionary *words) {

	Node *word = words->head;
	int max_row = 0;
	int row;

	while (word != NULL) {

		row = atoi(word->key);

		if (row > max_row) {
			max_row = row;
		}

		word = word->next;

	}
	return max_row;
}

// Registers lookup table according to the project definition
int register_to_int(char* line, int n_register) {
	char* p = get_n_substring(line, n_register);

	if (!strcmp("$zero", p)) { return 0; }
	else if (!strcmp("$imm", p)) { return 1; }
	else if (!strcmp("$v0", p)) { return 2; }
	else if (!strcmp("$a0", p)) { return 3; }
	else if (!strcmp("$a1", p)) { return 4; }
	else if (!strcmp("$a2", p)) { return 5; }
	else if (!strcmp("a3", p)) { return 6; }
	else if (!strcmp("$t0", p)) { return 7; }
	else if (!strcmp("$t1", p)) { return 8; }
	else if (!strcmp("$t2", p)) { return 9; }
	else if (!strcmp("$s0", p)) { return 10; }
	else if (!strcmp("$s1", p)) { return 11; }
	else if (!strcmp("$s2", p)) { return 12; }
	else if (!strcmp("$gp", p)) { return 13; }
	else if (!strcmp("$sp", p)) { return 14; }
	else if (!strcmp("$ra", p)) { return 15; }
	else { return -1; }

}

// Opcodes lookup table according to the project definition
int opcode_to_int(char* line) {
	char* p = get_n_substring(line, 0);

	if (!strcmp("add", p)) { return 0; }
	else if (!strcmp("sub", p)) { return 1; }
	else if (!strcmp("mul", p)) { return 2; }
	else if (!strcmp("and", p)) { return 3; }
	else if (!strcmp("or", p)) {
		return 4;
	}
	else if (!strcmp("xor", p)) { return 5; }
	else if (!strcmp("sll", p)) { return 6; }
	else if (!strcmp("sra", p)) { return 7; }
	else if (!strcmp("srl", p)) { return 8; }
	else if (!strcmp("beq", p)) { return 9; }
	else if (!strcmp("bne", p)) { return 10; }
	else if (!strcmp("blt", p)) { return 11; }
	else if (!strcmp("bgt", p)) { return 12; }
	else if (!strcmp("ble", p)) { return 13; }
	else if (!strcmp("bge", p)) { return 14; }
	else if (!strcmp("jal", p)) { return 15; }
	else if (!strcmp("lw", p)) { return 16; }
	else if (!strcmp("sw", p)) { return 17; }
	else if (!strcmp("halt", p)) { return 18; }
	else { return -1; }

}

// Do main opcode parsing to recieve hex format
void parse_opcode_line(char *line, char *hexline) {

	int op = opcode_to_int(line);
	int reg1 = register_to_int(line, 1);
	int reg2 = register_to_int(line, 2);
	int reg3 = register_to_int(line, 3);

	sprintf(hexline, "%0*X", 2, op);
	sprintf(hexline + 2, "%0*X", 1, reg1);
	sprintf(hexline + 3, "%0*X", 1, reg2);
	sprintf(hexline + 4, "%0*X", 1, reg3);

}

// Do imm const parsing
void parse_imm_const(char *imm_const, char *hex_line, Dictionary* labels) {
	int i = 0;

	if (is_imm_label(imm_const)) {
		char* value = search_by_key(labels, imm_const);
		i = atoi(value);
		sprintf(hex_line, "%0*X", 5, i);

	}

	else {
		i = strtol(imm_const, NULL, 0);
		sprintf(hex_line, "%05X", i & 0xFFFFF);
	}
}

// Get min of two ints
int _min(int a, int b) {
	if (a > b) {
		return b;
	}
	return a;
}


