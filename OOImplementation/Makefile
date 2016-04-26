CC=g++
CFLAGS=-I. -I../vivado_include -Wall -g -Wno-unknown-pragmas -Wno-unused-label
DEPS = *.h
CPP_FILES = $(wildcard *.cpp)
OBJS = $(CPP_FILES: .cpp=.o)

run: test
	./test
	
clean:
	rm *.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)


