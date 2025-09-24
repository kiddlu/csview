# csview - C Implementation

A high performance CSV viewer with CJK/emoji support, implemented in C.

## Dependencies

- libcsv-dev (CSV parsing)
- libunistring-dev (Unicode width calculation)
- GCC with C99 support

## Build

```bash
make
```

## Usage

```bash
./csview [OPTIONS] [FILE]
```

### Key Options

- `-H, --no-headers`: No header row
- `-n, --number`: Add line numbers
- `-t, --tsv`: TSV format (tab-delimited)
- `-d, --delimiter <CHAR>`: Custom delimiter [default: ,]
- `-s, --style <STYLE>`: Border style [default: sharp]
  - Values: none, ascii, ascii2, sharp, rounded, reinforced, markdown, grid
- `-h, --help`: Show help

### Examples

```bash
./csview data.csv                    # Basic usage
./csview -n -s rounded data.csv      # Line numbers + rounded style
./csview -t -H data.tsv              # TSV without headers
make test                            # Run test
```

## Features

- High performance CSV parsing with libcsv
- Professional Unicode width calculation using GNU libunistring
- Comprehensive CJK/emoji support including complex characters
- Multiple table styles (Unicode box drawing, ASCII, markdown)
- Custom delimiters and TSV support
- Line numbering