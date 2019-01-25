#include <stdbool.h>
#include <stdlib.h>
#include <ncurses.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <unistd.h>
#include <fcntl.h>
#include "map.h"
#include "dbuffer.h"
#include "camera.h"

WINDOW *init_window(void)
{
    WINDOW *win = 0;

    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    win = newwin(LINES, COLS, 0, 0);
    return (win);
}

void handle_keyboard(WINDOW *win, Camera *cam, bool *run, DataMap map)
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
        if (map.map[(int)cam->Y * map.X + (int)cam->X] == '#') {
            cam->X -= sinf(cam->Angle) * cam->speed;
            cam->Y -= cosf(cam->Angle) * cam->speed;
        }
    }
    if (key == 's') {
        cam->X -= sinf(cam->Angle) * cam->speed;
        cam->Y -= cosf(cam->Angle) * cam->speed;
        if (map.map[(int)cam->Y * map.X + (int)cam->X] == '#') {
            cam->X += sinf(cam->Angle) * cam->speed;
            cam->Y += cosf(cam->Angle) * cam->speed;
        }
    }
}

void RayCasting(wchar_t *display, DataMap m, Camera cam)
{
    float fRayAngle;
    float fSepSize = 0.1f;
    float fDistanceToWall = 0.0f;
    bool bHitWall = false;
    bool bHitBound = false;
    float fEyeX = 0.f;
    float fEyeY = 0.f;
    wchar_t nShade = ' ';

    for (int x = 0; x < COLS; x++) {
        fRayAngle = (cam.Angle - cam.FOV / 2.0f) + ((float)x / (float)COLS) * cam.FOV;
        fEyeX = sinf(fRayAngle);
        fEyeY = cosf(fRayAngle);
        while (!bHitWall && fDistanceToWall < cam.Depth) {
            fDistanceToWall += fSepSize;
            int nTestX = (int)(cam.X + fEyeX * fDistanceToWall);
            int nTestY = (int)(cam.Y + fEyeY * fDistanceToWall);
            if (nTestX < 0 || nTestX >= m.X || nTestY < 0 || nTestY >= m.Y) {
                bHitWall = true;
                fDistanceToWall = cam.Depth;
            } else {
                if (m.map[nTestY * m.X + nTestX] == '#') {
                    bHitWall = true;
                }
            }
        }
        int nCeiling = (float)(LINES / 2.0) - LINES / ((float)(fDistanceToWall));
        int nFloor = LINES - nCeiling;

        nShade = ' ';
		if (fDistanceToWall <= cam.Depth / 4.0f)			nShade = '$';
		else if (fDistanceToWall < cam.Depth / 3.0f)		nShade = '#';
		else if (fDistanceToWall < cam.Depth / 2.0f)		nShade = '/';
		else if (fDistanceToWall < cam.Depth)				nShade = ':';
		else											    nShade = ' ';

        for (int y = 0; y < LINES; y++) {
            if (y < nCeiling)
                display[y * COLS + x] = ' ';
            else if (y > nCeiling && y <= nFloor)
                display[y * COLS + x] = nShade;
            else {
                float b = 1.0f - (((float)y - LINES / 2.0f) / ((float)LINES / 2.0f));
                wchar_t floor_Shade = ' ';
                if (b < 0.25)               floor_Shade = '-';
                else if (b < 0.5)          floor_Shade = '-';
                else if (b < 0.75)          floor_Shade = '.';
                else if (b < 0.9)          floor_Shade = '.';
                else                        floor_Shade = ' ';
                display[y * COLS + x] = floor_Shade;
            }
        }
        fDistanceToWall = 0.0f;
        bHitBound = false;
        bHitWall = false;
    }
}

void display_projection(WINDOW *win, wchar_t *display)
{
    for (int i = 0; i < LINES; i++) {
        wprintw(win, "%.*ls", COLS, display + i * COLS);
    }
}

void display_map(wchar_t *display, DataMap m, Camera cam)
{
    int count = 0;

    for (int i = 1; i < m.Y; i++) {
        for (int j = 0; j < m.X - 1; j++) {
            display[i * COLS + j] = (wchar_t)m.map[count];
            count++;
        }
        count++;
    }
    display[(int)(((int)cam.Y + 1) * COLS + (int)cam.X)] = 'P';
}

int game_loop(WINDOW *win, DataMap m)
{
    bool run = true;
    wchar_t * display = malloc(sizeof(wchar_t) * LINES * 2 * COLS * 2 + 1);;
    Camera cam = Camera_new();

    keypad(win, TRUE);
    memset(display, 0, sizeof(wchar_t) * (LINES * COLS + 1));
    while (run) {
        clear();
        handle_keyboard(win, &cam, &run, m);
        RayCasting(display, m, cam);
        display_map(display, m, cam);
        display_projection(win, display);
        init_pair(1, COLOR_RED, COLOR_BLACK);
        attron(COLOR_PAIR(1));
        mvwprintw(win, 0, 0, "(X: %.2f, Y: %.2f), Angle: %.2f", cam.X, cam.Y, cam.Angle);
        attroff(COLOR_PAIR(1));
        wmove(win, 0, 0);
        memset(display, 0, sizeof(wchar_t) * (COLS * LINES + 1));
        refresh();
    }
    return (0);
}

int help(const char *av)
{
    char buffer[300];
    ssize_t r;

    if (strcmp(av, "-h") == 0) {
        int fd = open("./src/help.conf", O_RDONLY);
        r = read(fd, buffer, 300);
        write(1, buffer, r);
        return (1);
    }
    return (0);
}

int main(int ac, char **av)
{
    DataMap m;

    setlocale(LC_ALL, "");
    if (ac != 2) {
        dprintf(2, "You need to give a map as argument\n");
        return (1);
    }
    if (help(av[1]))
        return (0);
    WINDOW *win = init_window();
    if (has_colors() == FALSE) {
        endwin();
        dprintf(2, "Your terminal doens't support color\n");
        return (1);
    }
    m = DataMap_new(av[1]);
    game_loop(win, m);
    endwin();
    free(m.map);
    return (0);
}