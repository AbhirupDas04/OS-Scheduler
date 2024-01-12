CC = gcc

CFLAGS = -I. -Wall -Wextra
DEPS = Shellcommands/shell.h Colors/color.h
OBJ = Shellcommands/simple-shell.o Colors/color.o main.o
BIN = bin/main

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN): $(OBJ)
	mkdir -p bin
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJ) $(BIN) ./bin/.*