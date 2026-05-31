#!/usr/bin/env python3
import sys
import time


def fibonacci(n):
    if n <= 1:
        return n
    a, b = 0, 1
    for _ in range(2, n + 1):
        a, b = b, a + b
    return b


def main():
    n = int(sys.argv[1]) if len(sys.argv) > 1 else 35
    start = time.perf_counter()
    result = fibonacci(n)
    elapsed = time.perf_counter() - start
    print(f"fibonacci({n}) = {result}")
    print(f"time: {elapsed:.4f}s")


if __name__ == "__main__":
    main()
