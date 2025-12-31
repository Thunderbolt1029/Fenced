#ifndef TILES_H
#define TILES_H

typedef enum {
    EMPTY,
    WALL,
    PLAYER_WALL,
    PLAYER,
    FILL
} Tile;

void printTiles(Tile **tiles, int tileSize);
void generateTiles(Tile **tiles, int tileSize);
int removeFill(Tile **tiles, int tileSize);
int openArea(Tile **tiles, int tileSize, int showFill);

#endif
