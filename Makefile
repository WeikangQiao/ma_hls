CC=g++
CFLAGS=-I. -Wall -g
DEPS = *.h
TESTS = test_processing_element.o processing_element.o

run: test
	./test

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(TESTS)
	$(CC) -o $@ $^ $(CFLAGS)


