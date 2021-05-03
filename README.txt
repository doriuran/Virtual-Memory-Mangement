File: sim_mem.cpp	
login name= doriy

Writen by:Dor iuran,id=206123689


The program provides a "RAM simulation".
The program is handling the memory from the "disk" to ram.
It doing it by the methood of page table.
Each row in the page table got 4 indicator :
                                            1.vaild -1 if alrady in the main memorey 0 otherwise
                                            2.dirty- if changed during the run of the proccess.
                                            3.frame-The number of the frame in the main memorey.
                                            4.Permissions-0 if only for raed 1-for read and write.
 
Suppourt all RAM opertioins, can bring from a "file "(our disk),
writing and loading from the swap file for pages that changed.





Compile:g++ -g sim_mem.cpp main.cpp  -o ex4
Run: ex4.cpp

Program files:
  


1.main.cpp -The main,the user interface with the program.
2.sim_mem.h-Declaration of the class.
3.sim_mem.cpp- Contain the implementation of each function from the Vehicle.h.
4.makefile.
5.README



