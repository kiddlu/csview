#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "table_printer.h"
#include "utils.h"

static row_sep_t *create_row_sep(const char *inner,
                                 const char *ljunc,
                                 const char *cjunc,
                                 const char *rjunc)
{
    row_sep_t *sep = malloc(sizeof(row_sep_t));
    if (!sep)
        return NULL;
    sep->inner = strdup(inner);
    sep->ljunc = strdup(ljunc);
    sep->cjunc = strdup(cjunc);
    sep->rjunc = strdup(rjunc);
    return sep;
}

static col_seps_t create_col_seps(const char *lhs, const char *mid, const char *rhs)
{
    col_seps_t seps;
    seps.lhs = lhs ? strdup(lhs) : NULL;
    seps.mid = mid ? strdup(mid) : NULL;
    seps.rhs = rhs ? strdup(rhs) : NULL;
    return seps;
}

table_format_t *create_table_style(table_style_t style_type,
                                   int           padding,
                                   int           indent,
                                   alignment_t   header_align,
                                   alignment_t   body_align)
{
    table_format_t *style = malloc(sizeof(table_format_t));
    if (!style)
        return NULL;

    style->padding      = padding;
    style->indent       = indent;
    style->header_align = header_align;
    style->body_align   = body_align;

    // Initialize row separators to NULL
    style->row_seps.top = NULL;
    style->row_seps.snd = NULL;
    style->row_seps.mid = NULL;
    style->row_seps.bot = NULL;

    switch (style_type) {
        case STYLE_NONE:
            style->col_seps = create_col_seps(NULL, NULL, NULL);
            break;

        case STYLE_ASCII:
            style->col_seps     = create_col_seps("|", "|", "|");
            style->row_seps.top = create_row_sep("-", "+", "+", "+");
            style->row_seps.snd = create_row_sep("-", "+", "+", "+");
            style->row_seps.mid = NULL;
            style->row_seps.bot = create_row_sep("-", "+", "+", "+");
            break;

        case STYLE_ASCII2:
            style->col_seps     = create_col_seps(" ", "|", " ");
            style->row_seps.top = NULL;
            style->row_seps.snd = create_row_sep("-", " ", "+", " ");
            style->row_seps.mid = NULL;
            style->row_seps.bot = NULL;
            break;

        case STYLE_SHARP:
            style->col_seps     = create_col_seps("│", "│", "│");
            style->row_seps.top = create_row_sep("─", "┌", "┬", "┐");
            style->row_seps.snd = create_row_sep("─", "├", "┼", "┤");
            style->row_seps.mid = NULL;
            style->row_seps.bot = create_row_sep("─", "└", "┴", "┘");
            break;

        case STYLE_ROUNDED:
            style->col_seps     = create_col_seps("│", "│", "│");
            style->row_seps.top = create_row_sep("─", "╭", "┬", "╮");
            style->row_seps.snd = create_row_sep("─", "├", "┼", "┤");
            style->row_seps.mid = NULL;
            style->row_seps.bot = create_row_sep("─", "╰", "┴", "╯");
            break;

        case STYLE_REINFORCED:
            style->col_seps     = create_col_seps("│", "│", "│");
            style->row_seps.top = create_row_sep("─", "┏", "┬", "┓");
            style->row_seps.snd = create_row_sep("─", "├", "┼", "┤");
            style->row_seps.mid = NULL;
            style->row_seps.bot = create_row_sep("─", "┗", "┴", "┛");
            break;

        case STYLE_MARKDOWN:
            style->col_seps     = create_col_seps("|", "|", "|");
            style->row_seps.top = NULL;
            style->row_seps.snd = create_row_sep("-", "|", "|", "|");
            style->row_seps.mid = NULL;
            style->row_seps.bot = NULL;
            break;

        case STYLE_GRID:
            style->col_seps     = create_col_seps("│", "│", "│");
            style->row_seps.top = create_row_sep("─", "┌", "┬", "┐");
            style->row_seps.snd = create_row_sep("─", "├", "┼", "┤");
            style->row_seps.mid = create_row_sep("─", "├", "┼", "┤");
            style->row_seps.bot = create_row_sep("─", "└", "┴", "┘");
            break;

        default:
            // Default to SHARP style
            style->col_seps     = create_col_seps("│", "│", "│");
            style->row_seps.top = create_row_sep("─", "┌", "┬", "┐");
            style->row_seps.snd = create_row_sep("─", "├", "┼", "┤");
            style->row_seps.mid = NULL;
            style->row_seps.bot = create_row_sep("─", "└", "┴", "┘");
            break;
    }

    return style;
}

