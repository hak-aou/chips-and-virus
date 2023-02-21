CC=gcc
CFLAG=-Wall
LDFLAGS=-lMLV
ALL=$(OBJ)final.o

# .exe
EXE=game
BIN=./bin/
# .c
SRC=./src/
# .o
OBJ=./obj/

$(BIN)$(EXE): $(ALL)
	$(CC) -o $@ $^ $(CFLAG) $(LDFLAGS)

final.o: $(SRC)final.c

$(OBJ)%.o: $(SRC)%.c
	$(CC) -c $< -o $@ $(CFLAG)

clean:
	rm -f $(OBJ)*.o
	rm -f $(BIN)*