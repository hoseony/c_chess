#include <stdio.h> 

unsigned long long antiDiagonals[15]; 
unsigned long long Diagonals[15]; 

int main() {
	unsigned long long diagonal = 0; 
	for (size_t i = 0; i < 8; i++) {
		diagonal = 0; 
		for (size_t j = 0; j <= i; j++) {
			diagonal |= ((1ULL << j) | (1ULL << j + 8))
		}
		Diagonals[i] = diagonal; 
	}





}
