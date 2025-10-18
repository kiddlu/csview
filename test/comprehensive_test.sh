#!/bin/bash

# Comprehensive test suite for comparing C and Rust versions of csview
# Tests all parameters and combinations with various data types

# Color definitions
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m' # No Color

# Global counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
SKIPPED_TESTS=0

# Configuration
C_VERSION="../csview"
RUST_VERSION="../csview.rust"
DATA_DIR="./data"
OUTPUT_DIR="./output"
VERBOSE=false
STOP_ON_FAIL=false

usage() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -c <path>    Path to C version executable (default: ../csview)"
    echo "  -r <path>    Path to Rust version executable (default: ../csview.rust)"
    echo "  -d <path>    Test data directory (default: ./data)"
    echo "  -o <path>    Output directory (default: ./output)"
    echo "  -v           Verbose mode"
    echo "  -s           Stop on first failure"
    echo "  -h           Show help"
}

# Parse command line arguments
while getopts "c:r:d:o:vsh" opt; do
    case $opt in
        c) C_VERSION="$OPTARG" ;;
        r) RUST_VERSION="$OPTARG" ;;
        d) DATA_DIR="$OPTARG" ;;
        o) OUTPUT_DIR="$OPTARG" ;;
        v) VERBOSE=true ;;
        s) STOP_ON_FAIL=true ;;
        h) usage; exit 0 ;;
        *) usage; exit 1 ;;
    esac
done

