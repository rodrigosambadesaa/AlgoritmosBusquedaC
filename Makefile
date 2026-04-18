CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic
TARGET = quick_sort_benchmark.exe
SRC = main.c quick_sort.c vector_dinamico.c big_decimal.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
