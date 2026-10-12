import numpy as np

A = np.loadtxt("A.txt", skiprows=1, dtype=np.int64)
B = np.loadtxt("B.txt", skiprows=1, dtype=np.int64)
C = np.loadtxt("C.txt", skiprows=1, dtype=np.int64)

expected = A @ B

if np.array_equal(C, expected):
    print("OK: matrices match")
else:
    print("ERROR: matrices do not match")
    print("Expected:")
    print(expected)
    print("Actual:")
    print(C)