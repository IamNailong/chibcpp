#!/bin/bash

# Test script for chibcc compiler
# Usage: ./test_compiler.sh

# Don't exit on error, we want to capture and report them

COMPILER="./build/bin/chibcc"
TEST_DIR="test_cases"
RESULTS_DIR="test_results"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Create directories
mkdir -p "$TEST_DIR" "$RESULTS_DIR"

# Function to run a test case
run_test() {
    local test_name="$1"
    local input="$2"
    local expected_exit_code="${3:-0}"
    
    echo -e "${YELLOW}Testing: $test_name${NC}"
    echo "Input: $input"
    
    # Generate assembly
    if $COMPILER "$input" > "$RESULTS_DIR/${test_name}.s" 2> "$RESULTS_DIR/${test_name}.err"; then
        echo -e "${GREEN}✓ Compilation successful${NC}"
        
        # Show generated assembly
        echo "Generated assembly:"
        cat "$RESULTS_DIR/${test_name}.s"
        
        # Try to assemble and link
        if gcc -o "$RESULTS_DIR/${test_name}" "$RESULTS_DIR/${test_name}.s" 2>/dev/null; then
            # Run the executable
            if ./"$RESULTS_DIR/${test_name}"; then
                exit_code=$?
                echo -e "${GREEN}✓ Execution successful (exit code: $exit_code)${NC}"
                if [ $exit_code -eq $expected_exit_code ]; then
                    echo -e "${GREEN}✓ Expected exit code matched${NC}"
                else
                    echo -e "${RED}✗ Expected exit code $expected_exit_code, got $exit_code${NC}"
                fi
            else
                exit_code=$?
                echo -e "${YELLOW}Program exited with code: $exit_code${NC}"
            fi
        else
            echo -e "${RED}✗ Assembly/linking failed${NC}"
        fi
    else
        echo -e "${RED}✗ Compilation failed${NC}"
        if [ -s "$RESULTS_DIR/${test_name}.err" ]; then
            echo "Error output:"
            cat "$RESULTS_DIR/${test_name}.err"
        fi
    fi
    echo "----------------------------------------"
}

# Test cases
echo -e "${YELLOW}Starting compiler tests...${NC}"
echo "========================================"

# Basic arithmetic tests
run_test "simple_addition" "1+1" 2
run_test "simple_subtraction" "5-3" 2
run_test "simple_multiplication" "3*4" 12
run_test "simple_division" "8/2" 4

# More complex expressions
run_test "complex_expr1" "1+2*3" 7
run_test "complex_expr2" "(1+2)*3" 9
run_test "complex_expr3" "10-2*3" 4

# Edge cases
run_test "single_number" "42" 42
run_test "zero" "0" 0
run_test "negative" "-5" 251  # -5 as unsigned byte = 251

# Parentheses tests
run_test "nested_parens" "((1+2)*3)+4" 13
run_test "multiple_parens" "(1+2)*(3+4)" 21

echo -e "${GREEN}All tests completed!${NC}"
echo "Check $RESULTS_DIR/ for detailed results."