void free_table_style(table_format_t *style)
{
    if (style) {
        free(style->col_seps.lhs);
        free(style->col_seps.mid);
        free(style->col_seps.rhs);

        if (style->row_seps.top) {
            free(style->row_seps.top->inner);
            free(style->row_seps.top->ljunc);
            free(style->row_seps.top->cjunc);
            free(style->row_seps.top->rjunc);
            free(style->row_seps.top);
        }

        if (style->row_seps.snd) {
            free(style->row_seps.snd->inner);
            free(style->row_seps.snd->ljunc);
            free(style->row_seps.snd->cjunc);
            free(style->row_seps.snd->rjunc);
            free(style->row_seps.snd);
        }

        if (style->row_seps.mid) {
            free(style->row_seps.mid->inner);
            free(style->row_seps.mid->ljunc);
            free(style->row_seps.mid->cjunc);
            free(style->row_seps.mid->rjunc);
            free(style->row_seps.mid);
        }

        if (style->row_seps.bot) {
            free(style->row_seps.bot->inner);
            free(style->row_seps.bot->ljunc);
            free(style->row_seps.bot->cjunc);
            free(style->row_seps.bot->rjunc);
            free(style->row_seps.bot);
        }

        free(style);
    }
}

static char *calc_pad(const char *str, int width, int align, bool truncate)
{
    if (!str)
        return NULL;

    int str_width = unicode_display_width(str);

    // If string is already wider than target width and truncate is enabled
    if (truncate && str_width > width) {
        // Simple byte-based truncation for now
        // TODO: Could use libunistring's uc_truncate for proper Unicode truncation
        size_t max_bytes = width < (int)strlen(str) ? (size_t)width : strlen(str);
        char  *result    = malloc(max_bytes + 1);
        if (!result)
            return NULL;
        strncpy(result, str, max_bytes);
        result[max_bytes] = '\0';
        return result;
    }

    if (str_width >= width) {
        return strdup(str);
    }

    int padding = width - str_width;
    int left_pad, right_pad;

    switch (align) {
        case 0:  // Left
            left_pad  = 0;
            right_pad = padding;
            break;
        case 1:  // Center
            left_pad  = padding / 2;
            right_pad = padding - left_pad;
            break;
        case 2:  // Right
            left_pad  = padding;
            right_pad = 0;
            break;
        default:
            left_pad  = 0;
            right_pad = padding;
            break;
    }

    // Calculate required buffer size: string bytes + padding spaces + null terminator
    size_t str_bytes  = strlen(str);
    size_t total_size = str_bytes + (size_t)left_pad + (size_t)right_pad + 1;

    char *result = malloc(total_size);
    if (!result)
        return NULL;

    char *p = result;

    // Add left padding
    for (int i = 0; i < left_pad; i++) {
        *p++ = ' ';
    }

    // Add string
    strcpy(p, str);
    p += str_bytes;

    // Add right padding
    for (int i = 0; i < right_pad; i++) {
        *p++ = ' ';
    }

    *p = '\0';

    return result;
}

static void print_row_separator(FILE           *output,
                                table_format_t *style,
                                int            *widths,
                                int             col_count,
                                row_sep_t      *sep)
{
    if (!sep)
        return;

    // Print indent
    for (int i = 0; i < style->indent; i++) {
        fputc(' ', output);
    }

    // Print left junction
    if (style->col_seps.lhs) {
        fputs(sep->ljunc, output);
    }

    // Print column separators
    for (int i = 0; i < col_count; i++) {
        for (int j = 0; j < widths[i] + style->padding * 2; j++) {
            fputs(sep->inner, output);
        }
        if (style->col_seps.mid && i < col_count - 1) {
            fputs(sep->cjunc, output);
        }
    }

    // Print right junction
    if (style->col_seps.rhs) {
        fputs(sep->rjunc, output);
    }

    fputc('\n', output);
}

