CC = gcc
# https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
CFLAGS_DEBUG = -Wall -Wextra -std=c11 -g -O0
CFLAGS_BUILD = -Wall -Wextra -std=c11 -O3 
OTHER_CFLAGS_BUILD = -lraylib -lX11 -lm 
SRCS = bitboards.c move_generation.c parse.c legal_move.c main.c finding_magic.c engine.c
OBJS = $(addprefix ../build/, $(SRCS:.c=.o))

debug: CFLAGS = $(CFLAGS_DEBUG)
debug: clean chess

build: CFLAGS = $(CFLAGS_BUILD)
build: clean chess

chess: $(OBJS)
	$(CC) $(CFLAGS) -o ../build/chess $(OBJS) $(OTHER_CFLAGS_BUILD)

../build/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f ../build/chess $(OBJS)

run: chess
	../build/chess
