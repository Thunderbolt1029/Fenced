#include "tiles.h"

#include <stdlib.h>
#include <stdio.h>

#define WHT "\e[0;37m"
#define YEL "\e[0;33m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"

void printTiles(Tile **tiles, int tileSize) {
    for (int y = 0; y < tileSize; y++) {
        for (int x = 0; x < tileSize; x++) {
            switch (tiles[x][y])
            {
            case EMPTY:
                printf(" ");
                break;
            case WALL:
                printf("%s@", WHT);
                break;
            case PLAYER_WALL:
                printf("%s+", YEL);
                break;
            case PLAYER:
                printf("%s#", RED);
                break;
            case FILL:
                printf("%s.", GRN);
                break;
            
            default:
                fprintf(stderr, "UNKOWN TILE %d IN TILES ARRAY AT %d %d\n", tiles[x][y], x, y);
                break;
            }
        }
        printf("\n");
    }
}

void generateTiles(Tile **tiles, int tileSize) {
    for (int x = 0; x < tileSize; x++)
    for (int y = 0; y < tileSize; y++) {
        tiles[x][y] = (rand() % 3 == 0) ? WALL : EMPTY;
    }

    for (int x = (tileSize)/2 - 1; x <= (tileSize)/2 + 1; x++)
    for (int y = (tileSize)/2 - 1; y <= (tileSize)/2 +1; y++) {
        tiles[x][y] = EMPTY;
    }

    tiles[(tileSize)/2][(tileSize)/2] = PLAYER;
}

int removeFill(Tile **tiles, int tileSize) {
    for (int x = 0; x < tileSize; x++)
    for (int y = 0; y < tileSize; y++)
        if (tiles[x][y] == FILL)
            tiles[x][y] = EMPTY;
}

int openAreaRec(Tile **tiles, int tileSize, int x, int y) {
    if (x < 0 || y < 0 || x >= tileSize || y >= tileSize)
        return -1;

    if (tiles[x][y] == EMPTY || tiles[x][y] == PLAYER) {
        int temp, sum = 0;
    
        if (tiles[x][y] == EMPTY) {
            tiles[x][y] = FILL;
            sum++;
        }

        if ((temp = openAreaRec(tiles, tileSize, x+1, y)) == -1)
            return -1;
        sum += temp;
        if ((temp = openAreaRec(tiles, tileSize, x-1, y)) == -1)
            return -1;
        sum += temp;
        if ((temp = openAreaRec(tiles, tileSize, x, y+1)) == -1)
            return -1;
        sum += temp;
        if ((temp = openAreaRec(tiles, tileSize, x, y-1)) == -1)
            return -1;
        sum += temp;

        return sum;
    }

    return 0;
}

int openArea(Tile **tiles, int tileSize, int showFill) {
    removeFill(tiles, tileSize);
    int area = openAreaRec(tiles, tileSize, (tileSize)/2, (tileSize)/2);
    if (!showFill) removeFill(tiles, tileSize);
    return area;
}