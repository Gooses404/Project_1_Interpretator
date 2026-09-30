CC ?= gcc
CFLAGS = -Wall -Wextra -std=c23 -D_POSIX_C_SOURCE=200809L -Isrc -Isrc/reader
TARGET = mysh

SRCS = src/main.c \
       src/commands.c \
       src/reader/lexer.c \
       src/reader/parser.c

OBJS = $(SRCS:.c=.o)

.PHONY: all debug clean run

# Стандартная сборка с оптимизацией
all: CFLAGS += -O2
all: $(TARGET)

# Отладочная цель с AddressSanitizer и отладочными символами по ТЗ
debug: CFLAGS += -g -fsanitize=address,undefined
debug: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
