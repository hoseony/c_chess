#include "constants.h" 
#include "bitboards.h"
#include <stdbool.h>

// --------- Function Prototype --------
U64 generateKnightMove(int square);
U64 generateRookMove(int square, U64 occupied);
U64 generateBishopMove(int square, U64 occupied);
U64 generateQueenMove(int square, U64 occupied);
U64 generateKingMove(int square);
U64 generateEnPassant(State current, State prev);
U64 generateWhitePawnMove(int square, U64 occupied, State p, State prev);
U64 generateBlackPawnMove(int square, U64 occupied, State p, State prev);
U64 pawnPromotion(U64 board);
U64 blackAttackBoard(State p);
U64 whiteAttackBoard(State p);
U64 generateBlackKingCastleMove(State p);
U64 generateWhiteKingCastleMove(State p);
bool isInCheck(State p);

// --------- generate move ----------

// Todo: magic bitboard

// Could be more efficient
// There should be better way of doing this
U64 generateKnightMove(int square) {
    U64 knightMove = 0;
    U64 shifter = (1ULL << square); 

    // does not add invalid moves if knight is on edge of board 
    knightMove |= (shifter << 17) * !(((BIT_8_RANK | BIT_7_RANK) & shifter) || (BIT_H_FILE & shifter)); 
    knightMove |= (shifter << 15) * !(((BIT_8_RANK | BIT_7_RANK) & shifter) || (BIT_A_FILE & (shifter))); 
    knightMove |= (shifter << 6) * !((BIT_8_RANK & shifter) || ((BIT_A_FILE | BIT_B_FILE) & (shifter))); 
    knightMove |= (shifter << 10) * !((BIT_8_RANK & shifter) || ((BIT_G_FILE | BIT_H_FILE) & (shifter)));
    knightMove |= (shifter >> 17) * !(((BIT_1_RANK | BIT_2_RANK) & shifter) || (BIT_A_FILE & (shifter)));
    knightMove |= (shifter >> 15) * !(((BIT_1_RANK | BIT_2_RANK) & shifter) || (BIT_H_FILE & (shifter))); 
    knightMove |= (shifter >> 6) * !(((BIT_1_RANK & shifter) || ((BIT_G_FILE | BIT_H_FILE) & (shifter)))); 
    knightMove |= (shifter >> 10) * !((BIT_1_RANK & shifter) || ((BIT_A_FILE | BIT_B_FILE) & (shifter))); 

    return knightMove;
}

U64 generateRookMove(int square, U64 occupied) {
    U64 rookMove = 0;
    int r, f;

    int file = square % 8;
    int rank = square / 8;

    for (r = rank + 1; r < 8; r++) {
        rookMove |= (1ULL << (r * 8 + file));
        if (occupied & (1ULL << (r * 8 + file))) {
            break;
        }
    }
    
    for (r = rank - 1; r >= 0; r--) {
        rookMove |= (1ULL << (r * 8 + file));
        if (occupied & (1ULL << (r * 8 + file))) {
            break;
        }
    }

    for (f = file + 1; f < 8; f++) {
        rookMove |= (1ULL << (rank * 8 + f));
        if (occupied & (1ULL << (rank * 8 + f))) {
            break;
        }
    }

    for (f = file - 1; f >= 0; f--) {
        rookMove |= (1ULL << (rank * 8 + f));
        if (occupied & (1ULL << (rank * 8 + f))) {
            break;
        }
    }

    return rookMove;
}

U64 generateBishopMove(int square, U64 occupied) {
    U64 bishopMove = 0;
    int r, f;

    int file = square % 8;
    int rank = square / 8;

    for (r = rank + 1, f = file + 1; f < 8 && r < 8; r++, f++) {
        bishopMove |= (1ULL << (r * 8 + f));
        if (occupied & (1ULL << (r * 8 + f))) {
            break;
        }
    }

    for (r = rank + 1, f = file - 1; f >= 0 && r < 8; r++, f--) {
        bishopMove |= (1ULL << (r * 8 + f));
        if (occupied & (1ULL << (r * 8 + f))) {
            break;
        }
    }

    for (r = rank - 1, f = file + 1; f < 8 && r >= 0; r--, f++) {
        bishopMove |= (1ULL << (r * 8 + f));
        if (occupied & (1ULL << (r * 8 + f))) {
            break;
        }
    }

    for (r = rank - 1, f = file - 1; f >= 0 && r >= 0; r--, f--) {
        bishopMove |= (1ULL << (r * 8 + f));
        if (occupied & (1ULL << (r * 8 + f))) {
            break;
        }
    }

    return bishopMove;
}

U64 generateQueenMove(int square, U64 occupied) {
    return (generateBishopMove(square, occupied) | generateRookMove(square, occupied));
}

