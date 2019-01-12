CC = g++

#Jméno programu
BIN = ntfs.exe

#Seznam soubor�
SRC = $(wildcard *.cpp)

OBJ = $(SRC:.cpp=.o)

CFLAGS = -Wall -pedantic -pthread

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LIB) $(CFLAGS)
	
%.o: %.cpp
	$(CC) -c -o $@ $< -std=c++98


.PHONY: clean
clean:
	rm -f $(OBJ) $(BIN)