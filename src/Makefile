CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O0

SRCS = bitboards.c move_generation.c parse.c legal_move.c main.c finding_magic.c
OBJS = $(addprefix ../build/, $(SRCS:.c=.o))

chess: $(OBJS)
	$(CC) $(CFLAGS) -o ../build/chess $(OBJS)

../build/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f ../build/chess $(OBJS)

run: chess
	../build/chess
