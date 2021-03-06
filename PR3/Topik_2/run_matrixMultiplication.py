import subprocess

def findingOptimalGridBlock():
  arr = []
  sizesPow = [5, 7, 9, 11, 13]
  blocksDimPow = [5, 4, 3, 2, 1]
  for sizePow in sizesPow:
    size = 2**sizePow
    gridsDimPow = [sizePow-i for i in blocksDimPow]
    for counter in range(5):
      blockDim = 2**blocksDimPow[counter]
      gridDim = 2**gridsDimPow[counter]
      cmd = "./matrixMultiplication_nonshared.o 1 {0} {1} {1} {2} {2}".format(size, gridDim, blockDim)
      res = subprocess.check_output(cmd.split(" "))
      arr.append(res.decode("utf-8").strip())
      print(arr[-1])
  with open("result/result_varGridBlock.txt", "w") as f:
    for ar in arr:
      print(ar, file=f)

findingOptimalGridBlock()
