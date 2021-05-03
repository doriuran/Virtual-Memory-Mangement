
#include "sim_mem.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <queue>
#define READ 0
#define WRITE 1
#define FULL_FRAME -1

char main_memory[MEMORY_SIZE];

sim_mem::sim_mem(char exe_file_name[], char swap_file_name[], int text_size, int data_size,
                 int bss_size, int heap_stack_size, int num_of_pages, int page_size)
{
    //open excutable file
    program_fd = open(exe_file_name, O_RDONLY, 0);
    if (program_fd == -1)
    {
        perror("Cannot open Exe file");
        exit(1);
    }
    //open the swap file or create one
    swapfile_fd = open(swap_file_name, O_RDWR | O_CREAT, S_IRWXU);
    if (swapfile_fd == -1)
    {
        perror("Cannot open file");
        exit(1); //needed???
    }

    //intliaze all varbiles
    this->text_size = text_size;
    this->data_size = data_size;
    this->bss_size = bss_size;
    this->heap_stack_size = heap_stack_size;
    this->num_of_pages = num_of_pages;
    this->page_size = page_size;
    //all main memory intliaze to zero's
    int j;
    for (j = 0; j < MEMORY_SIZE; j++)
    {
        main_memory[j] = '0';
    }

    //alocating page table
    this->page_table = (page_descriptor *)malloc(sizeof(page_descriptor) * (this->num_of_pages));
    if (this->page_table == NULL)
    {
        perror(" ERR=Coudlnt allocate memory");
        exit(1);
    }

    //intlaize page table
    int k;
    //all vaild and dirty bit are 0 ,frame is -1 .
    for (k = 0; k < this->num_of_pages; k++)
    {
        //read and write premssion
        if (k < this->text_size / this->page_size)
        {                                 //will tell us the number of pages of text
            this->page_table[k].P = READ; //Text pages
        }
        else
        {
            this->page_table[k].P = WRITE; //Heap,stack,bss pages
        }
        this->page_table[k].V = 0; //vaild
        this->page_table[k].D = 0; //dirty bit
        this->page_table[k].frame = -1;
    }

    //swapfile intliaze with '0' with the size of the logical size
    int logical_size = this->num_of_pages * page_size;
    char temp[logical_size];
    int t = 0;
    while (t < logical_size)
    {
        temp[t] = '0';
        t++;
    }
    int e = write(swapfile_fd, temp, logical_size);
    if (e == -1)
    {
        perror(" ERR=Coudlnt allocate memory");
        exit(1);
    }

    // bool array for indicate a free space

    this->number_of_frames = MEMORY_SIZE / this->page_size;
    this->frame_array = (bool *)malloc(sizeof(bool) * this->number_of_frames); //will be indicator for empty space;
    if (this->page_table == NULL)
    {
        perror(" ERR=Coudlnt allocate memory");
        exit(1);
    }
    //this->frame_array[this->number_of_frames];
    int w = 0;
    while (w < this->number_of_frames)
    {
        this->frame_array[w] = false;
        w++;
    }
};

