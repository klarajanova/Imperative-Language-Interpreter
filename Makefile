CC = gcc
DEBUG = -D_DEBUG
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g
LIBS = -lm
FILES = expr.c helper.c ial.c main.c parser.c scanner.c stack.c semantic.c inst_list.c interpret.c
OBJS = $(FILES:.c=.o)
EXEC = ifj15

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(DEBUG) $(CFLAGS) -o $(EXEC) $(OBJS) $(LIBS)

.c.o:
	$(CC) $(DEBUG) $(CFLAGS) -c $< -o $@

run:
	./$(EXEC)

clean:
	rm -f *.o $(EXEC)
