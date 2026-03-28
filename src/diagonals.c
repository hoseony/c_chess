#include <stdio.h> 
#include <stddef.h>
#include <stdlib.h>

unsigned long long antiDiagonals[15]; 
unsigned long long Diagonals[15]; 


int main() {
	unsigned long long diagonal = 0; 

	for (size_t i = 0; i < 8; i++) {
		diagonal = 0; 
		for (size_t j = 0; j <= i; j++) {
			diagonal |= (1ULL << (i + 7 * j));
		}
		for (size_t j = 1; j < 8 - i; j++) {
			diagonal |= (1ULL << (i + 9 * j));
		}
        print_bb(diagonal);
	}

	for (size_t i = 0; i < 8; i++) {
		diagonal = 0; 
		for (size_t j = 0; j <= i; j++) {
			diagonal |= (1ULL << ((i * 8) - 7 * j));
		}
		for (size_t j = 1; j < 8 - i; j++) {
			diagonal |= (1ULL << ((i * 8) + 9 * j));
		}
        print_bb(diagonal);
	}
    return 0;
}

