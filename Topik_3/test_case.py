import numpy as np
import sys

n = int(sys.argv[1])

def print_matrix(A, fileObj=None):
    for row in A:
        print(" ".join(["%.9f" % x for x in row]), file=fileObj)

def print_vector(A, fileObj=None):
    for row in A:
        print("%.9f" % row, file=fileObj)

A = np.random.rand(n, n)
b = np.random.rand(n)
x = np.dot(np.linalg.inv(A), b)

outA = "matrix_A_%dx%d.txt" % (n, n)
outb = "vector_b_%dx1.txt" % (n)
outx = "vector_x_%dx1.txt" % (n)

with open(outA, "w") as fA:
    print_matrix(A, fA)

with open(outb, "w") as fb:
    print_vector(b, fb)

with open(outx, "w") as fx:
    print_vector(x, fx)