# Check executables
check_executables() {
    if [[ ! -x "$C_VERSION" ]]; then
        echo -e "${RED}Error: C version executable not found or not executable: $C_VERSION${NC}"
        exit 1
    fi
    
    if [[ ! -x "$RUST_VERSION" ]]; then
        echo -e "${RED}Error: Rust version executable not found or not executable: $RUST_VERSION${NC}"
        exit 1
    fi
    
    if [[ ! -d "$DATA_DIR" ]]; then
        echo -e "${RED}Error: Test data directory not found: $DATA_DIR${NC}"
        exit 1
    fi
}

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Run a single test
run_test() {
    local test_name="$1"
    local data_file="$2"
    local args="$3"
    local description="$4"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    echo -e "${BLUE}Test $TOTAL_TESTS: $test_name${NC}"
    if [[ "$VERBOSE" == true ]]; then
        echo "  Description: $description"
        echo "  Data file: $data_file"
        echo "  Arguments: $args"
    fi
    
    # Check if data file exists
    if [[ ! -f "$data_file" ]]; then
        echo -e "  ${YELLOW}⚠ SKIPPED: Data file not found${NC}"
        SKIPPED_TESTS=$((SKIPPED_TESTS + 1))
        echo
        return
    fi
    
    # Run C version
    local c_output="$OUTPUT_DIR/${test_name}_c.txt"
    local c_error="$OUTPUT_DIR/${test_name}_c.err"
    if [[ -n "$args" ]]; then
        eval "$C_VERSION $args \"$data_file\"" > "$c_output" 2> "$c_error"
    else
        "$C_VERSION" "$data_file" > "$c_output" 2> "$c_error"
    fi
    local c_exit_code=$?
    
    # Run Rust version
    local rust_output="$OUTPUT_DIR/${test_name}_rust.txt"
    local rust_error="$OUTPUT_DIR/${test_name}_rust.err"
    if [[ -n "$args" ]]; then
        eval "$RUST_VERSION $args \"$data_file\"" > "$rust_output" 2> "$rust_error"
    else
        "$RUST_VERSION" "$data_file" > "$rust_output" 2> "$rust_error"
    fi
    local rust_exit_code=$?
    
    # Compare outputs
    local test_passed=true
    
    if ! diff -q "$c_output" "$rust_output" > /dev/null; then
        echo -e "  ${RED}✗ Output differs${NC}"
        test_passed=false
        
        if [[ "$VERBOSE" == true ]]; then
            echo "  Output differences:"
            diff -u "$c_output" "$rust_output" | head -20
        fi
        
        # Save diff to file
        diff -u "$c_output" "$rust_output" > "$OUTPUT_DIR/${test_name}_diff.txt"
    fi
    
    # Check exit codes
    if [[ $c_exit_code -ne $rust_exit_code ]]; then
        echo -e "  ${YELLOW}⚠ Exit codes differ: C=$c_exit_code, Rust=$rust_exit_code${NC}"
        if [[ "$test_passed" == true ]]; then
            test_passed=false
        fi
    fi
    
    if [[ "$test_passed" == true ]]; then
        echo -e "  ${GREEN}✓ PASSED${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "  ${RED}✗ FAILED${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        
        if [[ "$STOP_ON_FAIL" == true ]]; then
            echo -e "${RED}Stopping on first failure as requested${NC}"
            exit 1
        fi
    fi
    
    echo
}

# Test basic functionality
test_basic_functionality() {
    echo -e "${CYAN}=== Basic Functionality Tests ===${NC}"
    
    run_test "basic_default" "$DATA_DIR/basic.csv" "" "Default output"
    run_test "basic_no_headers" "$DATA_DIR/basic.csv" "-H" "No headers mode"
    run_test "basic_with_numbers" "$DATA_DIR/basic.csv" "-n" "With line numbers"
    run_test "basic_no_headers_numbers" "$DATA_DIR/basic.csv" "-H -n" "No headers with line numbers"
    run_test "basic_tsv_mode" "$DATA_DIR/basic.csv" "-t" "TSV mode"
}

# Test table styles
test_table_styles() {
    echo -e "${CYAN}=== Table Style Tests ===${NC}"
    
    local styles=("none" "ascii" "ascii2" "sharp" "rounded" "reinforced" "markdown" "grid")
    
    for style in "${styles[@]}"; do
        run_test "style_${style}" "$DATA_DIR/basic.csv" "-s $style" "Table style: $style"
        run_test "style_${style}_no_headers" "$DATA_DIR/basic.csv" "-s $style -H" "Table style: $style without headers"
    done
}

# Test alignment options
test_alignment() {
    echo -e "${CYAN}=== Alignment Tests ===${NC}"
    
    local alignments=("left" "center" "right")
    
    for align in "${alignments[@]}"; do
        run_test "header_align_${align}" "$DATA_DIR/wide_columns.csv" "--header-align $align" "Header alignment: $align"
        run_test "body_align_${align}" "$DATA_DIR/wide_columns.csv" "--body-align $align" "Body alignment: $align"
    done
}

# Test Unicode and special characters
test_unicode_special() {
    echo -e "${CYAN}=== Unicode and Special Character Tests ===${NC}"
    
    run_test "unicode_basic" "$DATA_DIR/unicode.csv" "" "Basic Unicode"
    run_test "unicode_no_headers" "$DATA_DIR/unicode.csv" "-H" "Unicode without headers"
    
    run_test "cjk_mixed" "$DATA_DIR/cjk_mixed.csv" "" "Mixed CJK characters"
    run_test "cjk_mixed_no_headers" "$DATA_DIR/cjk_mixed.csv" "-H" "Mixed CJK without headers"
    
    run_test "chinese_sectors" "$DATA_DIR/chinese_sectors.csv" "" "Chinese sector data"
    run_test "chinese_sectors_no_headers" "$DATA_DIR/chinese_sectors.csv" "-H" "Chinese sectors without headers"
    run_test "chinese_sectors_with_numbers" "$DATA_DIR/chinese_sectors.csv" "-n" "Chinese sectors with line numbers"
    
    run_test "emoji_data" "$DATA_DIR/emoji_data.csv" "" "Emoji data"
    run_test "emoji_no_headers" "$DATA_DIR/emoji_data.csv" "-H" "Emoji without headers"
    run_test "emoji_with_numbers" "$DATA_DIR/emoji_data.csv" "-n" "Emoji with numbers"
    
    run_test "wide_unicode" "$DATA_DIR/wide_unicode.csv" "" "Wide Unicode columns"
    run_test "special_chars" "$DATA_DIR/special_chars.csv" "" "Special characters and escaping"
}

# Test edge cases
test_edge_cases() {
    echo -e "${CYAN}=== Edge Case Tests ===${NC}"
    
    run_test "empty_fields" "$DATA_DIR/empty_fields.csv" "" "Empty fields"
    run_test "single_row" "$DATA_DIR/single_row.csv" "" "Single data row"
    run_test "header_only" "$DATA_DIR/header_only.csv" "" "Header only"
    run_test "numbers_mixed" "$DATA_DIR/numbers_mixed.csv" "" "Mixed number formats"
}

# Test large datasets and sniff behavior
test_large_datasets() {
    echo -e "${CYAN}=== Large Dataset Tests ===${NC}"
    
    run_test "query_default" "$DATA_DIR/query.csv" "" "Large dataset (6000+ rows) with default sniff"
    run_test "query_sniff_100" "$DATA_DIR/query.csv" "--sniff 100" "Large dataset with sniff limit 100"
    run_test "query_sniff_0" "$DATA_DIR/query.csv" "--sniff 0" "Large dataset with sniff disabled"
    run_test "query_no_headers" "$DATA_DIR/query.csv" "-H" "Large dataset without headers"
}

# Test complex combinations
test_complex_combinations() {
    echo -e "${CYAN}=== Complex Parameter Combination Tests ===${NC}"
    
    run_test "markdown_center" "$DATA_DIR/basic.csv" "-s markdown --body-align center" "Markdown style with center alignment"
    run_test "unicode_rounded" "$DATA_DIR/unicode.csv" "-s rounded" "Unicode with rounded style"
    run_test "emoji_grid" "$DATA_DIR/emoji_data.csv" "-s grid" "Emoji with grid style"
    run_test "complex_combo1" "$DATA_DIR/wide_unicode.csv" "-H -n -s rounded -p 2 -i 4 --body-align center" "Complex combination 1"
    run_test "complex_combo2" "$DATA_DIR/emoji_data.csv" "-s grid --header-align right --body-align left -p 0" "Complex combination 2"
}

# Main test execution
main() {
    echo -e "${MAGENTA}=== Comprehensive CSV Viewer Comparison Test Suite ===${NC}"
    echo "C version: $C_VERSION"
    echo "Rust version: $RUST_VERSION"
    echo "Data directory: $DATA_DIR"
    echo "Output directory: $OUTPUT_DIR"
    echo
    
    check_executables
    
    # Run all test suites
    test_basic_functionality
    test_table_styles
    test_alignment
    test_unicode_special
    test_edge_cases
    test_large_datasets
    test_complex_combinations
    
    # Final summary
    echo -e "${MAGENTA}=== Test Summary ===${NC}"
    echo "Total tests: $TOTAL_TESTS"
    echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
    echo -e "Failed: ${RED}$FAILED_TESTS${NC}"
    echo -e "Skipped: ${YELLOW}$SKIPPED_TESTS${NC}"
    
    if [[ $FAILED_TESTS -eq 0 ]]; then
        echo -e "${GREEN}All tests passed successfully!${NC}"
        exit 0
    else
        echo -e "${RED}$FAILED_TESTS test(s) failed${NC}"
        echo "Check the diff files in $OUTPUT_DIR for detailed differences"
        exit 1
    fi
}

main "$@"