U64 generateKingMove(int square) {
    U64 kingMove = 0;
    
    kingMove |= (1ULL << (square + 1)) * !(BIT_H_FILE & (1ULL << square));
    kingMove |= (1ULL << (square - 1)) * !(BIT_A_FILE & (1ULL << square));
    kingMove |= (1ULL << (square + 8)) * !(BIT_8_RANK & (1ULL << square));
    kingMove |= (1ULL << (square - 8)) * !(BIT_1_RANK & (1ULL << square));
    kingMove |= (1ULL << (square + 9)) * !((BIT_H_FILE | BIT_8_RANK) & (1ULL << square));
    kingMove |= (1ULL << (square + 7)) * !((BIT_A_FILE | BIT_8_RANK) & (1ULL << square));
    kingMove |= (1ULL << (square - 9)) * !((BIT_1_RANK | BIT_A_FILE) & (1ULL << square));
    kingMove |= (1ULL << (square - 7)) * !((BIT_1_RANK | BIT_H_FILE) & (1ULL << square));

    return kingMove;
}

U64 generateEnPassant(State current, State prev) {
   U64 en_passant = ((((current.wp ^ prev.wp)) & current.wp) >> 8) * (((current.wp ^ prev.wp) & BIT_2_RANK) > 0) * (((current.wp ^ prev.wp) & BIT_4_RANK) > 0);
   en_passant |= ((((current.bp ^ prev.bp)) & current.bp) << 8) * (((current.bp ^ prev.bp) & BIT_7_RANK) > 0) * (((current.bp ^ prev.bp) & BIT_5_RANK) > 0);
   return en_passant; 
}

U64 generateWhitePawnMove(int square, U64 occupied, State p, State prev) {
    U64 whitePawnMove = 0;
    U64 black_board = blackOccupied(p);

    // capture
    whitePawnMove |= ((1ULL << square) << 9) * ((BIT_A_FILE & (1ULL << square) << 9) == 0); 
    whitePawnMove |= ((1ULL << square) << 7) * ((BIT_H_FILE & (1ULL << square) << 7) == 0); 
    whitePawnMove &= black_board;  
    // en-passant
    whitePawnMove |= (((1ULL << square) << 9) & generateEnPassant(p, prev)) * ((BIT_A_FILE & (1ULL << square) << 9) == 0); 
    whitePawnMove |= (((1ULL << square) << 7) & generateEnPassant(p, prev)) * ((BIT_H_FILE & (1ULL << square) << 7) == 0);
    // direction
    whitePawnMove |= (1ULL << square << 8) * (((1ULL << square << 8) & occupied) == 0); 
    // two move initial
    whitePawnMove |= (1ULL << square << 16) * (((1ULL << square & BIT_2_RANK) > 0)) * (((1ULL << square << 16) & occupied) == 0) * (((1ULL << square << 8) & occupied) == 0);
    return whitePawnMove;
}

U64 generateWhitePawnAttack(int square) {
    U64 whitePawnAttack = 0;
    whitePawnAttack |= ((1ULL << square) << 9) * ((BIT_A_FILE & (1ULL << square) << 9) == 0); 
    whitePawnAttack |= ((1ULL << square) << 7) * ((BIT_H_FILE & (1ULL << square) << 7) == 0); 
    return whitePawnAttack;  
}

U64 generateBlackPawnAttack(int square) {
    U64 blackPawnAttack = 0;
    blackPawnAttack |= ((1ULL << square) >> 9) * ((BIT_H_FILE & (1ULL << square) >> 9) == 0); 
    blackPawnAttack |= ((1ULL << square) >> 7) * ((BIT_A_FILE & (1ULL << square) >> 7) == 0); 
    return blackPawnAttack;  
}

U64 generateBlackPawnMove(int square, U64 occupied, State p, State prev) {
    U64 blackPawnMove = 0;
    U64 white_board = whiteOccupied(p); 
    // capture
    blackPawnMove |= ((1ULL << square) >> 9) * ((BIT_H_FILE & (1ULL << square) >> 9) == 0); 
    blackPawnMove |= ((1ULL << square) >> 7) * ((BIT_A_FILE & (1ULL << square) >> 7) == 0); 
    blackPawnMove &= white_board;  
    // en-passant
    blackPawnMove |= (((1ULL << square) >> 9) & generateEnPassant(p, prev)) * ((BIT_H_FILE & (1ULL << square) >> 9) == 0); 
    blackPawnMove |= (((1ULL << square) >> 7) & generateEnPassant(p, prev)) * ((BIT_A_FILE & (1ULL << square) >> 7) == 0);
    // direction
    blackPawnMove |= (1ULL << square >> 8) * (((1ULL << square >> 8) & occupied) == 0); 
    // two move initial
    blackPawnMove |= (1ULL << square >> 16) * ((1ULL << square & BIT_7_RANK) > 0) * (((1ULL << square >> 16) & occupied) == 0) * (((1ULL << square >> 8) & occupied) == 0);
    return blackPawnMove;
}

