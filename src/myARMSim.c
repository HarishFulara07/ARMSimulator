/* 

The project is developed as part of Computer Architecture class

Project Name: Functional Simulator for subset of ARM Processor

Developer's Name: Harish Fulara (2014143)
Developer's Email id: harish14143@iiitd.ac.in
Date: March 24, 2015


*/


/* 

myARMSim.cpp

Purpose of this file: implementation file for myARMSim

*/

#include "myARMSim.h"
#include <stdlib.h>
#include <stdio.h>


//Register file
static unsigned int R[16];

//flags
static int N,C,V,Z;

//memory
static unsigned char MEM[4000];

//intermediate datapath and control path signals

static unsigned int instruction_word;
static unsigned int operand1;
static unsigned int operand2;
static int pc = -4;
static unsigned int destination;
static unsigned int I;
static unsigned int F;
static unsigned int opcode;
static unsigned int S;
static unsigned int cond;
static unsigned int offset12;
static int offset24;
static unsigned int temp;
static int flag;

void run_armsim()
{
	while(1) 
	{
		fetch();
		printf("\n");
	    	decode();
		printf("\n");
    		execute();
		printf("\n");
    		mem();
		printf("\n");
    		write_back();
		printf("-----------------------------------------------------------\n\n");
  	}
}

// it is used to set the reset values
//reset all registers and memory content to 0

void reset_proc()
{
	int i;
	for(i = 0 ; i < 16 ; i++)
	{
		R[i] = 0;
	}
	N = C = V = Z = 0;
	for(i = 0 ; i < 4000 ; i++)
	{
		MEM[i] = '\0';
	}
	instruction_word = 0;
	operand1 = 0;
	operand2 = 0;
}

//load_program_memory reads the input memory, and pupulates the instruction 
//memory

void load_program_memory(char * file_name)
{
	FILE *fp;
	unsigned int address, instruction;
	fp = fopen(file_name, "r");
	if(fp == NULL)
	{
		printf("Error opening input mem file\n");
		exit(1);
	}
	while(fscanf(fp, "%x %x", &address, &instruction) != EOF)
	{
	    	write_word(MEM, address, instruction);
  	}
  	fclose(fp);
}

//writes the data memory in "data_out.mem" file

void write_data_memory()
{
	FILE *fp;
	unsigned int i;
  	fp = fopen("data_out.mem", "w");
  	if(fp == NULL)
	{
	    	printf("Error opening dataout.mem file for writing\n");
		return;
  	}
  
  	for(i=0; i < 4000; i = i+4)
	{
	    	fprintf(fp, "%x %x\n", i, read_word(MEM, i));
 	}
	fclose(fp);
}

//should be called when instruction is swi_exit

void swi_exit()
{
	write_data_memory();
	printf("\n\n=============PROGRAM TERMINATED SUCCESSFULLY================\n\n");
	exit(0);
}


//reads from the instruction memory and updates the instruction register

void fetch()
{
	printf("FETCH: ");
	while(1)
	{
		pc = pc + 4;
		instruction_word = read_word(MEM, pc);
		if(instruction_word != 0)
		{		
	    		printf("FETCH Instruction 0x%x From Address 0x%x\n", instruction_word,pc);
			if(instruction_word == 0xEF000011)
			{
				swi_exit();
			}
			break;
		}
 	}
}

//reads the instruction register, reads operand1, operand2 from register file, decides the operation to be performed in execute stage

