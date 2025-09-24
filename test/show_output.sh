#!/bin/bash

# Show output examples for specific test cases

usage() {
    echo "Usage: $0 <data_file> [arguments]"
    echo "Examples:"
    echo "  $0 basic.csv"
    echo "  $0 basic.csv -H"
    echo "  $0 wide_columns.csv --header-align right"
    echo "  $0 emoji_data.csv -n -s rounded"
    echo ""
    echo "Available test data files:"
    ls -1 data/*.csv data/*.tsv 2>/dev/null | sed 's/data\//  /' | sort
}

if [[ $# -eq 0 ]]; then
    usage
    exit 1
fi

DATA_FILE="data/$1"
shift
ARGS="$*"

if [[ ! -f "$DATA_FILE" ]]; then
    echo "Error: Test data file not found: $DATA_FILE"
    echo ""
    usage
    exit 1
fi

echo "=== Test Data File Content ==="
echo "File: $DATA_FILE"
echo "Arguments: $ARGS"
echo ""
cat "$DATA_FILE"
echo ""

echo "=== C Version Output ==="
if [[ -n "$ARGS" ]]; then
    eval "../csview $ARGS \"$DATA_FILE\""
else
    ../csview "$DATA_FILE"
fi

echo ""
echo "=== Rust Version Output ==="
if [[ -n "$ARGS" ]]; then
    eval "../csview.rust $ARGS \"$DATA_FILE\""
else
    ../csview.rust "$DATA_FILE"
fi

echo ""
echo "=== Output Comparison ==="
# Quick diff check
c_output=$(mktemp)
rust_output=$(mktemp)

if [[ -n "$ARGS" ]]; then
    eval "../csview $ARGS \"$DATA_FILE\"" > "$c_output" 2>/dev/null
    eval "../csview.rust $ARGS \"$DATA_FILE\"" > "$rust_output" 2>/dev/null
else
    ../csview "$DATA_FILE" > "$c_output" 2>/dev/null
    ../csview.rust "$DATA_FILE" > "$rust_output" 2>/dev/null
fi

if diff -q "$c_output" "$rust_output" > /dev/null; then
    echo "✓ Outputs are identical"
else
    echo "✗ Outputs differ:"
    diff -u "$c_output" "$rust_output"
fi

rm -f "$c_output" "$rust_output"