U64 generateWhiteKingCastleMove(State p) {
    U64 castlingMove = 0;
    U64 occupied = allOccupied(p);
    U64 blackAttack = blackAttackBoard(p);

    if (p.castleState == 0b0000) {
        return 0;
    }

    if (p.castleState & 0b1000) {
        if ( !(occupied & ((1ULL << 5) | (1ULL << 6))) && !(blackAttack & ((1ULL << 4) | (1ULL << 5) | (1ULL << 6))) ) {
            castlingMove |= (1ULL << 6);
        }
    }

    if (p.castleState & 0b0100) {
        if ( !(occupied & ((1ULL << 1) | (1ULL << 2) | (1ULL << 3))) && !(blackAttack & ((1ULL << 2) | (1ULL << 3) | (1ULL << 4))) ) {
            castlingMove |= (1ULL << 2);
        }
    }

    return castlingMove;
}

U64 generateBlackKingCastleMove(State p) {
    U64 castlingMove = 0;
    U64 occupied = allOccupied(p);
    U64 whiteAttack = whiteAttackBoard(p);

    if (p.castleState == 0b0000) {
        return 0;
    }

    if (p.castleState & 0b0010) {
        if ( !(occupied & ((1ULL << 61) | (1ULL << 62))) && !(whiteAttack & ((1ULL << 60) | (1ULL << 61) | (1ULL << 62))) ) {
            castlingMove |= (1ULL << 62);
        }
    }

    if (p.castleState & 0b0001) {
        if ( !(occupied & ((1ULL << 57) | (1ULL << 58) | (1ULL << 59))) && !(whiteAttack & ((1ULL << 58) | (1ULL << 59) | (1ULL << 60))) ) {
            castlingMove |= (1ULL << 58);
        }
    }

    return castlingMove;
}

U64 pawnPromotion(U64 board) {
    // I think this can be combined because white pawn 
    // cannot go to the rank 1 and black pawn cannot go to the 
    // rank 8 
    // but idk if you want to make it so that you can also play really weird Position
    // maybe you need to separate this.

    return 0;
}

// --------------------------------------------------

U64 blackAttackBoard(State p) {
    U64 attackTable = 0;
    U64 occupied = allOccupied(p);
    
    U64 temp = p.bp;
    while(temp) {
        int square = popLSB(&temp);
        attackTable |= generateBlackPawnAttack(square);
    }

    temp = p.bn;
    while(temp) {
        int square = popLSB(&temp);
        attackTable |= generateKnightMove(square);
    }

    temp = p.bb;
    while(temp) {
        int square = popLSB(&temp);
        attackTable |= generateBishopMove(square, occupied);
    }

    temp = p.br;
    while(temp) {
        int square = popLSB(&temp);
        attackTable |= generateRookMove(square, occupied);
    }

    temp = p.bq;
    while(temp) {
        int square = popLSB(&temp);
        attackTable |= generateQueenMove(square, occupied);
    }

    temp = p.bk;
    while(temp) {
        int square = popLSB(&temp);
        attackTable |= generateKingMove(square);
    }

    return (attackTable);
}

U64 whiteAttackBoard(State p) {
    U64 attackTable = 0;
    U64 occupied = allOccupied(p);
    
    U64 temp = p.wp;
    while(temp) {
        int square = popLSB(&temp);
        attackTable |= generateWhitePawnAttack(square);
    }

    temp = p.wn;
    while(temp) {
        int square = popLSB(&temp);
        attackTable |= generateKnightMove(square);
    }

    temp = p.wb;
    while(temp) {
        int square = popLSB(&temp);
        attackTable |= generateBishopMove(square, occupied);
    }

    temp = p.wr;
    while(temp) {
        int square = popLSB(&temp);
        attackTable |= generateRookMove(square, occupied);
    }

    temp = p.wq;
    while(temp) {
        int square = popLSB(&temp);
        attackTable |= generateQueenMove(square, occupied);
    }

    temp = p.wk;
    while(temp) {
        int square = popLSB(&temp);
        attackTable |= generateKingMove(square);
    }

    return (attackTable);
}

bool isInCheck(State p) {
    if (p.turn == WHITE) {
        return ((p.wk & blackAttackBoard(p)) > 0);
    } else {
        return ((p.bk & whiteAttackBoard(p)) > 0);
    }
}
