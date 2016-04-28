NETWORK = networks/miniFire8

CC=g++
CFLAGS=-I$(NETWORK) -I. -I./vivado_include -Wall -g -Wno-unknown-pragmas -Wno-unused-label -Wno-c++11-compat-deprecated-writable-strings
DEPS = *.h
CPP_FILES = $(wildcard *.cpp) $(wildcard $(NETWORK)/*.cpp)
OBJS = $(CPP_FILES: .cpp=.o)

run: test
	./test
	
clean:
	rm *.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJS)
	# copy weights binary from network to root directory
	cp $(NETWORK)/weights.bin weights.bin
	$(CC) -o $@ $^ $(CFLAGS)


