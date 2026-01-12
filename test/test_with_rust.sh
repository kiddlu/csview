#!/bin/bash

# Test script that automatically downloads Rust version and runs comparison tests

set -e

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
TEST_DIR="$SCRIPT_DIR"
RUST_DOWNLOAD_DIR="$BUILD_DIR/rust-download"

# Color definitions
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Detect architecture
ARCH=$(uname -m)
case "$ARCH" in
aarch64)
    RUST_URL="https://github.com/wfxr/csview/releases/download/v1.3.4/csview-v1.3.4-aarch64-unknown-linux-musl.tar.gz"
    ;;
x86_64)
    RUST_URL="https://github.com/wfxr/csview/releases/download/v1.3.4/csview-v1.3.4-x86_64-unknown-linux-musl.tar.gz"
    ;;
*)
    echo -e "${RED}Error: Unsupported architecture: $ARCH${NC}"
    exit 1
    ;;
esac

echo -e "${BLUE}Architecture detected: $ARCH${NC}"
echo -e "${BLUE}Rust version URL: $RUST_URL${NC}"

# Check if C version is built
C_VERSION="$BUILD_DIR/csview"
if [ ! -f "$C_VERSION" ]; then
    echo -e "${RED}Error: C version not built. Please run 'make' first${NC}"
    exit 1
fi

# Download and extract Rust version
RUST_TAR="$RUST_DOWNLOAD_DIR/csview-rust.tar.gz"
RUST_EXEC="$RUST_DOWNLOAD_DIR/csview"

mkdir -p "$RUST_DOWNLOAD_DIR"

if [ ! -f "$RUST_EXEC" ] || [ ! -x "$RUST_EXEC" ]; then
    echo -e "${YELLOW}Downloading Rust version...${NC}"
    if command -v curl > /dev/null 2>&1; then
        curl -L -o "$RUST_TAR" "$RUST_URL" || {
            echo -e "${RED}Download failed${NC}"
            exit 1
        }
    elif command -v wget > /dev/null 2>&1; then
        wget -O "$RUST_TAR" "$RUST_URL" || {
            echo -e "${RED}Download failed${NC}"
            exit 1
        }
    else
        echo -e "${RED}Error: Neither curl nor wget found. Please install one of them.${NC}"
        exit 1
    fi

    echo -e "${YELLOW}Extracting Rust version...${NC}"
    tar -xzf "$RUST_TAR" -C "$RUST_DOWNLOAD_DIR" --strip-components=1 || {
        echo -e "${RED}Extraction failed${NC}"
        exit 1
    }

    chmod +x "$RUST_EXEC" || true

    # Clean up tar file
    rm -f "$RUST_TAR"
else
    echo -e "${GREEN}Rust version already exists, skipping download${NC}"
fi

# Run comprehensive test
echo -e "${BLUE}Running comparison tests...${NC}"
cd "$TEST_DIR"
bash comprehensive_test.sh -c "$C_VERSION" -r "$RUST_EXEC" "$@"
