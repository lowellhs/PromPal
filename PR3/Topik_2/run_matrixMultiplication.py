import subprocess

def findingOptimalGridBlock():
  arr = []
  sizesPow = [8, 9, 10, 11, 12, 13]
  blocksDimPow = [5, 4, 3, 2, 1]
  for sizePow in sizesPow:
    size = 2**sizePow
    gridsDimPow = [sizePow-i for i in blocksDimPow]
    print("\n{0} * {0}".format(size))
    arr.append("{0} * {0}".format(size))
    for counter in range(5):
      blockDim = 2**blocksDimPow[counter]
      gridDim = 2**gridsDimPow[counter]
      cmd = "./matrixMultiplication.o {0} 2 {1} {1} {2} {2}".format(size, gridDim, blockDim)
      res = subprocess.check_output(cmd.split(" "))
      lines = [line.decode("utf-8") for line in res.splitlines()]
      arr.append("{0:<40s} {1} {2}".format(lines[0], lines[1], lines[2]))
      print(arr[-1])
    arr.append("")
    print(arr[-1])
  with open("result/result_varGridBlock.txt", "w") as f:
    for ar in arr:
      print(ar, file=f)

findingOptimalGridBlock()

