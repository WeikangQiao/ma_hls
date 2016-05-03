NETWORK = networks/sqTest

CC=g++
CFLAGS=-I$(NETWORK) -I. -I./vivado_include -Wall -g -Wno-unknown-pragmas -Wno-unused-label
# Change in Makefile or Network should trigger recompile, too:
DEPS = *.h Makefile $(NETWORK)/*
CPP_FILES = $(wildcard *.cpp) $(wildcard $(NETWORK)/*.cpp)
OBJS = $(CPP_FILES: .cpp=.o)

all: compileandlink 
	#./test	// 

test: compileandlink
	./test > test.out

clean:
	rm *.o
	rm -rf test.dSYM/

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

compileandlink: $(OBJS)
	# copy weights binary from network to root directory
	cp $(NETWORK)/weights.bin weights.bin
	$(CC) -o test $^ $(CFLAGS)

.PHONY: all test clean