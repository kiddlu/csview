#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cli.h"
#include "csv_parser.h"
#include "table_printer.h"

int main(int argc, char *argv[])
{
    // Set locale for proper Unicode handling
    setlocale(LC_ALL, "");

    // Parse command line arguments
    struct cli_args args;
    if (parse_cli_args(argc, argv, &args) != 0)
    {
        return 1;
    }

    // Handle help and version
    if (args.help)
    {
        print_help(argv[0]);
        return 0;
    }

    if (args.version)
    {
        printf("csview 1.3.4\n");
        return 0;
    }

    // Check for input
    if (!args.file && isatty(STDIN_FILENO))
    {
        fprintf(stderr, "no input file specified (use -h for help)\n");
        return 1;
    }

    // Setup pager if needed
    setup_pager(args.disable_pager);

    // Open input file or use stdin
    FILE *input = args.file ? fopen(args.file, "r") : stdin;
    if (!input && args.file)
    {
        perror(args.file);
        return 1;
    }

    // Parse CSV
    struct csv_data csv;
    int             ret = parse_csv(input, &csv, args);

    if (args.file)
    {
        fclose(input);
    }

    if (ret != 0)
    {
        return ret;
    }

    // Print table
    ret = print_table(&csv, &args);

    // Cleanup
    free_csv_data(&csv);
    free_cli_args(&args);

    return ret;
}