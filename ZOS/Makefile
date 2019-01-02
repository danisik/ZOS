CC = gcc
BIN = ntfs
OBJ = $(wildcard src/*.c)
FLAGS = -lpthread -Wall -std=c99 -D_GNU_SOURCE	

all: $(BIN) clean

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(FLAGS)

%.o: %.c
	$(CC) -c $< -o $@

clean:	
	rm -f *.o