void decode()
{
	printf("DECODE: ");
	F = (instruction_word & 0x0C000000) >> 26;
	cond = (instruction_word & 0xF0000000) >> 28;
	if(F == 0)
	{
		printf("It Is A Data Processing Instruction\n");
		I = (instruction_word & 0x02000000) >> 25;
		opcode = (instruction_word & 0x01E00000) >> 21;
		S = (instruction_word & 0x00100000) >> 20;
		destination = (instruction_word & 0x0000F000) >> 12;
		operand1 = (instruction_word & 0x000F0000) >> 16;
		printf("Cond                 = %u\n",cond);
		printf("F                    = %u\n",F);
		printf("I                    = %u\n",I);
		printf("Opcode               = %u\n",opcode);
		switch(opcode)
		{
			case 0 : printf("Operation is AND\n");
			break;
			case 1 : printf("Operation is EOR\n");
			break;
			case 2 : printf("Operation is SUB\n");
			break;
			case 4 : printf("Operation is ADD\n");
			break;
			case 5 : printf("Operation is ADC\n");
			break;
			case 10 : printf("Operation is CMP\n");
			break; 
			case 12 : printf("Operation is ORR\n");
			break; 
			case 13 : printf("Operation is MOV\n");
			break; 
			case 15 : printf("Operation is MVN\n");
			break; 
		}
		printf("S                    = %u\n",S);
		printf("Operand1             = %u\n",operand1);
		printf("Destination Register = %u\n",destination);
		if(I == 0)
		{
			operand2 = instruction_word & 0x0000000F;
			printf("Operand2             = %u\n",operand2);
		}
		else if(I == 1)	
		{
			operand2 = instruction_word & 0x000000FF;
			printf("Operand2             = %u\n",operand2);
		}
		printf("Register R%u         = %u\n",operand1,R[operand1]);
		printf("Register R%u         = %u\n",operand2,R[operand2]);
	}
	else if(F == 1)
	{
		printf("It Is A Data Transfer Instruction\n");
		opcode = (instruction_word & 0x03F00000) >> 20;
		destination = (instruction_word & 0x0000F000) >> 12;
		operand1 = (instruction_word & 0x000F0000) >> 16;
		offset12 = instruction_word & 0x00000FFF;
		printf("Cond                 = %u\n",cond);
		printf("F                    = %u\n",F);
		printf("Opcode               = %u\n",opcode);
		switch(opcode)
		{
			case 24 : printf("Operation is STR\n");
			break;
			case 25 : printf("Operation is LDR\n");
			break; 
		}
		printf("Operand1             = %u\n",operand1);
		printf("Destination Register = %u\n",destination);
		printf("Offset               = %u\n",offset12);
		printf("Register R%u         = %u\n",operand1,R[operand1]);
		printf("Register R%u         = %u\n",destination,R[destination]);
	}
	else if(F == 2)
	{
		printf("It Is A Branch Instruction\n");
		opcode = (instruction_word & 0x03000000) >> 24;
		offset24 = instruction_word & 0x00FFFFFF;
		int msb = (offset24 & 0x800000) >> 23;
		if(msb == 1)
		{
			offset24 = 0xFF000000 + offset24;
		}
		else
		{
			offset24 = 0x00000000 + offset24;
		}
		printf("Cond                 = %u\n",cond);
		switch(cond)
		{
			case 0 : printf("Condition Is BEQ\n");
			break;
			case 1 : printf("Condition Is BNE\n");
			break;
			case 11 : printf("Condition Is BLT\n");
			break;
			case 12 : printf("Condition Is BGT\n");
			break;
			case 13 : printf("Condition Is BLE\n");
			break;
			case 10 : printf("Condition Is BGE\n");
			break;
			case 14 : printf("Condition Is B\n");
			break; 
		}
		printf("F                    = %u\n",F);
		printf("Opcode               = %u\n",opcode);
		printf("Offset               = %d\n",offset24);
	} 
}

//executes the ALU operation based on ALUop

