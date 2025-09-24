#include <stdlib.h>
#include <string.h>

#include <uniwidth.h>

#include "utils.h"

int unicode_display_width(const char *str)
{
    if (!str)
        return 0;

    size_t len = strlen(str);
    if (len == 0)
        return 0;

    // Handle multiline strings by finding the maximum line width
    const char *line_start = str;
    const char *line_end;
    int         max_width = 0;

    while ((line_end = strchr(line_start, '\n')) != NULL)
    {
        size_t line_len = line_end - line_start;
        if (line_len > 0)
        {
            int line_width = u8_width((const uint8_t *)line_start, line_len, "UTF-8");
            if (line_width < 0)
                line_width = (int)line_len;
            if (line_width > max_width)
                max_width = line_width;
        }
        line_start = line_end + 1;
    }

    // Handle the last line (or the only line if no newlines)
    size_t last_line_len = strlen(line_start);
    if (last_line_len > 0)
    {
        int last_line_width = u8_width((const uint8_t *)line_start, last_line_len, "UTF-8");
        if (last_line_width < 0)
            last_line_width = (int)last_line_len;
        if (last_line_width > max_width)
            max_width = last_line_width;
    }

    return max_width;
}