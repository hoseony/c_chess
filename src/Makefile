CC = gcc
CFLAGS = -Wall -std=c11

chess: bitboards.c move_generation.c fen.c main.c
	$(CC) $(CFLAGS) -o chess bitboards.c move_generation.c fen.c main.c -lm

clean:
	rm -f chess
