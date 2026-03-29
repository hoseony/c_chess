# gcc -shared -fPIC -o debugtool/chess.so src/bitboards.c -lm
# python3.11 debugtool/debug.py

import ctypes
import pygame

chess = ctypes.CDLL("./chess.so")

chess.generateKnightMove.argtypes = [ctypes.c_int]
chess.generateKnightMove.restype = ctypes.c_uint64

chess.generateRookMove.argtypes = [ctypes.c_int]
chess.generateRookMove.restype = ctypes.c_uint64

chess.generateBishopMove.argtypes = [ctypes.c_int, ctypes.c_uint64]
chess.generateBishopMove.restype = ctypes.c_uint64

chess.generateQueenMove.argtypes = [ctypes.c_int]
chess.generateQueenMove.restype = ctypes.c_uint64

pygame.init()
WIDTH = 640
HEIGHT = 700
BOARD = 640
SQUARE = BOARD // 8
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Chess Debugger")

WHITE    = (240, 217, 181)
BROWN    = (181, 136, 99)
HUD_BG   = (30,  30,  30)
HUD_TEXT = (220, 220, 220)

PIECES = {
    pygame.K_n: ("Knight", lambda sq, occ: int(chess.generateKnightMove(sq))),
    pygame.K_r: ("Rook",   lambda sq, occ: int(chess.generateRookMove(sq, ctypes.c_uint64(occ)))),
    pygame.K_b: ("Bishop", lambda sq, occ: int(chess.generateBishopMove(sq, ctypes.c_uint64(occ)))),
    pygame.K_q: ("Queen",  lambda sq, occ: int(chess.generateQueenMove(sq, ctypes.c_uint64(occ)))),
}

piece_pos       = None
move_bb         = 0
occ             = 0
placing_blocker = False
selected_piece  = "Bishop"
selected_fn     = PIECES[pygame.K_b][1]

font_sm = pygame.font.SysFont(None, 22)
font_md = pygame.font.SysFont(None, 26)

def draw_x(surface, color, cx, cy, size, thickness):
    pad = size // 4
    pygame.draw.line(surface, color, (cx - pad, cy - pad), (cx + pad, cy + pad), thickness)
    pygame.draw.line(surface, color, (cx + pad, cy - pad), (cx - pad, cy + pad), thickness)

def draw_board():
    for rank in range(8):
        for file in range(8):
            square = rank * 8 + file
            color = WHITE if ((rank + file) % 2 == 0) else BROWN

            x = file * SQUARE
            y = (7 - rank) * SQUARE
            cx = x + SQUARE // 2
            cy = y + SQUARE // 2

            if (move_bb >> square) & 1:
                color = (200, 60, 60)

            pygame.draw.rect(screen, color, (x, y, SQUARE, SQUARE))

            # blue X for piece
            if piece_pos == square:
                draw_x(screen, (50, 100, 220), cx, cy, SQUARE, 4)

            # yellow X only for occupied blockers
            if (occ >> square) & 1:
                draw_x(screen, (220, 200, 30), cx, cy, SQUARE, 4)

    for rank in range(8):
        label = font_sm.render(str(rank + 1), True, (80, 80, 80))
        screen.blit(label, (4, (7 - rank) * SQUARE + 4))

    for file in range(8):
        label = font_sm.render(chr(ord('a') + file), True, (80, 80, 80))
        screen.blit(label, (file * SQUARE + SQUARE - 14, BOARD - 18))

def draw_hud():
    pygame.draw.rect(screen, HUD_BG, (0, BOARD, WIDTH, HEIGHT - BOARD))

    mode = "BLOCKER mode (space)" if placing_blocker else "PIECE mode"
    occ_hex = f"occ: 0x{occ:016X}"
    piece_label = f"Piece: {selected_piece}  |  N / R / B / Q  |  C to clear  |  {mode}"

    screen.blit(font_md.render(piece_label, True, HUD_TEXT), (8, BOARD + 8))
    screen.blit(font_sm.render(occ_hex, True, (160, 160, 160)), (8, BOARD + 34))

    if piece_pos is not None:
        file = piece_pos % 8
        rank = piece_pos // 8
        sq_label = f"square: {chr(ord('a') + file)}{rank + 1}  (index {piece_pos})"
        screen.blit(font_sm.render(sq_label, True, (160, 160, 160)), (340, BOARD + 34))

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

        if event.type == pygame.KEYDOWN:
            if event.key in PIECES:
                selected_piece, selected_fn = PIECES[event.key]
                if piece_pos is not None:
                    move_bb = selected_fn(piece_pos, occ)

            elif event.key == pygame.K_SPACE:
                placing_blocker = True

            elif event.key == pygame.K_c:
                occ = 0
                move_bb = 0
                piece_pos = None

        if event.type == pygame.KEYUP:
            if event.key == pygame.K_SPACE:
                placing_blocker = False

        if event.type == pygame.MOUSEBUTTONDOWN:
            x, y = pygame.mouse.get_pos()
            if y < BOARD:
                file   = x // SQUARE
                rank   = 7 - (y // SQUARE)
                square = rank * 8 + file

                if placing_blocker:
                    occ ^= (1 << square)
                    if piece_pos is not None:
                        move_bb = selected_fn(piece_pos, occ)
                else:
                    piece_pos = square
                    move_bb = selected_fn(piece_pos, occ)

    screen.fill(HUD_BG)
    draw_board()
    draw_hud()
    pygame.display.flip()

pygame.quit()
