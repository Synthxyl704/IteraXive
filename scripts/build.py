#!/usr/bin/env python3
import subprocess
import sys
import os
import shutil

PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC_DIR = os.path.join(PROJECT_ROOT, "src")
BUILD_DIR = os.path.join(PROJECT_ROOT, "build")


def find_cmake():
    return shutil.which("cmake")


def find_compiler():
    candidates = ["g++", "clang++", "gcc", "clang"]
    for c in candidates:
        if shutil.which(c):
            return c
    return None


def build_with_cmake():
    if not find_cmake():
        return False
    os.makedirs(BUILD_DIR, exist_ok=True)
    result = subprocess.run(
        ["cmake", "-S", PROJECT_ROOT, "-B", BUILD_DIR, "-DCMAKE_BUILD_TYPE=Release"],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        print("CMake configuration failed:", result.stderr, file=sys.stderr)
        return False
    result = subprocess.run(
        ["cmake", "--build", BUILD_DIR, "--parallel"], capture_output=True, text=True
    )
    if result.returncode != 0:
        print("CMake build failed:", result.stderr, file=sys.stderr)
        return False
    binary = os.path.join(BUILD_DIR, "iteraxive")
    if os.path.exists(binary):
        print(f"Build successful: {binary}")
        return True
    return False


def build_direct():
    compiler = find_compiler()
    if not compiler:
        print("No C++ compiler found!", file=sys.stderr)
        return False
    sources = [
        os.path.join(SRC_DIR, "main.cpp"),
        os.path.join(SRC_DIR, "benchmarkAnalyzer.cpp"),
    ]
    output = os.path.join(PROJECT_ROOT, "iteraxive")
    cmd = (
        [compiler, "-std=c++17", "-O2"] + sources + ["-o", output, "-lpthread", "-ldl"]
    )
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print("Build failed:", result.stderr, file=sys.stderr)
        return False
    print(f"Build successful: {output} (using {compiler})")
    return True


def main():
    if build_with_cmake():
        return 0
    print("CMake build unavailable, falling back to direct build...")
    if build_direct():
        return 0
    return 1


if __name__ == "__main__":
    sys.exit(main())
