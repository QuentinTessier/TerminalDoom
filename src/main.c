/*
** EPITECH PROJECT, 2019
** TerminalDoom
** File description:
** main
*/

#include <stdbool.h>
#include <stdlib.h>
#include <ncurses.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "map.h"
#include "dbuffer.h"
#include "camera.h"

const int nScreenW = 128;
const int nScreenH = 72;

const int nMapWidth = 16;
const int nMapHeight = 9;

WINDOW *init_window(void)
{
    WINDOW *win = 0;

    initscr();
    cbreak();
    noecho();
    win = newwin(nScreenH, nScreenW, 0, 0);
    return (win);
}

void handle_keyboard(WINDOW *win, const char *map, Camera *cam, bool *run)
{
    float rotation = 0.2;
    int key = wgetch(win);

    if (key == 27)
        *run = false;
    if (key == 'q')
        cam->Angle -= rotation;
    if (key == 'd')
        cam->Angle += rotation;
    if (key == 'z') {
        cam->X += sinf(cam->Angle) * cam->speed;
        cam->Y += cosf(cam->Angle) * cam->speed;
        if(map[(int)(cam->X * nMapWidth + cam->Y)] == '#') {
            cam->X -= sinf(cam->Angle) * cam->speed;
            cam->Y -= cosf(cam->Angle) * cam->speed;
        }
    }
    if (key == 's') {
        cam->X -= sinf(cam->Angle) * cam->speed;
        cam->Y -= cosf(cam->Angle) * cam->speed;
        if(map[(int)(cam->X * nMapWidth + cam->Y)] == '#') {
            cam->X += sinf(cam->Angle) * cam->speed;
            cam->Y += cosf(cam->Angle) * cam->speed;
        }
    }
}

void RayCasting(wchar_t *display, const char *map, Camera cam)
{
    float fRayAngle;
    float fSepSize = 0.1f;
    float fDistanceToWall = 0.0f;
    bool bHitWall = false;
    bool bHitBound = false;
    float fEyeX = 0.f;
    float fEyeY = 0.f;

    for (int x = 0; x < nScreenW; x++) {
        fRayAngle = (cam.Angle - cam.FOV / 2.0f) + ((float)x / (float)nScreenW) * cam.FOV;
        fEyeX = sinf(fRayAngle);
        fEyeY = cosf(fRayAngle);
        while (!bHitWall && fDistanceToWall < cam.Depth) {
            fDistanceToWall += fSepSize;
            int nTestX = (int)(cam.X + fEyeX * fDistanceToWall);
            int nTestY = (int)(cam.Y + fEyeY * fDistanceToWall);
            if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight) {
                bHitWall = true;
                fDistanceToWall = cam.Depth;
            } else {
                if (map[nTestX * nMapWidth + nTestY] == '#') {
                    bHitWall = true;
                }
            }
        }
        int nCeiling = (float)(nScreenH / 2.0) - nScreenH / ((float)(fDistanceToWall));
        int nFloor = nScreenH - nCeiling;

        short nShade = ' ';
		if (fDistanceToWall <= cam.Depth / 4.0f)			nShade = '#';
		else if (fDistanceToWall < cam.Depth / 3.0f)		nShade = '$';
		else if (fDistanceToWall < cam.Depth / 2.0f)		nShade = '/';
		else if (fDistanceToWall < cam.Depth)				nShade = ':';
		else											    nShade = ' ';

        for (int y = 0; y < nScreenH; y++) {
            if (y < nCeiling)
                display[y * nScreenW + x] = ' ';
            else if (y > nCeiling && y <= nFloor)
                display[y * nScreenW + x] = nShade;
            else
                display[y * nScreenW + x] = '-';
        }
        fDistanceToWall = 0.0f;
        bHitBound = false;
        bHitWall = false;
    }
}

void display_projection(WINDOW *win, wchar_t *display)
{
    for (int i = 0; i < nScreenH; i++) {
        wprintw(win, "%.*ls", nScreenW, display + i * nScreenW);
    }
}

void display_map(wchar_t *display, const char *map, Camera cam)
{
    int count = 0;

    for (int i = 0; i < nMapHeight; i++) {
        for (int j = 0; j < nMapWidth - 1; j++) {
            display[i * nScreenW + j] = (wchar_t)map[count];
            count++;
        }
        count++;
    }
    display[(int)(cam.Y * nScreenW + cam.X)] = 'P';
}

int game_loop(WINDOW *win, const char *map)
{
    bool run = true;
    wchar_t * display = malloc(sizeof(wchar_t) * nScreenH * nScreenW + 1);;
    Camera cam = Camera_new();

    keypad(win, TRUE);
    memset(display, 0, sizeof(wchar_t) * (nScreenW * nScreenH + 1));
    while (run) {
        clear();
        handle_keyboard(win, map, &cam, &run);
        RayCasting(display, map, cam);
        display_map(display, map, cam);
        display_projection(win, display);
        wmove(win, 0, 0);
        memset(display, 0, sizeof(wchar_t) * (nScreenW * nScreenH + 1));
        refresh();
    }
    return (0);
}

int main(int ac, char **av)
{
    char *map = 0;

    if (ac != 2) {
        dprintf(2, "You need to give a map as argument\n");
        return (1);
    }
    WINDOW *win = init_window();
    map = readFile(av[1]);
    game_loop(win, map);
    endwin();
    free(map);
    return (0);
}