
#ifndef SIM_MEM_H
#define SIM_MEM_H
#include <queue> 
#include <iostream>


using namespace std;
#define MEMORY_SIZE 100//changed to 100 before sending
extern char main_memory[MEMORY_SIZE];


typedef struct page_descriptor
{
    unsigned int V;     // valid  c
    unsigned int D;     // dirty
    unsigned int P;     // permission
    unsigned int frame; //the number of a frame if in case it is page-mapped
} page_descriptor;



class sim_mem
{
    int swapfile_fd; //swap file fd
    int program_fd;  //executable file fd
    int text_size;
    int data_size;
    int bss_size;
    int heap_stack_size;
    int num_of_pages;
    int page_size;
    page_descriptor *page_table; //pointer to page table
   // queue<queue_fr_page> frame_queue; 
    queue <int> frame_queue;
    int number_of_frames;
    bool *frame_array;

public:
    sim_mem(char exe_file_name[], char swap_file_name[], int text_size, int data_size,
     int bss_size, int heap_stack_size, int num_of_pages, int page_size);
    ~sim_mem();  
    char load(int address);
    void store(int address, char value);
    void print_memory();
    void print_swap();
    void print_page_table();
    char in_main_memory(int offset,int page);
    int find_empty_frame(int size_temp);
    void copy_array_to_main_memory(char temp[],int frame_space);
    char bring_log(int page,int offset);
    int  full_frame_swap_or_delete();
    char bring_from_swap(int page ,int offset);
    char fill_it_with_zero(int page);
};


#endif