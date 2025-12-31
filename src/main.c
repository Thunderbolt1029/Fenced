#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>
#include <unistd.h>

#include "tiles.h"

#define TILES_SIZE 15

#define TARGET_FRAME_TIME 0.01
#define CURS_BLINK_SPEED 0.5

#define SCORE(area, placedWalls) ((area) - 7*(placedWalls))

void Init(WINDOW *win, long seed);
void destroyTiles();
int Update(WINDOW *win, double deltaTime);
void Draw(WINDOW *win);

long long timeInMilliseconds(void) 
{
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

int quit = 0;

Tile **tiles;
int placedWalls;

int main(int argc, char **argv)
{
    long seed;
    if (argc > 1)
        seed = atol(argv[1]);
    else 
        seed = time(NULL);

    WINDOW *GameWin;

    initscr();
    start_color();

    init_pair(EMPTY, COLOR_WHITE, COLOR_BLACK);
    init_pair(WALL, COLOR_WHITE, COLOR_BLACK);
    init_pair(PLAYER_WALL, COLOR_YELLOW, COLOR_BLACK);
    init_pair(PLAYER, COLOR_RED, COLOR_BLACK);
    init_pair(FILL, COLOR_GREEN, COLOR_BLACK);

    clear();
    // raw();
    noecho();
    curs_set(0);

    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    
    GameWin = newwin(TILES_SIZE+2, TILES_SIZE*2+2, (maxY - TILES_SIZE) / 2 -1, (maxX - TILES_SIZE*2) / 2 -1);
    nodelay(GameWin, TRUE);
    keypad(GameWin, TRUE);

    Init(GameWin, seed);

    double deltaTime = 0;
    long long start, frameTime;
    while (!quit)
    {
        start = timeInMilliseconds();

        if (Update(GameWin, deltaTime))
            Draw(GameWin);

        frameTime = timeInMilliseconds() - start;
        if (frameTime < TARGET_FRAME_TIME * 1000)
            usleep((TARGET_FRAME_TIME * 1000 - frameTime) * 1000);

        deltaTime = (double)(timeInMilliseconds() - start) / 1000;
    }

    nodelay(GameWin, FALSE);
    endwin();

    int area = openArea(tiles, TILES_SIZE, 1);
    if (area == -1)
        removeFill(tiles, TILES_SIZE);

    printf("Size: %d\n", TILES_SIZE);
    printf("Seed: %ld\n", seed);
    printf("Placed walls: %d\n", placedWalls);
    if (area != -1) { 
        printf("Area: %d\n", area);
        printf("Score: %d\n", SCORE(area, placedWalls));
        printf("Solution:\n");
    }
    else
        printf("Incompleted:\n");
    printTiles(tiles, TILES_SIZE);

    destroyTiles();

    return 0;
}

int cursX, cursY;
int cursState = 1;
double cursBlink = CURS_BLINK_SPEED;

void Init(WINDOW *win, long seed) {
    srand(seed);

    tiles = malloc(sizeof(Tile *) * TILES_SIZE);
    for (int i = 0; i < TILES_SIZE; i++)
        tiles[i] = malloc(sizeof(Tile) * TILES_SIZE);


    int area;
    do {
        generateTiles(tiles, TILES_SIZE);
        area = openArea(tiles, TILES_SIZE, 0);
    } while (area != -1);

    cursX = 0; cursY = 0;
    placedWalls = 0;

    for (int x = 0; x < TILES_SIZE * 2 +1; x++)
    {
        mvwaddch(win, 0, x, ACS_HLINE);
        mvwaddch(win, TILES_SIZE+1, x, ACS_HLINE);
    }
    for (int y = 0; y < TILES_SIZE+1; y++)
    {
        mvwaddch(win, y, 0, ACS_VLINE);
        mvwaddch(win, y, TILES_SIZE*2+1, ACS_VLINE);
    }
    mvwaddch(win, 0, 0, ACS_ULCORNER);
    mvwaddch(win, 0, TILES_SIZE*2+1, ACS_URCORNER);
    mvwaddch(win, TILES_SIZE+1, 0, ACS_LLCORNER);
    mvwaddch(win, TILES_SIZE+1, TILES_SIZE*2+1, ACS_LRCORNER);

    mvwprintw(win, 0, 2, "Seed %ld", seed);

    Draw(win);
}

void destroyTiles() {
    for (int i = 0; i < TILES_SIZE; i++)
        free(tiles[i]);
    free(tiles);
}


int Update(WINDOW *win, double deltaTime) {
    int ret = 0;

    cursBlink -= deltaTime;
    if (cursBlink < 0) {
        cursBlink = CURS_BLINK_SPEED;
        cursState = !cursState;
        ret = 1;
    }

    int inp = wgetch(win);
    switch (inp)
    {
        case 'q':
        case 'Q':
            quit = 1;
            return 0;

        case KEY_LEFT:
            if (cursX == 0) break;
            cursX--;
            ret = 1;
            break;
        case KEY_RIGHT:
            if (cursX == TILES_SIZE-1) break;
            cursX++;
            ret = 1;
            break;
        case KEY_UP:
            if (cursY == 0) break;
            cursY--;
            ret = 1;
            break;
        case KEY_DOWN:
            if (cursY == TILES_SIZE-1) break;
            cursY++;
            ret = 1;
            break;

        case ' ':
            if (tiles[cursX][cursY] == EMPTY || tiles[cursX][cursY] == FILL) {
                placedWalls++;
                tiles[cursX][cursY] = PLAYER_WALL;
                ret = 1;
            }
            else if (tiles[cursX][cursY] == PLAYER_WALL) {
                placedWalls--;
                tiles[cursX][cursY] = EMPTY;
                removeFill(tiles, TILES_SIZE);
                ret = 1;
            }
        
        default:
            break;
    }

    return ret;
}

void Draw(WINDOW *win) {
    for (int x = 1; x < TILES_SIZE * 2 +1; x++) {
        mvwaddch(win, TILES_SIZE+1, x, ACS_HLINE);
    }
    int area = openArea(tiles, TILES_SIZE, 1);
    if (area == -1) {
        removeFill(tiles, TILES_SIZE);
        mvwprintw(win, TILES_SIZE+1, 2, "Score : open");
    }
    else
        mvwprintw(win, TILES_SIZE+1, 2, "Score : %d", SCORE(area, placedWalls));


    for (int x = 0; x < TILES_SIZE; x++)
    for (int y = 0; y < TILES_SIZE; y++) {
        int c = (cursState && cursX == x && cursY == y) ? ACS_CKBOARD : tiles[x][y] ? (' ' | A_REVERSE) : ' ';

        wattron(win, COLOR_PAIR(tiles[x][y]));
        mvwaddch(win, y+1, x*2+1, c);
        mvwaddch(win, y+1, x*2+2, c);
        wattroff(win, COLOR_PAIR(tiles[x][y]));
    }
}
