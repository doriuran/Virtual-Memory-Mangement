all: sim_mem.cpp sim_mem.h main.cpp
	g++ -g sim_mem.cpp main.cpp  -o ex4

all-GDB: sim_mem.cpp sim_mem.h main.cpp
	g++ -g sim_mem.cpp main.cpp -o ex4