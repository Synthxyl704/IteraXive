#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
TEST_DIR="${PROJECT_ROOT}/test_programs"
ITERAXIVE="${BUILD_DIR}/iteraxive"

PASS=0
FAIL=0

print_result() {
    local name="$1"
    local status="$2"
    if [ "$status" -eq 0 ]; then
        echo "  [PASS] $name"
        PASS=$((PASS + 1))
    else
        echo "  [FAIL] $name"
        FAIL=$((FAIL + 1))
    fi
}

build_iteraxive() {
    echo "=== Building IteraXive ==="
    cd "$PROJECT_ROOT"
    if command -v cmake &>/dev/null; then
        mkdir -p "$BUILD_DIR"
        cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release &>/dev/null
        cmake --build "$BUILD_DIR" --parallel 2>/dev/null
    else
        g++ -std=c++17 -O2 src/main.cpp src/benchmarkAnalyzer.cpp -o "$ITERAXIVE" -lpthread -ldl
    fi

    if [ ! -x "$ITERAXIVE" ]; then
        echo "ERROR: Failed to build IteraXive" >&2
        exit 1
    fi
    echo "Build OK: $ITERAXIVE"
    echo
}

test_benchmark_cpp() {
    echo "=== Test: benchmark C++ program ==="
    local output
    output=$(timeout 15 "$ITERAXIVE" "${TEST_DIR}/bm1.cpp" 2>&1 || true)
    echo "$output" | grep -q "Welcome 2 IteraXive" && true
    print_result "C++ benchmark starts" $?
}

test_benchmark_c() {
    echo "=== Test: benchmark C program ==="
    local output
    output=$(timeout 15 "$ITERAXIVE" "${TEST_DIR}/test_c.c" 2>&1 || true)
    echo "$output" | grep -q "COMPILATION PHASE" && true
    print_result "C program compiles" $?
}

test_compiler_detection() {
    echo "=== Test: compiler detection ==="
    local output
    output=$(timeout 15 "$ITERAXIVE" "${TEST_DIR}/test_c.c" 2>&1 || true)
    if echo "$output" | grep -qE "Compiler[[:space:]]+(g\+\+|gcc|clang)"; then
        print_result "Compiler detected" 0
    else
        print_result "Compiler detected" 1
    fi
}

test_source_not_found() {
    echo "=== Test: nonexistent source file ==="
    local output
    output=$(timeout 5 "$ITERAXIVE" "/nonexistent/path.cpp" 2>&1 || true)
    if echo "$output" | grep -qi "not found"; then
        print_result "Nonexistent source handled" 0
    else
        print_result "Nonexistent source handled" 1
    fi
}

test_exit_code_on_success() {
    echo "=== Test: exit code on success ==="
    timeout 15 "$ITERAXIVE" "${TEST_DIR}/test_c.c" &>/dev/null && true
    print_result "Exit code 0" $?
}

test_cmake_build() {
    echo "=== Test: CMake build system ==="
    if [ -f "${PROJECT_ROOT}/CMakeLists.txt" ]; then
        print_result "CMakeLists.txt exists" 0
    else
        print_result "CMakeLists.txt exists" 1
    fi
}

test_python_build() {
    echo "=== Test: Python build script ==="
    if python3 "${PROJECT_ROOT}/scripts/build.py" &>/dev/null; then
        print_result "Python build works" 0
    else
        print_result "Python build works" 1
    fi
}

main() {
    build_iteraxive
    test_benchmark_cpp
    test_benchmark_c
    test_compiler_detection
    test_source_not_found
    test_exit_code_on_success
    test_cmake_build
    test_python_build

    echo
    echo "=== Results: $PASS passed, $FAIL failed ==="
    if [ "$FAIL" -gt 0 ]; then
        exit 1
    fi
}

main