void execute()
{
	printf("EXECUTE: ");
	if(F==0 && I==0)
	{
		switch(opcode)
		{
			case 0 : temp = R[operand1] & R[operand2];
			printf("AND %u and %u\n",R[operand1],R[operand2]);  
			break;
			case 1 : temp = (R[operand1] & ~R[operand2]) | (~R[operand1] & R[operand2]);
			printf("XOR %u and %u\n",R[operand1],R[operand2]); 
			break;
			case 2 : temp = R[operand1] - R[operand2];
			printf("SUB %u and %u\n",R[operand1],R[operand2]); 
			break;
			case 4 : temp = R[operand1] + R[operand2];
			printf("ADD %u and %u\n",R[operand1],R[operand2]); 
			break;
			case 5 : temp = R[operand1] + R[operand2];
			C = 0;
			if(temp > 4294967296)
			{
				C = 1;
			}
			printf("ADDC %u and %u\n",R[operand1],R[operand2]); 
			printf("Carry C = %d\n",C);
			break;	
			case 10 : flag = R[operand1] - R[operand2];
			if(flag == 0)
			{
				N = 0;
				C = 0;
				V = 0;
				Z = 1;
			}
			else if (flag > 0)
			{
				N = 0;
				C = 0;
				V = 0;
				Z = 0;
			}
			else if (flag < 0)
			{
				N = 1;
				C = 0;
				V = 0;
				Z = 0;
			}
			printf("CMP %u and %u\n",R[operand1],R[operand2]);
			printf("N = %d\n",N);
			printf("C = %d\n",C);
			printf("V = %d\n",V);
			printf("Z = %d\n",Z); 
			break; 
			case 12 : temp = R[operand1] | R[operand2];
			printf("ORR %u and %u\n",R[operand1],R[operand2]); 
			break; 
			case 13 : temp = R[operand2];
			printf("MOV %u\n",R[operand2]); 
			break; 
			case 15 : temp = ~R[operand2];
			printf("MVN %u\n",R[operand2]); 
			break;
		}
	}
	else if (F==0 && I == 1)
	{
		switch(opcode)
		{
			case 0 : temp = R[operand1] & operand2;  
			printf("AND %u and %u\n",R[operand1],operand2); 
			break;
			case 1 : temp = (R[operand1] & ~(operand2)) | (~R[operand1] & (operand2));
			printf("XOR %u and %u\n",R[operand1],operand2); 
			break;
			case 2 : temp = R[operand1] - operand2;
			printf("SUB %u and %u\n",R[operand1],operand2); 
			break;
			case 4 : temp = R[operand1] + operand2;
			printf("ADD %u and %u\n",R[operand1],operand2); 
			break;
			case 5 : temp = R[operand1] + operand2;
			C = 0;
			if(temp > 4294967296)
			{
				C = 1;
			}
			printf("ADDC %u and %u\n",R[operand1],operand2); 
			printf("Carry C = %d\n",C);
			break;
			case 10 : flag = R[operand1] - operand2;
			if(flag == 0)
			{
				N = 0;
				C = 0;
				V = 0;
				Z = 1;
			}
			else if (flag > 0)
			{
				N = 0;
				C = 0;
				V = 0;
				Z = 0;
			}
			else if (flag < 0)
			{
				N = 1;
				C = 0;
				V = 0;
				Z = 0;
			}
			printf("CMP %u and %u\n",R[operand1],operand2); 
			printf("N = %d\n",N);
			printf("C = %d\n",C);
			printf("V = %d\n",V);
			printf("Z = %d\n",Z); 
			break; 
			case 12 : temp = R[operand1] | operand2;
			printf("ORR %u and %u\n",R[operand1],operand2); 
			break; 
			case 13 : temp = operand2;
			printf("MOV %u\n",operand2); 
			break; 
			case 15 : temp = ~(operand2);
			printf("MVN %u\n",operand2); 
			break;
		}
	}
	else if(F==1)
	{
		switch(opcode)
		{
			case 24 : temp = R[operand1] + offset12;
			printf("ADD %u and %u\n",R[operand1],offset12); 
			break;
			case 25 : temp = R[operand1] + offset12;
			printf("ADD %u and %u\n",R[operand1],offset12); 
			break; 
		}
	}
	else if(F==2)
	{
		printf("OLD pc = %d\n",pc);
		switch(cond)
		{
			case 0 : if (Z==1)
			{
				pc = pc + (offset24 * 4) + 4; 
			}
			break;
			case 1 : if (Z==0)
			{
				pc = pc + (offset24 * 4) + 4; 
			}
			break;
			case 11 : if (Z==0 && N==1)
			{
				pc = pc + (offset24 * 4) + 4; 
			}
			break;
			case 12 : if (Z==0 && N==0)
			{
				pc = pc + (offset24 * 4) + 4; 
			}
			break;
			case 13 : if ((Z==0 && N==1) || Z==1)
			{
				pc = pc + (offset24 * 4) + 4; 
			}
			break;
			case 10 : if ((Z==0 && N==0) || Z==1)
			{
				pc = pc + (offset24 * 4) + 4; 
			}
			break;
			case 14 : pc = pc + (offset24 * 4) + 4;
			break; 
		}
		printf("NEW pc = %d\n",pc);
	}
}

