CC = gcc
CFLAGS = -Wall -Wextra -Werror
TARGET = color
BIN_DIR = bin

all: $(TARGET)

$(TARGET): color.c
	$(CC) $(CFLAGS) -o $(TARGET) color.c
	mkdir -p $(BIN_DIR)
	mv $(TARGET) $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)