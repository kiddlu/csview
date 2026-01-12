#include <stdlib.h>
#include <string.h>

#include <csv.h>

#include "csv_parser.h"
#include "utils.h"

#define BUFFER_SIZE             8192
#define INITIAL_RECORD_CAPACITY 1000

struct parse_state {
    struct csv_data   *csv;
    struct csv_record *current_record;
    bool               is_header;
    bool               no_headers;
    bool               number;
    int                sniff_limit;
    int                record_count;
};

static void update_column_widths(struct csv_data *csv, struct csv_record *record)
{
    for (int i = 0; i < record->field_count; i++) {
        if (i >= csv->max_columns) {
            csv->max_columns = i + 1;
            csv->column_widths =
                realloc(csv->column_widths, (size_t)csv->max_columns * sizeof(int));
            for (int j = i; j < csv->max_columns; j++) {
                csv->column_widths[j] = 0;
            }
        }

        int width = unicode_display_width(record->fields[i]);
        if (width > csv->column_widths[i]) {
            csv->column_widths[i] = width;
        }
    }
}

static void field_callback(void *s, size_t len, void *data)
{
    struct parse_state *state = (struct parse_state *)data;

    if (!state->current_record) {
        state->current_record              = malloc(sizeof(struct csv_record));
        state->current_record->fields      = NULL;
        state->current_record->field_count = 0;
    }

    // Expand fields array
    state->current_record->fields =
        realloc(state->current_record->fields,
                (size_t)(state->current_record->field_count + 1) * sizeof(char *));

    // Copy field data
    char *field = malloc(len + 1);
    memcpy(field, s, len);
    field[len] = '\0';

    state->current_record->fields[state->current_record->field_count] = field;
    state->current_record->field_count++;
}

static void record_callback(int c __attribute__((unused)), void *data)
{
    struct parse_state *state = (struct parse_state *)data;

    if (!state->current_record) {
        return;
    }

    // Handle header
    if (state->is_header && !state->no_headers) {
        state->csv->header = state->current_record;
        update_column_widths(state->csv, state->csv->header);
        state->is_header      = false;
        state->current_record = NULL;
        return;
    }

    state->is_header = false;

    // Expand records array if needed
    if (state->csv->record_count >= INITIAL_RECORD_CAPACITY) {
        state->csv->records =
            realloc(state->csv->records,
                    (size_t)(state->csv->record_count + 1) * sizeof(struct csv_record));
    }

    state->csv->records[state->csv->record_count] = *state->current_record;

    // Only update column widths if we haven't reached the sniff limit
    if (state->sniff_limit == 0 || state->record_count < state->sniff_limit) {
        update_column_widths(state->csv, &state->csv->records[state->csv->record_count]);
    }

    state->csv->record_count++;
    state->record_count++;

    free(state->current_record);
    state->current_record = NULL;
}

int parse_csv(FILE *input, struct csv_data *csv, struct cli_args args)
{
    csv->header        = NULL;
    csv->records       = malloc(INITIAL_RECORD_CAPACITY * sizeof(struct csv_record));
    csv->record_count  = 0;
    csv->max_columns   = 0;
    csv->column_widths = NULL;

    if (!csv->records) {
        return -1;
    }

    struct csv_parser parser;
    if (csv_init(&parser, 0) != 0) {
        free(csv->records);
        return -1;
    }

    // Set delimiter
    char delimiter = args.tsv ? '\t' : args.delimiter;
    csv_set_delim(&parser, (unsigned char)delimiter);

    // Set up parsing state
    struct parse_state state = {.csv            = csv,
                                .current_record = NULL,
                                .is_header      = !args.no_headers,
                                .no_headers     = args.no_headers,
                                .number         = args.number,
                                .sniff_limit    = args.sniff,
                                .record_count   = 0};

    // Read and parse input
    char   buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, input)) > 0) {
        if (csv_parse(&parser, buffer, bytes_read, field_callback, record_callback, &state) !=
            bytes_read) {
            fprintf(stderr, "Error parsing CSV: %s\n", csv_strerror(csv_error(&parser)));
            csv_free(&parser);
            free_csv_data(csv);
            return -1;
        }

        // Continue parsing all records even after sniff limit
        // The sniff limit only affects column width calculation
    }

    // Finalize parsing
    if (csv_fini(&parser, field_callback, record_callback, &state) != 0) {
        fprintf(stderr, "Error finalizing CSV parse: %s\n", csv_strerror(csv_error(&parser)));
        csv_free(&parser);
        free_csv_data(csv);
        return -1;
    }

    csv_free(&parser);

    // Add sequence number column width if needed
    if (args.number) {
        int seq_width = snprintf(NULL, 0, "%d", csv->record_count);
        if (seq_width < 1)
            seq_width = 1;

        // Shift column widths to make room for sequence number
        csv->max_columns++;
        csv->column_widths = realloc(csv->column_widths, (size_t)csv->max_columns * sizeof(int));
        memmove(&csv->column_widths[1],
                &csv->column_widths[0],
                (size_t)(csv->max_columns - 1) * sizeof(int));
        csv->column_widths[0] = seq_width;
    }

    return 0;
}

void free_csv_record(struct csv_record *record)
{
    if (record && record->fields) {
        for (int i = 0; i < record->field_count; i++) {
            free(record->fields[i]);
        }
        free(record->fields);
        record->fields      = NULL;
        record->field_count = 0;
    }
}

void free_csv_data(struct csv_data *csv)
{
    if (csv) {
        if (csv->header) {
            free_csv_record(csv->header);
            free(csv->header);
            csv->header = NULL;
        }

        if (csv->records) {
            for (int i = 0; i < csv->record_count; i++) {
                free_csv_record(&csv->records[i]);
            }
            free(csv->records);
            csv->records = NULL;
        }

        free(csv->column_widths);
        csv->column_widths = NULL;
        csv->record_count  = 0;
        csv->max_columns   = 0;
    }
}