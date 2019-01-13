#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "map.h"

char *readFile(const char *path)
{
        int fd = open(path, O_RDONLY);
        char *contents = 0;
        char buffer[2048] = {0};
        ssize_t bytes = 1;

        if (fd == -1)
                return (0);
        contents = strdup("");
        bytes = read(fd, buffer, 2048);
        while (bytes > 0) {
                buffer[bytes] = 0;
                contents = (char *)realloc(contents,
                                        sizeof(char) *
                                                strlen(contents) + strlen(buffer) + 1);
                strcat(contents, buffer);
                bytes = read(fd, buffer, 2048);
        }
        if (bytes == -1) {
                free(contents);
                contents = 0;
        }
        return (contents);
}