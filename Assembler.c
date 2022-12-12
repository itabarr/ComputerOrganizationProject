#include <stdio.h>
#include <string.h>

// This function converts the given assembly instruction
// into its machine code representation.
int assemble(char* instr) {
  // Check the instruction
  if (strcmp(instr, "ADD") == 0) {
    // Return the machine code for ADD
    return 1;
  } else if (strcmp(instr, "SUB") == 0) {
    // Return the machine code for SUB
    return 2;
  } else if (strcmp(instr, "MUL") == 0) {
    // Return the machine code for MUL
    return 3;
  } else if (strcmp(instr, "DIV") == 0) {
    // Return the machine code for DIV
    return 4;
  } else {
    // Return -1 if the instruction is not recognized
    return -1;
  }
}

int main() {
  // Read the assembly instruction from the user
  char instr[10];
  printf("Enter assembly instruction: ");
  scanf("%s", instr);

  // Assemble the instruction
  int machineCode = assemble(instr);

  // Print the machine code
  printf("Machine code: %d\n", machineCode);

  return 0;
}