//perform the memory operation

void mem() 
{
	printf("MEMORY: ");
	if(F==0 || F==1)
	{
		switch(opcode)
		{
			case 0 : printf("NO Memory Operation\n");
			break;
			case 1 : printf("NO Memory Operation\n");
			break;
			case 2 : printf("NO Memory Operation\n");
			break;
			case 4 : printf("NO Memory Operation\n");
			break;
			case 5 : printf("NO Memory Operation\n");
			break;
			case 10 : printf("NO Memory Operation\n");
			break; 
			case 12 : printf("NO Memory Operation\n");
			break; 
			case 13 : printf("NO Memory Operation\n");
			break; 
			case 15 : printf("NO Memory Operation\n");
			break;
			case 24 : write_word(MEM, temp, R[destination]);
			printf("Storing %u At Address %x\n",R[destination],temp);
			break;
			case 25 : printf("Loading %u From Address %x\n",read_word(MEM, temp),temp);
			temp = read_word(MEM, temp); 
			break;  
		}
	}
	if(F==2)
	{
		switch(cond)
		{
			case 0 : printf("NO Memory Operation\n");
			break;
			case 1 : printf("NO Memory Operation\n");
			break;
			case 11 : printf("NO Memory Operation\n");
			break;
			case 12 : printf("NO Memory Operation\n");
			break;
			case 13 : printf("NO Memory Operation\n");
			break;
			case 10 : printf("NO Memory Operation\n");
			break;
			case 14 : printf("NO Memory Operation\n");
			break; 
		}
	}
}

//writes the results back to register file

void write_back()
{
	printf("WRITEBACK: ");
	if(F==0 || F==1)
	{
		switch(opcode)
		{
			case 0 : printf("Write %u to R%u\n",temp,destination);
			R[destination] = temp;
			break;
			case 1 : printf("Write %u to R%u\n",temp,destination);
			R[destination] = temp;
			break;
			case 2 : printf("Write %u to R%u\n",temp,destination);
			R[destination] = temp;
			break;
			case 4 : printf("Write %u to R%u\n",temp,destination);
			R[destination] = temp;
			break;
			case 5 : printf("Write %u to R%u\n",temp,destination);
			R[destination] = temp;
			break;
			case 10 : printf("No WriteBack Operation\n");
			break; 
			case 12 : printf("Write %u to R%u\n",temp,destination);
			R[destination] = temp;
			break; 
			case 13 : printf("Write %u to R%u\n",temp,destination);
			R[destination] = temp;
			break; 
			case 15 : printf("Write %u to R%u\n",temp,destination);
			R[destination] = temp;
			break; 
			case 24 : printf("No WriteBack Operation\n");
			break;
			case 25 : printf("Write %u to R%u\n",temp,destination);
			R[destination] = temp;
			break;  
		}
	}
	if(F==2)
	{
		switch(cond)
		{
			case 0 : printf("No WriteBack Operation\n");
			break;
			case 1 : printf("No WriteBack Operation\n");
			break;
			case 11 : printf("No WriteBack Operation\n");
			break;
			case 12 : printf("No WriteBack Operation\n");
			break;
			case 13 : printf("No WriteBack Operation\n");
			break;
			case 10 : printf("No WriteBack Operation\n");
			break;
			case 14 : printf("No WriteBack Operation\n");
			break; 
		}
	}
}


int read_word(char * mem, unsigned int address)
{
	int * data;
	data =  (int *) (mem + address);
	return *data;
}

void write_word(char * mem, unsigned int address, unsigned int data)
{
	int * data_p;
	data_p = (int *) (mem + address);
	*data_p = data;
}