void sim_mem::store(int address, char value)
{
    int offset = address % page_size;     //how many steps into the frame;
    int page = address / this->page_size; //number of the page

    //if the page doent have premisson to write p=0
    if (this->page_table[page].P == 0)
    {
        printf("This is a text area doent have permission to write here");
        return;
    }
    else
    { //p=1
        if (this->page_table[page].V == 1)
        { //insdie the main memory just changed D bit and changed value
            //int phyiscal_address = (this->page_table[page].frame * this->page_size) + offset;

            main_memory[(this->page_table[page].frame * this->page_size) + offset] = value;
            this->page_table[page].D == 1;
        }
        else
        { //vaild = 0 and p =1
            if (this->page_table[page].D == 0)
            {
                //if text or data bring from the logical memory
                if (page < (this->data_size + this->text_size) / this->page_size)
                {
                    this->page_table[page].D = 1;
                    bring_log(page, offset); //put it in the main memory
                    main_memory[(this->page_table[page].frame * this->page_size) + offset] = value;
                }
                else //heap/stack/bss
                {
                    this->page_table[page].D = 1;
                    fill_it_with_zero(page);
                    main_memory[(this->page_table[page].frame * this->page_size) + offset] = value;
                }
            }
            else //vaild=0 p=0 d=1 so he is in the swap
            {
                this->page_table[page].D = 1;
                bring_from_swap(page, offset); //bring it to the maim memory
                main_memory[(this->page_table[page].frame * this->page_size) + offset] = value;
            }
        }
    }
}
char sim_mem ::load(int address)
{

    int offset = address % page_size;     //how many steps into the frame;
    int page = address / this->page_size; //number of the page

    if (this->page_table[page].V == 1)
    {
        return in_main_memory(offset, page);
    }
    if (this->page_table[page].V == 0)
    {
        if (this->page_table[page].P == 0) // v=0 and p=0
        {
            return bring_log(page, offset);
        }
        else
        {                                      //p=1
            if (this->page_table[page].D == 0) //p=1 and d=0
            {
                if (page >= (this->data_size + this->text_size + this->bss_size) / this->page_size) //means that your page isnt a data page
                {                                                                                   //if it is heap stack  and its v=0 and d=0 eroor
                    printf("erorr Vaild is zero and dirty is zero,trying to load heap,stack,bss for the first time \n");
                    return '\0';
                }
                //bss are
                if ((page >= (this->data_size + this->text_size) / this->page_size) &&
                    page < (this->data_size + this->text_size + this->bss_size) / (this->page_size))
                {
                    return fill_it_with_zero(page);
                }

                else
                {
                    // if it data need from bring log
                    return bring_log(page, offset); //he's heap/stack/bss and never been used
                }
            }
            else //p=1 and d=1
            {
                //dirty is one need to bring is from swap
                return bring_from_swap(page, offset);
            }
        }
    }
};
char sim_mem::fill_it_with_zero(int page)
{
    //fine a free space
    int frame_space = find_empty_frame(this->number_of_frames);
    if (frame_space == FULL_FRAME)
    {
        frame_space = full_frame_swap_or_delete();
    }
    //fill a temp arrray with '0'
    char temp[this->page_size];
    int i = 0;
    while (i < this->page_size)
    {
        temp[i] = '0';
        i++;
    }
    //copy it to main memory
    copy_array_to_main_memory(temp, frame_space);
    //update page table
    this->frame_array[frame_space] = true;
    this->page_table[page].V = 1;
    this->page_table[page].frame = frame_space;
    this->frame_queue.push(frame_space);
    return '0';
}
char sim_mem::bring_from_swap(int page, int offset)
{
    char temp[this->page_size];
    lseek(this->swapfile_fd, (page * this->page_size), SEEK_SET); //will move the pointer to the right page
    int n = read(this->swapfile_fd, temp, this->page_size);
    if (n <= 0)
    {
        perror("Read Failed");
        exit(1);
    }
    int frame_space = find_empty_frame(this->number_of_frames);

    if (frame_space == FULL_FRAME)
    {
        frame_space = full_frame_swap_or_delete();
    }

    this->frame_array[frame_space] = true;
    copy_array_to_main_memory(temp, frame_space);
    //fill page table
    this->page_table[page].V = 1;
    this->page_table[page].frame = frame_space;
    this->frame_queue.push(frame_space);
    return in_main_memory(page, offset);
}
char sim_mem ::bring_log(int page, int offset) //v=0
{

    char temp[this->page_size];
    lseek(this->program_fd, (page * this->page_size), SEEK_SET); //will move the pointer to the right page
    int n = read(this->program_fd, temp, this->page_size);
    if (n <= 0)
    {
        perror("Read Failed");
        exit(1);
    }

    int frame_space = find_empty_frame(this->number_of_frames);

    if (frame_space == FULL_FRAME)
    {
        frame_space = full_frame_swap_or_delete();
    }

    this->frame_array[frame_space] = true;
    copy_array_to_main_memory(temp, frame_space);
    //fill page table
    this->page_table[page].V = 1;
    this->page_table[page].frame = frame_space;
    this->frame_queue.push(frame_space);

    return in_main_memory(page, offset);
};
void sim_mem::copy_array_to_main_memory(char temp[], int frame_space)
{
    int phyiscal_address = frame_space * this->page_size;
    for (int i = 0; i < this->page_size; i++)
    {
        main_memory[phyiscal_address + i] = temp[i];
    }
};

