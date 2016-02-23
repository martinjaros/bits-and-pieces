#!/usr/bin/env python

def game_of_life(grid):
    def neighbors(cell):
        x, y = cell
        yield (x + 1, y)
        yield (x + 1, y + 1)
        yield (x,     y + 1)
        yield (x - 1, y + 1)
        yield (x - 1, y)
        yield (x - 1, y - 1)
        yield (x,     y - 1)
        yield (x + 1, y - 1)

    while True:
        generation = set()
        for cell in (nbr for cell in grid for nbr in neighbors(cell) if nbr not in generation):
            num = sum((nbr in grid) for nbr in neighbors(cell))
            if (cell in grid) and (num > 1 and num < 4) or (num == 3):
                generation.add(cell)

        yield generation
        grid = generation

if __name__ == "__main__":
    import pygame, pygame.gfxdraw

    pygame.init()
    pygame.display.set_caption("Conway's Game of Life")
    screen = pygame.display.set_mode((640, 480))
    surface = pygame.Surface((320, 240))
    bgcolor = pygame.Color(63, 63, 63)
    fgcolor = pygame.Color(255, 0, 0)

    glidergun = {
        (  6, -4), ( 4, -3), ( 6, -3), ( -6, -2), ( -5, -2), ( 2, -2), (  3, -2), ( 16, -2), ( 17, -2),
        ( -7, -1), (-3, -1), ( 2, -1), (  3, -1), ( 16, -1), (17, -1), (-18,  0), (-17,  0), ( -8,  0),
        ( -2,  0), ( 2,  0), ( 3,  0), (-18,  1), (-17,  1), (-8,  1), ( -4,  1), ( -2,  1), ( -1,  1),
        (  4,  1), ( 6,  1), (-8,  2), ( -2,  2), (  6,  2), (-7,  3), ( -3,  3), ( -6,  4), ( -5,  4) }

    grid = { (100 + x, 100 + y) for (x, y) in glidergun } \
         | { (200 - x, 101 + y) for (x, y) in glidergun }

    for generation in game_of_life(grid):
        surface.fill(bgcolor)
        for (x, y) in generation:
            pygame.gfxdraw.pixel(surface, x, y, fgcolor)

        pygame.transform.scale(surface, screen.get_size(), screen)
        pygame.display.flip()
        pygame.time.wait(50)
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                raise SystemExit

