#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <stdio.h>
#include "cli.h"

struct csv_record {
    char **fields;
    int field_count;
};

struct csv_data {
    struct csv_record *header;
    struct csv_record *records;
    int record_count;
    int max_columns;
    int *column_widths;
};

int parse_csv(FILE *input, struct csv_data *csv, struct cli_args args);
void free_csv_data(struct csv_data *csv);
void free_csv_record(struct csv_record *record);

#endif // CSV_PARSER_H