TARGET = fzf
SRC = fzf.c
LIBS = -lm -lncurses
CC = gcc
CFLAGS = -g -Wall

OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)
