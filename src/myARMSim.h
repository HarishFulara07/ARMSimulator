/* 

The project is developed as part of Computer Architecture class
Project Name: Functional Simulator for subset of ARM Processor

Developer's Name: Harish Fulara (2014143)
Developer's Email id: harish14143@iiitd.ac.in
Date: March 24, 2015


*/


/* 

myARMSim.h

Purpose of this file: header file for myARMSim

*/

void run_armsim();
void reset_proc();
void load_program_memory(char* file_name);
void write_data_memory();
void swi_exit();


//reads from the instruction memory and updates the instruction register
void fetch();
//reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
void decode();
//executes the ALU operation based on ALUop
void execute();
//perform the memory operation
void mem();
//writes the results back to register file
void write_back();


int read_word(char *mem, unsigned int address);
void write_word(char *mem, unsigned int address, unsigned int data);
