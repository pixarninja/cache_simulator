CPP=g++
CFLAGS=-std=c++11 -g -Wall

all: cachesim

cachesim:
	$(CPP) $(CFLAGS) fun.cpp -c -lm
	$(CPP) $(CFLAGS) cachesim.cpp fun.o -o cachesim -lm

clean: 
	rm cachesim