int sim_mem::find_empty_frame(int size_temp)
{
    int i = 0;
    bool flag = false;
    while (i < size_temp)
    {
        if (frame_array[i] == false)
        {
            return i;
        }
        else
            i++;
    }
    return FULL_FRAME;
};
int sim_mem ::full_frame_swap_or_delete()
{
    int out = this->frame_queue.front(); //value from the first element
    this->frame_queue.pop();             //out from the queue
    for (int m = 0; m < this->num_of_pages; m++)
    {
        if (this->page_table[m].frame == out) //will point on the right frame
        {

            if (this->page_table[m].P == 0 || this->page_table[m].P == 1 && this->page_table[m].D == 0) //cant write on it so can ran over it
            {
                this->page_table[m].V = 0;
                this->page_table[m].frame = -1;
                return out;
            }
            int to_write;
            if (this->page_table[m].P == 1)
            {
                if (this->page_table[m].D == 1)
                { //somone wrote on it need to go swap page
                    for (int n = 0; n < this->num_of_pages; n++)
                    { //find out wich page am i ;
                        if (this->page_table[n].frame == out)
                        {
                            to_write = n; ///page from the logical area
                        }
                    }
                    char temp_swap[this->page_size];
                    int l = 0;
                    while (l < this->page_size)
                    {
                        temp_swap[l] = main_memory[(out * this->page_size) + l]; //was to write
                        l++;
                    }
                    lseek(this->swapfile_fd, (to_write * this->page_size), SEEK_SET);
                    int e = write(swapfile_fd, temp_swap, this->page_size);
                    if (e == -1)
                    {
                        perror(" ERR=Coudlnt write to file ");
                        exit(1);
                    }

                    this->page_table[to_write].V = 0;
                    this->page_table[to_write].frame = -1;
                }
            }
        }
    }
    return out; //indictor for the empty sapce now
}

char sim_mem ::in_main_memory(int page, int offset)
{
    int frame = this->page_table[page].frame;
    int phyiscal_address = frame * this->page_size + offset;
    // printf("this the letter %c\n", main_memory[phyiscal_address]);
    return main_memory[phyiscal_address];
};

void sim_mem::print_memory()
{
    int i;
    printf("\n Physical memory \n");
    for (i = 0; i < MEMORY_SIZE; i++)
    {
        printf("[%c]\n", main_memory[i]);
    }
};

void sim_mem::print_swap()
{
    char *str = (char *)malloc((this->page_size) * sizeof(char));
    int i;
    printf("\n Swap memory\n");
    lseek(swapfile_fd, 0, SEEK_SET); // go to the start of the file
    while (read(swapfile_fd, str, this->page_size) == this->page_size)
    {
        for (i = 0; i < page_size; i++)
        {
            printf("%d - [%c]\t", i, str[i]);
        }
        printf("\n");
    }
    free(str);
};

void sim_mem::print_page_table()
{
    int i;
    printf("\n page table \n");
    printf("Valid\t Dirty\t Permission \t Frame\n");
    for (i = 0; i < num_of_pages; i++)
    {
        printf("[%d]\t[%d]\t[%d]\t[%d]\n",
               page_table[i].V,
               page_table[i].D,
               page_table[i].P,
               page_table[i].frame);
    }
};
sim_mem::~sim_mem()
{
    free(this->frame_array);
    free(this->page_table);
    close(this->swapfile_fd);
    close(this->program_fd);
}