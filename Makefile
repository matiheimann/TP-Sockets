include Makefile.inc
SOURCES=$(wildcard *.c)
SOURCES_SERVER = $(filter-out client.c ,$(SOURCES))
SOURCES_CLIENT = $(filter-out server.c ,$(SOURCES))
OBJECTS=$(SOURCES:.c=.o)
DEPS=$(wildcard include/*.h)
CLIENT = client
SERVER = server
DATABASE = database.o

fromzero: clean all

all: $(DATABASE) $(CLIENT) $(SERVER) 

$(DATABASE): $(DEPS) 
	$(GCC) -c -O database.c $(GCCFLAGS) 

$(CLIENT): $(DEPS) $(DATABASE)
	$(GCC) -o $@ $(SOURCES_CLIENT) $(GCCFLAGS) 

$(SERVER): $(DEPS) $(DATABASE)
	$(GCC) -o $@ $(SOURCES_SERVER) $(GCCFLAGS) 

clean:
	rm -ff *.o $(CLIENT) $(SERVER) socket_path_is_useful

.PHONY: all clean