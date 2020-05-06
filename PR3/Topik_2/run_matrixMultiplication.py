import subprocess

def findingOptimalGridBlock():
  sizesPow = [7, 8, 9, 10, 11]
  blocksDimPow = [5, 4, 3, 2, 1]
  for sizePow in sizesPow:
    size = 2**sizePow
    gridsDimPow = [sizePow-i for i in blocksDimPow]
    print("\n{0} * {0}".format(size))
    for counter in range(5):
      blockDim = 2**blocksDimPow[counter]
      gridDim = 2**gridsDimPow[counter]
      cmd = "./matrixMultiplication.o {0} {1} {1} {2} {2}".format(size, gridDim, blockDim)
      res = subprocess.check_output(cmd.split(" "))
      print(" ".join([line.decode("utf-8") for line in res.splitlines()]))

findingOptimalGridBlock()

