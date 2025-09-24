# CSV Viewer Version Comparison Test Suite

This comprehensive test framework compares the output of C and Rust versions of csview across all parameters and data types.

## Directory Structure

```
test/
├── README.md                   # This documentation
├── quick_test.sh              # Quick test runner
├── comprehensive_test.sh      # Comprehensive test suite
├── show_output.sh             # Show specific output examples
├── data/                      # Test data files
│   ├── basic.csv              # Basic test data
│   ├── wide_columns.csv       # Wide column test data
│   ├── empty_fields.csv       # Empty field test data
│   ├── single_row.csv         # Single row test data
│   ├── header_only.csv        # Header only test data
│   ├── unicode.csv            # Unicode test data
│   ├── cjk_mixed.csv          # Mixed CJK characters
│   ├── emoji_data.csv         # Emoji test data
│   ├── wide_unicode.csv       # Wide Unicode columns
│   ├── special_chars.csv      # Special characters and escaping
│   ├── numbers_mixed.csv      # Mixed number formats
│   ├── tsv_data.tsv           # TSV format data
│   └── custom_delimiter.csv   # Custom delimiter data
└── output/                    # Test output files (auto-generated)
```

## Usage

### Quick Test (Recommended)
```bash
cd test
./quick_test.sh
```

### Comprehensive Test
```bash
cd test
./comprehensive_test.sh -v
```

### Show Specific Examples
```bash
cd test
./show_output.sh basic.csv
./show_output.sh emoji_data.csv -H -n
./show_output.sh cjk_mixed.csv -s rounded --body-align center
```

### Custom Test Configuration
```bash
cd test
./comprehensive_test.sh -c /path/to/c_version -r /path/to/rust_version -v -s
```

## Test Coverage

### Basic Functionality (8 tests)
- Default output
- No headers mode (`-H`)
- Line numbers (`-n`)
- TSV mode (`-t`)
- Various combinations

### Table Styles (16 tests)
Tests all 8 styles with and without headers:
- `none`, `ascii`, `ascii2`, `sharp`, `rounded`, `reinforced`, `markdown`, `grid`

### Alignment Options (12 tests)
- Header alignment: `left`, `center`, `right`
- Body alignment: `left`, `center`, `right`
- All combinations of header + body alignment (9 tests)

### Padding and Indentation (11 tests)
- Padding values: 0, 1, 2, 3, 5
- Indentation values: 0, 2, 4, 8
- Combination tests

### Delimiter Options (3 tests)
- Built-in TSV mode
- Custom pipe delimiter
- Error handling for invalid delimiters

### Sniff Limits (5 tests)
- Various sniff limits: 0, 1, 2, 10, 1000

### Unicode and Special Characters (15 tests)
- Basic Unicode characters
- Mixed CJK (Chinese, Japanese, Korean)
- Emoji data with various symbols
- Wide Unicode columns
- Special CSV characters and escaping

### Edge Cases (8 tests)
- Empty fields
- Single row data
- Header-only files
- Mixed number formats
- Various alignment with edge cases

### Complex Combinations (40+ tests)
- Style + alignment combinations
- Unicode + style combinations
- Multi-parameter combinations
- All style × alignment matrix tests

### Error Conditions (6 tests)
- Invalid parameters
- Boundary conditions
- File handling errors

## Parameters Tested

### Command Line Options
- `-H, --no-headers`: Treat first row as data
- `-n, --number`: Add line numbers
- `-t, --tsv`: Use tab delimiter
- `-d, --delimiter <char>`: Custom delimiter
- `-s, --style <style>`: Table border style
- `-p, --padding <num>`: Cell padding
- `-i, --indent <num>`: Global indentation
- `--sniff <num>`: Column width sniffing limit
- `--header-align <align>`: Header alignment
- `--body-align <align>`: Body alignment

### Style Options
- `none`: No borders
- `ascii`: ASCII characters
- `ascii2`: Simple ASCII
- `sharp`: Unicode box drawing
- `rounded`: Rounded corners
- `reinforced`: Heavy borders
- `markdown`: Markdown table format
- `grid`: Full grid lines

### Alignment Options
- `left`: Left alignment
- `center`: Center alignment
- `right`: Right alignment

## Test Data Types

### Character Sets
- **ASCII**: Basic English text
- **Unicode**: Various Unicode symbols
- **CJK**: Chinese, Japanese, Korean characters
- **Emoji**: Various emoji symbols and flags
- **Mixed**: Combinations of all above

### Data Formats
- **CSV**: Comma-separated values
- **TSV**: Tab-separated values
- **Custom**: Pipe-separated values
- **Special**: Quoted fields, embedded commas, newlines

### Content Types
- **Text**: Various string lengths
- **Numbers**: Integers, floats, scientific notation
- **Empty**: Empty fields and missing data
- **Special**: Currency, percentages, symbols

## Output Files

After running tests, the `output/` directory contains:

- `*_c.txt`: C version output
- `*_rust.txt`: Rust version output
- `*_diff.txt`: Difference files (only when outputs differ)
- `*_c.err`: C version error output
- `*_rust.err`: Rust version error output

**Note**: The `output/` directory is automatically ignored by git via `.gitignore` to prevent test artifacts from being committed to the repository.

## Result Interpretation

- ✓ **Green**: Test passed (outputs identical)
- ✗ **Red**: Test failed (outputs differ)
- ⚠ **Yellow**: Warning (exit codes differ or other issues)
- **Skipped**: Test data file not found

## Command Line Options

### comprehensive_test.sh Options
- `-c <path>`: Path to C version executable
- `-r <path>`: Path to Rust version executable
- `-d <path>`: Test data directory
- `-o <path>`: Output directory
- `-v`: Verbose mode (show differences)
- `-s`: Stop on first failure
- `-h`: Show help

## Expected Results

All tests should pass, indicating that both versions produce identical output for all parameter combinations and data types. Any failures indicate inconsistencies that need to be addressed.

## Adding New Tests

To add new test cases:

1. Add test data to `data/` directory
2. Add test calls in appropriate sections of `comprehensive_test.sh`
3. Use descriptive test names and descriptions
4. Test both with and without headers where applicable

## Performance Notes

The comprehensive test suite runs 200+ individual tests and may take several minutes to complete. Use the quick test for faster feedback during development.