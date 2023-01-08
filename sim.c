#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DIR_LEN 1024        // Max name of files
#define MAX_LINE_SIZE 300       // Max line size of memin
#define MEMORY_SIZE 4096        // Memory size
#define REGISTERS_NUM 16        // Numbers of registers
#define REGISTER_SIZE 32        // Registers bit size


// Instruction struct definition
typedef struct Instruction {
    char raw[6];
    int opcode;
    int rd;
    int rs;
    int rt;
    int imm;
} Instruction;

// Instruction related methods
Instruction create_instruction(char *memory_row ,char *imm_row);
int is_instruction_imm(Instruction inst);

// Memory related methods
char **init_memory(char *memin_file);
void free_memory(char **memory);

// File updateing methods

void excecute_programm(char **memory , int *registers, char* trace_file, char* cycles_file , char* regout_file);
void update_trace(FILE *trace , int pc , Instruction instruction , int *registers);
void write_memout(char *memout_file, char **memory);
void write_cycles(char *cycles_file, int cycles);
void write_regout(char *regout_file, int *registres);

int main(int argc, char* argv[]) {
    char memin_file[MAX_DIR_LEN];
    char memout_file[MAX_DIR_LEN];
    char regout_file[MAX_DIR_LEN];
    char trace_file[MAX_DIR_LEN];
    char cycles_file[MAX_DIR_LEN];


    if (argc!=6 && argc!=1){
        printf("Error: Zero or Two arguments are required.\n");
        return 1;
    }

    if (argc == 1){
        strcpy(memin_file,"memin.txt");
        strcpy(memout_file,"memout.txt");
        strcpy(regout_file,"regout.txt");
        strcpy(trace_file,"trace.txt");
        strcpy(cycles_file,"cycles.txt");
    }

    if (argc == 6){
        strcpy(memin_file,argv[1]);
        strcpy(memout_file,argv[2]);
        strcpy(regout_file,argv[3]);
        strcpy(trace_file,argv[4]);
        strcpy(cycles_file,argv[5]);
    }
        
    
    char **memory = init_memory(memin_file);
    int registres[REGISTERS_NUM] = {0};

    excecute_programm(memory , registres , trace_file , cycles_file , regout_file);
    
    free_memory(memory);
    return 0;
}


// Init memory array with memin file content
char **init_memory(char* memin_file){
    // Init 2D array
    char **memory = (char **)malloc(MEMORY_SIZE * sizeof(char *));
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = (char *)malloc(6 * sizeof(char));
    }


    int row = 0;
    char line[MAX_LINE_SIZE];

    FILE *memin = fopen(memin_file , "r");

    while (fgets(line, sizeof line, memin)){
        strncpy(memory[row] , line , 5);
        memory[row][5] = '\0';
        row++;

    }

    while (row < MEMORY_SIZE){
        strncpy(memory[row] , "00000" , 5);
        memory[row][5] = '\0';
        row++;
    }

    fclose(memin);
    
    return memory;
}

// Free memory allocated by init_memory
void free_memory(char **memory){
    for (int i = 0; i < MEMORY_SIZE; i++) {
        free(memory[i]);
        }
    free(memory);
}

// Create instruction from memory 
Instruction create_instruction(char *memory_row , char *imm_row){
    char hex[2] = {'0' , 'X'};

    char tmp_opcode[5] = {'\0'};
    strncpy(tmp_opcode , hex ,2);
    strncpy(tmp_opcode+2 , memory_row ,2);
    
    char tmp_rd[4] = {'\0'};
    strncpy(tmp_rd , hex ,2);
    strncpy(tmp_rd+2 , memory_row+2 ,1);
    
    char tmp_rs[4] = {'\0'};
    strncpy(tmp_rs , hex ,2);
    strncpy(tmp_rs+2 , memory_row+3 ,1);
    
    char tmp_rt[4] = {'\0'};
    strncpy(tmp_rt , hex ,2);
    strncpy(tmp_rt+2 , memory_row+4 ,1);
    
    char tmp_imm[8] = {'\0'};
    strncpy(tmp_imm , hex ,2);
    strncpy(tmp_imm+2 , imm_row ,5);

    
    Instruction instruction;
    instruction.opcode = strtol(tmp_opcode,NULL,0);
    instruction.rd = strtol(tmp_rd,NULL,0);
    instruction.rs = strtol(tmp_rs,NULL,0);
    instruction.rt = strtol(tmp_rt,NULL,0);
    instruction.imm = strtol(tmp_imm,NULL,0);
    strcpy(instruction.raw , memory_row); 

    return instruction;
}

// Determine if instruction is immediate 
int is_instruction_imm(Instruction inst){
    if (inst.rd ==1 || inst.rs == 1 || inst.rt == 1){
        return 1;
    } 
    return 0;
}

