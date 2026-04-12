#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include "types.h"

bool parseLAN(char *input, int *fromSquare, int *toSquare);
State fenToState(char *fen);

#endif
