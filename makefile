CC=g++
CFLAGS=-std=c++11
LIBS=-lpthread
DEPS=directory.hpp


all: client.x server.x

client.x: client.o
	$(CC) -o $@ $^ $(LIBS)
server.x: server.o
	$(CC) -o $@ $^ $(LIBS)
%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PRECIOUS: %.o

clean:
	rm *.o *.x
