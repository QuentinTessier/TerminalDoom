#include <stdlib.h>
#include <wchar.h>
#include <ncurses.h>
#include "dbuffer.h"

void Update_dBuffer(wchar_t * *display)
{
    *display = realloc(*display, COLS * LINES + 1);
}