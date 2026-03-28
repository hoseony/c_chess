#include "constants.h" 
#include "types.h" 

U64 MG_knight(U64 board) {
    U64 MG_knight = 0;
    U64 shifter = 0;

    for(int i = 0; i < 64; i++) {
        if ( ((shifter = (1ULL << i)) | board) == board )  {
            // Generates moves for knight dependent on row and column
            // (does not add invalid moves if knight is on edge of board)

            MG_knight |= (shifter << 17) * !(((BIT_8_RANK | BIT_7_RANK) & shifter) || (BIT_H_FILE & shifter));
            MG_knight |= (shifter << 15) * !(((BIT_8_RANK | BIT_7_RANK) & shifter) || (BIT_A_FILE & (shifter)));
            MG_knight |= (shifter << 6) * !((BIT_8_RANK & shifter) || ((BIT_A_FILE | BIT_B_FILE) & (shifter)));
            MG_knight |= (shifter << 10) * !((BIT_8_RANK & shifter) || ((BIT_G_FILE | BIT_H_FILE) & (shifter)));
            MG_knight |= (shifter >> 17) * !(((BIT_1_RANK | BIT_2_RANK) & shifter) || (BIT_A_FILE & (shifter)));
            MG_knight |= (shifter >> 15) * !(((BIT_1_RANK | BIT_2_RANK) & shifter) || (BIT_H_FILE & (shifter)));
            MG_knight |= (shifter >> 6) * !(((BIT_1_RANK & shifter) || ((BIT_G_FILE | BIT_H_FILE) & (shifter))));
            MG_knight |= (shifter >> 10) * !((BIT_1_RANK & shifter) || ((BIT_A_FILE | BIT_B_FILE) & (shifter)));
        }
    }
    return MG_knight;
}

U64 MG_rook(U64 board) {
    U64 MG_rook = 0;
    U64 shifter = 0;
    for(int i = 0; i < 64; i++) {
        if( ((shifter = (1ULL << i)) | board) == board) {
            MG_rook |= (BIT_A_FILE << (i % 8));
            MG_rook |= (BIT_1_RANK << (i / 8) * 8);
        }
    }
    return MG_rook;
}

// Only produces valid data if board has only one on bit in the board
// Locates position of on bit
Board_pos debug_index_bitboard_pos(U64 board) {
	size_t r, f; 
	for (r = 0; (board >> 8) > 0; r++)
		board >>= 8; 
	for (f = 0; (board >> 1) > 0; f++)
		board >>= 1;
	return (Board_pos)(r * 8 + f)
}



