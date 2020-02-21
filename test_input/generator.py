import random
import sys

m = int(sys.argv[1])
n = int(sys.argv[2])
flag = int(sys.argv[3])

def func(i, j):
  if i == j:
    return 1
  return 0

def func2(i, j):
  return random.random()

matrix = None

if sys.argv[3] == "1":
  matrix = [[func(i, j) for j in range(n)] for i in range(m)]
else:
  matrix = [[func2(i, j) for j in range(n)] for i in range(m)]

for line in matrix:
  print(" ".join(["%.6f" % (x) for x in line]))