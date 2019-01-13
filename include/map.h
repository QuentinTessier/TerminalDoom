#pragma once

#include <wchar.h>

typedef struct DataMap {
    char *map;
    int X;
    int Y;
} DataMap;

DataMap DataMap_new(const char *path);
char *readFile(const char *path);