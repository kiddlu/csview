#!/bin/bash

# Test script to verify pager behavior between C and Rust implementations

echo "=== Pager Behavior Test ==="
echo

# Test 1: With -P flag (pager disabled)
echo "Test 1: Both versions with -P flag (pager disabled)"
echo "C version:"
timeout 2 ../csview -P data/chinese_sectors.csv | wc -l
echo "Rust version:"
timeout 2 ../csview.rust -P data/chinese_sectors.csv | wc -l
echo

# Test 2: Piped output (pager should auto-disable)
echo "Test 2: Piped output (pager should auto-disable)"
echo "C version:"
timeout 2 ../csview data/chinese_sectors.csv | wc -l
echo "Rust version:"
timeout 2 ../csview.rust data/chinese_sectors.csv | wc -l
echo

# Test 3: Compare outputs
echo "Test 3: Output comparison (with -P)"
../csview -P data/chinese_sectors.csv > /tmp/c_pager_test.txt
../csview.rust -P data/chinese_sectors.csv > /tmp/rust_pager_test.txt
if diff -q /tmp/c_pager_test.txt /tmp/rust_pager_test.txt > /dev/null; then
    echo "✓ Outputs are identical"
else
    echo "✗ Outputs differ"
    exit 1
fi
rm -f /tmp/c_pager_test.txt /tmp/rust_pager_test.txt

echo
echo "All pager tests passed!"

