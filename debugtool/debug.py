# gcc -shared -fPIC -o chess.so bitboards.c -lm
# this creates chess.so file that this python script reference
# should use 3.11 as the latest python does not support the library
# python3.11 debug.py

import ctypes
import pygame

chess = ctypes.CDLL("./chess.so")

chess.MG_knight.argtypes = [ctypes.c_uint64]
chess.MG_knight.restype = ctypes.c_uint64

chess.MG_rook.argtypes = [ctypes.c_uint64]
chess.MG_rook.restype = ctypes.c_uint64

chess.MG_bishop.argtypes = [ctypes.c_uint64]
chess.MG_bishop.restype = ctypes.c_uint64

chess.MG_queen.argtypes = [ctypes.c_uint64]
chess.MG_queen.restype = ctypes.c_uint64

pygame.init()
WIDTH = 640
HEIGHT = 640
SQUARE = WIDTH // 8
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Chess Debugger")

WHITE = (240, 217, 181)
BROWN = (181, 136, 99)
HIGHLIGHT = (200, 60, 60)
PIECE = (50, 50, 200)

piece_pos = None
move_bb = 0

def draw_board(piece_pos, move_bb):
    for rank in range(8):
        for file in range(8):
            square = rank * 8 + file
            color = WHITE if ((rank + file) % 2 == 0) else BROWN
            
            if piece_pos == square:
                color = PIECE
            elif (move_bb >> square) & 1:
                color = HIGHLIGHT
            
            x = file * SQUARE
            y = (7 - rank) * SQUARE
            pygame.draw.rect(screen, color, (x, y, SQUARE, SQUARE))

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        
        if event.type == pygame.MOUSEBUTTONDOWN:
            x, y = pygame.mouse.get_pos()
            file = x // SQUARE
            rank = 7 - (y // SQUARE)
            piece_pos = rank * 8 + file
            # move_bb = chess.MG_knight(1 << piece_pos)
            # move_bb = chess.MG_rook(1 << piece_pos)
            # move_bb = chess.MG_bishop(1 << piece_pos)
            move_bb = chess.MG_queen(1 << piece_pos)

    draw_board(piece_pos, move_bb)
    pygame.display.flip()

pygame.quit()
