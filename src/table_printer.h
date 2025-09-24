#ifndef TABLE_PRINTER_H
#define TABLE_PRINTER_H

#include "cli.h"
#include "csv_parser.h"

typedef struct
{
    char *inner;  // Inner row separator (e.g., "─")
    char *ljunc;  // Left junction (e.g., "├")
    char *cjunc;  // Cross junction (e.g., "┼")
    char *rjunc;  // Right junction (e.g., "┤")
} row_sep_t;

typedef struct
{
    row_sep_t *top;  // Top border
    row_sep_t *snd;  // Second row (header separator)
    row_sep_t *mid;  // Middle rows (between data rows)
    row_sep_t *bot;  // Bottom border
} row_seps_t;

typedef struct
{
    char *lhs;  // Left border
    char *mid;  // Column separator
    char *rhs;  // Right border
} col_seps_t;

typedef struct
{
    col_seps_t  col_seps;
    row_seps_t  row_seps;
    int         padding;
    int         indent;
    alignment_t header_align;
    alignment_t body_align;
} table_format_t;

int             print_table(struct csv_data *csv, struct cli_args *args);
table_format_t *create_table_style(table_style_t style_type,
                                   int           padding,
                                   int           indent,
                                   alignment_t   header_align,
                                   alignment_t   body_align);
void            free_table_style(table_format_t *style);

#endif  // TABLE_PRINTER_H