#ifndef CLI_H
#define CLI_H

#include <stdbool.h>

typedef enum
{
    STYLE_NONE,
    STYLE_ASCII,
    STYLE_ASCII2,
    STYLE_SHARP,
    STYLE_ROUNDED,
    STYLE_REINFORCED,
    STYLE_MARKDOWN,
    STYLE_GRID
} table_style_t;

typedef enum
{
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
} alignment_t;

struct cli_args
{
    char         *file;
    bool          no_headers;
    bool          number;
    bool          tsv;
    char          delimiter;
    table_style_t style;
    int           padding;
    int           indent;
    int           sniff;
    alignment_t   header_align;
    alignment_t   body_align;
    bool          disable_pager;
    bool          help;
    bool          version;
};

int  parse_cli_args(int argc, char *argv[], struct cli_args *args);
void print_help(const char *program_name);
void free_cli_args(struct cli_args *args);
void setup_pager(bool disable_pager);
void wait_for_pager(void);

#endif  // CLI_H