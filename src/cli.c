#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cli.h"

static table_style_t parse_style(const char *style_str)
{
    if (strcasecmp(style_str, "none") == 0)
        return STYLE_NONE;
    if (strcasecmp(style_str, "ascii") == 0)
        return STYLE_ASCII;
    if (strcasecmp(style_str, "ascii2") == 0)
        return STYLE_ASCII2;
    if (strcasecmp(style_str, "sharp") == 0)
        return STYLE_SHARP;
    if (strcasecmp(style_str, "rounded") == 0)
        return STYLE_ROUNDED;
    if (strcasecmp(style_str, "reinforced") == 0)
        return STYLE_REINFORCED;
    if (strcasecmp(style_str, "markdown") == 0)
        return STYLE_MARKDOWN;
    if (strcasecmp(style_str, "grid") == 0)
        return STYLE_GRID;
    return STYLE_SHARP;  // default
}

static alignment_t parse_alignment(const char *align_str)
{
    if (strcasecmp(align_str, "left") == 0)
        return ALIGN_LEFT;
    if (strcasecmp(align_str, "center") == 0)
        return ALIGN_CENTER;
    if (strcasecmp(align_str, "right") == 0)
        return ALIGN_RIGHT;
    return ALIGN_LEFT;  // default
}

void print_help(const char *program_name)
{
    printf("Usage: %s [OPTIONS] [FILE]\n\n", program_name);
    printf("A high performance csv viewer with cjk/emoji support.\n\n");
    printf("Arguments:\n");
    printf("  [FILE]  File to view\n\n");
    printf("Options:\n");
    printf("  -H, --no-headers          Specify that the input has no header row\n");
    printf("  -n, --number              Prepend a column of line numbers to the table\n");
    printf("  -t, --tsv                 Use '\\t' as delimiter for tsv\n");
    printf("  -d, --delimiter <CHAR>    Specify the field delimiter [default: ,]\n");
    printf("  -s, --style <STYLE>       Specify the border style [default: sharp]\n");
    printf("                            [possible values: none, ascii, ascii2, sharp, rounded,\n");
    printf("                             reinforced, markdown, grid]\n");
    printf("  -p, --padding <NUM>       Specify padding for table cell [default: 1]\n");
    printf("  -i, --indent <NUM>        Specify global indent for table [default: 0]\n");
    printf("      --sniff <LIMIT>       Limit column widths sniffing to the specified number\n");
    printf("                            of rows. Specify \"0\" to cancel limit [default: 1000]\n");
    printf(
        "      --header-align <ALIGN> Specify the alignment of the table header [default: "
        "center]\n");
    printf("                            [possible values: left, center, right]\n");
    printf("      --body-align <ALIGN>  Specify the alignment of the table body [default: left]\n");
    printf("                            [possible values: left, center, right]\n");
    printf("  -P, --disable-pager       Disable pager\n");
    printf("  -h, --help                Print help information\n");
    printf("  -V, --version             Print version information\n");
}

int parse_cli_args(int argc, char *argv[], struct cli_args *args)
{
    // Initialize defaults
    args->file          = NULL;
    args->no_headers    = false;
    args->number        = false;
    args->tsv           = false;
    args->delimiter     = ',';
    args->style         = STYLE_SHARP;
    args->padding       = 1;
    args->indent        = 0;
    args->sniff         = 1000;
    args->header_align  = ALIGN_CENTER;
    args->body_align    = ALIGN_LEFT;
    args->disable_pager = false;
    args->help          = false;
    args->version       = false;

    static struct option long_options[] = {
        {"no-headers",    no_argument,       0, 'H' },
        {"number",        no_argument,       0, 'n' },
        {"tsv",           no_argument,       0, 't' },
        {"delimiter",     required_argument, 0, 'd' },
        {"style",         required_argument, 0, 's' },
        {"padding",       required_argument, 0, 'p' },
        {"indent",        required_argument, 0, 'i' },
        {"sniff",         required_argument, 0, 1001},
        {"header-align",  required_argument, 0, 1002},
        {"body-align",    required_argument, 0, 1003},
        {"disable-pager", no_argument,       0, 'P' },
        {"help",          no_argument,       0, 'h' },
        {"version",       no_argument,       0, 'V' },
        {0,               0,                 0, 0   }
    };

    int c;
    int option_index = 0;

    while ((c = getopt_long(argc, argv, "Hntd:s:p:i:PhV", long_options, &option_index)) != -1)
    {
        switch (c)
        {
            case 'H':
                args->no_headers = true;
                break;
            case 'n':
                args->number = true;
                break;
            case 't':
                args->tsv = true;
                break;
            case 'd':
                if (strlen(optarg) != 1)
                {
                    fprintf(stderr, "Delimiter must be a single character\n");
                    return -1;
                }
                args->delimiter = optarg[0];
                break;
            case 's':
                args->style = parse_style(optarg);
                break;
            case 'p':
                args->padding = atoi(optarg);
                if (args->padding < 0)
                {
                    fprintf(stderr, "Padding must be non-negative\n");
                    return -1;
                }
                break;
            case 'i':
                args->indent = atoi(optarg);
                if (args->indent < 0)
                {
                    fprintf(stderr, "Indent must be non-negative\n");
                    return -1;
                }
                break;
            case 1001:  // --sniff
                args->sniff = atoi(optarg);
                if (args->sniff < 0)
                {
                    fprintf(stderr, "Sniff limit must be non-negative\n");
                    return -1;
                }
                break;
            case 1002:  // --header-align
                args->header_align = parse_alignment(optarg);
                break;
            case 1003:  // --body-align
                args->body_align = parse_alignment(optarg);
                break;
            case 'P':
                args->disable_pager = true;
                break;
            case 'h':
                args->help = true;
                return 0;
            case 'V':
                args->version = true;
                return 0;
            case '?':
                return -1;
            default:
                fprintf(stderr, "Unknown option: %c\n", c);
                return -1;
        }
    }

    // Handle positional arguments
    if (optind < argc)
    {
        args->file = strdup(argv[optind]);
        if (!args->file)
        {
            fprintf(stderr, "Memory allocation failed\n");
            return -1;
        }
    }

    // Validate conflicting options
    if (args->tsv && args->delimiter != ',')
    {
        fprintf(stderr, "Cannot specify both --tsv and --delimiter\n");
        return -1;
    }

    return 0;
}

void free_cli_args(struct cli_args *args)
{
    if (args && args->file)
    {
        free(args->file);
        args->file = NULL;
    }
}

void setup_pager(bool disable_pager)
{
    // Only setup pager if output is going to a terminal and pager is not disabled
    if (disable_pager || !isatty(STDOUT_FILENO))
    {
        return;
    }

    // Check for custom pager in environment
    const char *pager_cmd = getenv("CSVIEW_PAGER");
    if (!pager_cmd)
    {
        pager_cmd = "less";
    }

    // Create pipe
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return;
    }

    if (pid == 0)
    {
        // Child process: setup pager
        close(pipefd[1]);               // Close write end
        dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin to read end of pipe
        close(pipefd[0]);

        // Set environment for less if using default pager
        if (strcmp(pager_cmd, "less") == 0)
        {
            setenv("LESS", "-SF", 0);  // Don't override existing LESS settings
        }

        // Execute pager
        execlp(pager_cmd, pager_cmd, (char *)NULL);
        perror("execlp");
        exit(1);
    }
    else
    {
        // Parent process: redirect stdout to pipe
        close(pipefd[0]);                // Close read end
        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to write end of pipe
        close(pipefd[1]);
    }
}