static void print_row(FILE           *output,
                      table_format_t *style,
                      char          **fields,
                      int             field_count,
                      int            *widths,
                      int             col_count,
                      alignment_t     align,
                      int             row_number)
{
    // Print indent
    for (int i = 0; i < style->indent; i++) {
        fputc(' ', output);
    }

    // Print left border
    if (style->col_seps.lhs) {
        fputs(style->col_seps.lhs, output);
    }

    // Print cells
    for (int i = 0; i < col_count; i++) {
        // Print padding
        for (int j = 0; j < style->padding; j++) {
            fputc(' ', output);
        }

        // Get field content
        char *content;
        if (i == 0 && row_number != 0) {
            // Sequence number column
            if (row_number == -1) {
                // Header row with sequence numbers
                content = strdup("#");
            } else {
                // Data row with sequence numbers
                content = malloc(20);
                snprintf(content, 20, "%d", row_number);
            }
        } else {
            int field_index = (row_number != 0) ? i - 1 : i;
            if (field_index >= 0 && field_index < field_count && fields[field_index]) {
                content = strdup(fields[field_index]);
            } else {
                content = strdup("");
            }
        }

        // Pad content to column width
        char *padded = calc_pad(content, widths[i], align, true);
        if (padded) {
            fputs(padded, output);
            free(padded);
        }
        free(content);

        // Print padding
        for (int j = 0; j < style->padding; j++) {
            fputc(' ', output);
        }

        // Print column separator
        if (style->col_seps.mid && i < col_count - 1) {
            fputs(style->col_seps.mid, output);
        }
    }

    // Print right border
    if (style->col_seps.rhs) {
        fputs(style->col_seps.rhs, output);
    }

    fputc('\n', output);
}

int print_table(struct csv_data *csv, struct cli_args *args)
{
    if (!csv)
        return -1;

    table_format_t *style = create_table_style(
        args->style, args->padding, args->indent, args->header_align, args->body_align);
    if (!style)
        return -1;

    FILE *output = stdout;

    // Print top border
    print_row_separator(output, style, csv->column_widths, csv->max_columns, style->row_seps.top);

    // Print header
    if (csv->header) {
        char **header_fields      = csv->header->fields;
        int    header_field_count = csv->header->field_count;

        // Add sequence header if needed
        if (args->number) {
            print_row(output,
                      style,
                      header_fields,
                      header_field_count,
                      csv->column_widths,
                      csv->max_columns,
                      style->header_align,
                      -1);
        } else {
            print_row(output,
                      style,
                      header_fields,
                      header_field_count,
                      csv->column_widths,
                      csv->max_columns,
                      style->header_align,
                      0);
        }

        // Print header separator
        if (csv->record_count > 0) {
            print_row_separator(
                output, style, csv->column_widths, csv->max_columns, style->row_seps.snd);
        }
    }

    // Print data rows
    for (int i = 0; i < csv->record_count; i++) {
        struct csv_record *record = &csv->records[i];

        if (args->number) {
            print_row(output,
                      style,
                      record->fields,
                      record->field_count,
                      csv->column_widths,
                      csv->max_columns,
                      style->body_align,
                      i + 1);
        } else {
            print_row(output,
                      style,
                      record->fields,
                      record->field_count,
                      csv->column_widths,
                      csv->max_columns,
                      style->body_align,
                      0);
        }

        // Print middle separator if needed
        if (style->row_seps.mid && i < csv->record_count - 1) {
            print_row_separator(
                output, style, csv->column_widths, csv->max_columns, style->row_seps.mid);
        }
    }

    // Print bottom border
    print_row_separator(output, style, csv->column_widths, csv->max_columns, style->row_seps.bot);

    free_table_style(style);
    return 0;
}