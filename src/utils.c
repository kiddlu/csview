#include <stdlib.h>
#include <string.h>
#include <uniwidth.h>

#include "utils.h"

int unicode_display_width(const char *str) {
    if (!str) return 0;

    size_t len = strlen(str);
    if (len == 0) return 0;

    // Use libunistring's u8_width function for UTF-8 strings
    int width = u8_width((const uint8_t *)str, len, "UTF-8");

    // If u8_width fails, fall back to byte length
    return width >= 0 ? width : (int)len;
}