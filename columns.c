#include <stdio.h> 

int main() {
	unsigned long long cols[8]; 
	unsigned long long col = 0; 
	for (int i = 0; i < 8; i++) {
		col = 0; 
		for (int j = 0; j < 8; j++) {
			col |= 1ULL << (j + i * 8);
		}
		cols[i] = col; 
	}
	

	for (int i = 0; i < 8; i++) {
		printf("#define BIT_%c_ROW %llu\n", 'A' + i, cols[i]); 
	}
}
