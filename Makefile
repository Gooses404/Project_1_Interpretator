CC = gcc
CFLAGS = -Wall -Wextra -Isrc -Isrc/reader

TARGET = myfind

SRCS = src/reader/lexer.c \
       src/reader/parser.c \
       src/commands.c \
       src/main.c

OBJS = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)