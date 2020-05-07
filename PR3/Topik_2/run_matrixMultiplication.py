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
      cmd = "./matrixMultiplication.o 1 {0} {1} {1} {2} {2}".format(size, gridDim, blockDim)
      res = subprocess.check_output(cmd.split(" "))
      arr.append(res.decode("utf-8").strip())
      print(arr[-1])
  with open("result/result_varGridBlock.txt", "w") as f:
    for ar in arr:
      print(ar, file=f)

def compareCompTime():
  sizesPow = [8, 9, 10, 11, 12, 13, 14]
  for sizePow in sizesPow:
    print("\n{0} * {0}".format(2**sizePow))
    with open("result/result_matmul_{0}.txt".format(2**sizePow), "w") as fobj:
      for execFile in ["./matrixMultiplication.o", "./matrixMultiplication_cublas.o"]:
        size = 2**sizePow
        cmd = "nvprof --log-file result/temp.txt {0} {1} 2".format(execFile, size)
        res = subprocess.check_output(cmd.split(" "))
        lines = [line.decode("utf-8") for line in res.splitlines()]
        print(execFile, file=fobj)
        print("\n".join(lines), file=fobj)
        print("", file=fobj)

findingOptimalGridBlock()
# compareCompTime()
