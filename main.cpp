#include "sim_mem.h"
#include <iostream>

//char main_memory[MEMORY_SIZE];


int main()
{
//char val;
sim_mem mem_sm((char*)"text",(char*) "swap_file" ,25, 50, 25,25, 25, 5);
mem_sm.print_page_table();
char a=mem_sm.load(1);//a
mem_sm.print_memory();

mem_sm.store(30,'X');
mem_sm.store(36,'y');
mem_sm.print_memory();

char b=mem_sm.load(7);//b
char c=mem_sm.load(12);//c
mem_sm.print_memory();

char d=mem_sm.load(16);//d
char w=mem_sm.load(1);//a
mem_sm.print_memory();

char t=mem_sm.load(40);

//printf("75 is bss :%c \n",t);
//printf(" 1 is :%c \n,7 is :%c \n,12 is :%c \n,16 is :%c \n",a,b,c,d);
mem_sm.print_page_table();
//mem_sm.store(30,'X');
//val = mem_sm.load ( 98);
mem_sm.print_memory();
mem_sm.print_swap();
}