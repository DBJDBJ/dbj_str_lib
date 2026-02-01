# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c2x -O2

LDFLAGS =

# Target executable
TARGET = main

# Source files
SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJECTS) a.out

# Rebuild everything
rebuild: clean all

# Run the program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean rebuild run
