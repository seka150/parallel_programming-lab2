import numpy as np
import sys

if len(sys.argv) != 2:
    print("Usage: python gen.py <size>")
    sys.exit(1)

n = int(sys.argv[1])

if n <= 0:
    print("Error: size must be positive")
    sys.exit(1)

A = np.random.randint(0, 10, size=(n, n))
B = np.random.randint(0, 10, size=(n, n))

with open("A.txt", "w") as f:
    f.write(f"{n}\n")
    np.savetxt(f, A, fmt="%d")

with open("B.txt", "w") as f:
    f.write(f"{n}\n")
    np.savetxt(f, B, fmt="%d")

print(f"Generated two {n}x{n} matrices")

