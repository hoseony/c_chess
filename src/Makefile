CC = gcc
CFLAGS = -Wall -std=c11

SRCS = bitboards.c move_generation.c fen.c legal_move.c main.c
OBJS = $(SRCS:.c=.o)

chess: $(OBJS)
	$(CC) $(CFLAGS) -o chess $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f chess $(OBJS)
