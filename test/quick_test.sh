#!/bin/bash

# Quick test runner for CSV viewer comparison
cd "$(dirname "$0")"

echo "=== Quick CSV Viewer Comparison Test ==="
echo

# Check executables
if [[ ! -x "../csview" ]]; then
    echo "Error: C version executable not found: ../csview"
    exit 1
fi

if [[ ! -x "../csview.rust" ]]; then
    echo "Error: Rust version executable not found: ../csview.rust"
    exit 1
fi

# Run comprehensive test
chmod +x comprehensive_test.sh
./comprehensive_test.sh -v

echo
echo "Quick test completed!"