// Logic
void excecute_programm(char **memory , int *registers, char* trace_file , char* cycles_file , char* regout_file){
    
    FILE* trace = fopen(trace_file, "w");
    
    // Fetch first instruction
    Instruction inst = create_instruction(memory[0] , memory[1]);
    
    int pc = 0;
    int cycles = 0;
    char tmp_mem[8] = {'\0'};
    tmp_mem[0] = '0';
    tmp_mem[1] = 'X';
    int try = 0;
    
    // // Do main programm loop - stop when encouter halt 
    while (1){
        if (inst.opcode == 18){
            update_trace(trace , pc , inst , registers);
            write_cycles(cycles_file, cycles);
            write_regout(regout_file, registers);
            fclose(trace);
            return;

        }

        if (is_instruction_imm(inst)){
            registers[1] = inst.imm;
            update_trace(trace , pc , inst , registers);
            pc = pc + 2;
            cycles = cycles + 2;
        }

        else{
            registers[1] = 0;
            update_trace(trace , pc , inst , registers);
            pc++;
            cycles++;
        }


        switch (inst.opcode)
        {
        case 0: // Do add
            registers[inst.rd] = registers[inst.rs] + registers[inst.rt];
            break;
        case 1: // Do sub
            registers[inst.rd] = registers[inst.rs] - registers[inst.rt];
            break;
        
        case 2: // Do mul
            registers[inst.rd] = registers[inst.rs] * registers[inst.rt];    
            break;
        
        case 3: // Do and
            registers[inst.rd] = registers[inst.rs] & registers[inst.rt];    
            break;

        case 4: // Do or
            registers[inst.rd] = registers[inst.rs] || registers[inst.rt];
            break;

        case 5: // Do xor
            registers[inst.rd] = registers[inst.rs] ^ registers[inst.rt];
            break;
        
        case 6: // Do sll
            registers[inst.rd] = registers[inst.rs] << registers[inst.rt];
            break;
           
            
        case 7: // Do sra
            registers[inst.rd] = registers[inst.rs] >> registers[inst.rt];
            break;
            
        case 8: // Do srl
            registers[inst.rd] = registers[inst.rs] >> registers[inst.rt];
            break;
            
        case 9: // Do beq 
            if (registers[inst.rs] == registers[inst.rt]){
                pc = registers[inst.rd];
            }
            break;
            
        
        case 10: // Do bne
            if (registers[inst.rs] != registers[inst.rt]){
                pc = registers[inst.rd];
            }
            break;


        case 11: // Do blt
            if (registers[inst.rs] < registers[inst.rt]){
                pc = registers[inst.rd];
            }
            break;
        
            
        case 12: // Do bgt
            if (registers[inst.rs] > registers[inst.rt]){
                pc = registers[inst.rd];
            }
            break;
            
        case 13: // Do ble
            if (registers[inst.rs] <= registers[inst.rt]){
                pc = registers[inst.rd];
            }
            break;
        case 14: // Do bge
            if (registers[inst.rs] >= registers[inst.rt]){
                pc = registers[inst.rd];
            }
            break;
        case 15: // Do jal
            inst = create_instruction(memory[inst.rd] , memory[inst.rd+1]);
            pc = registers[inst.rs];
            break;
        case 16: // Do lw
            
            strncpy(tmp_mem+2 , memory[inst.rt + inst.rs] ,5);
            registers[inst.rd] = strtol(tmp_mem,NULL,0);
            cycles++;
            break;
            
        case 17: // Do sw
            sprintf(memory[registers[inst.rt + inst.rs]],"%05X", registers[inst.rd] & 0xFFFFF);
            cycles++;
            break;
        }

        inst = create_instruction(memory[pc] , memory[pc+1]);

    }
    
}

// Update trace line
void update_trace(FILE *trace , int pc , Instruction instruction , int *registers){
    
    // Initialize line with none char and relevant spaces 
    char tmp_line[155];
    tmp_line[153] = '\n';
    tmp_line[154] = '\0';

    int space_index[] = {3, 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 135, 144};
    for (int i = 0; i < (sizeof(space_index) / sizeof(int)); i++) {
        tmp_line[space_index[i]] = ' ';
    }

    // handle pc
    char tmp_pc[4] = {'\0'};
    sprintf(tmp_pc,"%03X", pc & 0xFFF);
    strncpy(tmp_line, tmp_pc, 3);

    // handle istruction
    strncpy(tmp_line + 4 , instruction.raw , 5);

    char tmp_reg[9] = {'\0'};
    // handle registers
    for (int i = 0; i < (REGISTERS_NUM); i++) {
        sprintf(tmp_reg,"%08X", registers[i] & 0xFFFFFFFF);
        strncpy(tmp_line + 10 + i * 9 , tmp_reg , 8);
    }

    fprintf(trace, "%s", tmp_line);

}


// Write final memory to memout file
void write_memout(char *memout_file, char **memory){
    // Open the file for writing
    FILE* fp = fopen(memout_file, "w");
    
    if (fp == NULL) {
        fprintf(stderr, "Error opening file\n");
        return;
    }

    // Write the contents of memory to the file
    for (int i = 0; i < MEMORY_SIZE; i++) {
        fprintf(fp, "%s\n", memory[i]);
    }
    fclose(fp);
    return;

}

// Write final cycles file
void write_cycles(char *cycles_file, int cycles){
    // Open the file for writing
    FILE* fp = fopen(cycles_file, "w");
    
    if (fp == NULL) {
        fprintf(stderr, "Error opening file\n");
        return;
    }

    fprintf(fp, "%d", cycles);
    
    fclose(fp);
    return;

}

void write_regout(char *regout_file, int *registers){
    // Open the file for writing
    FILE* fp = fopen(regout_file, "w");
    char tmp_line[10] = {'\0'};
    // handle registers
    for (int i = 0; i < (REGISTERS_NUM); i++) {
        sprintf(tmp_line,"%08X\n", registers[i] & 0xFFFFFFFF);
        fprintf(fp, "%s", tmp_line);
    }


    fclose(fp);

}

