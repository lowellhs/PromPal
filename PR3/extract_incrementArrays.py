import os
import re
import sys

name = sys.argv[1]

def toSeconds(timeStr):
  match = re.match('([0-9\.]*)(s|ms|us|ns)', timeStr)
  if match:
    t = float(match.group(1))
    val = match.group(2)
    if val == "ns":
      t *= 1E-9
    elif val == "us":
      t *= 1E-6
    elif val == "ms":
      t *= 1E-3
    return t    

if name == "incArr":
  res = []
  for filePath in os.listdir('./result'):
    match = re.match('result\_'+name+'\_([0-9]+)\_([0-9]+)\_([0-9]+).txt', filePath)
    if match:
      with open("result/"+filePath, "r") as fObj:
        textLine = fObj.readlines()
      lineSelected = [line for line in textLine if "incrementArrayOnDevice" in line][0]
      lineSelected = lineSelected.replace("GPU activities:", "")
      splitted = re.sub("\s+", " ", lineSelected).split(" ")
      time = ("%.6f" % toSeconds(splitted[2]))
      size = int(match.group(1))
      nBlocks = int(match.group(2))
      blockSize = int(match.group(3))
      res.append((size, nBlocks, blockSize, time))
  res.sort(key=lambda tup: (tup[0], tup[1]))
  for i in res:
    print("%20s %20s %10s %10s" % i)
elif name == "incArr2":
  res = []
  for filePath in os.listdir('./result'):
    match = re.match('result\_'+name+'\_([0-9]+)\_([0-9]+)-([0-9]+)\_([0-9]+)-([0-9]+).txt', filePath)
    if match:
      with open("result/"+filePath, "r") as fObj:
        textLine = fObj.readlines()
      lineSelected = [line for line in textLine if "incrementArrayOnDevice" in line][0]
      lineSelected = lineSelected.replace("GPU activities:", "")
      splitted = re.sub("\s+", " ", lineSelected).split(" ")
      time = ("%.6f" % toSeconds(splitted[2]))
      size = int(match.group(1))
      blockDimX = int(match.group(2))
      blockDimY = int(match.group(3))
      gridDimX = int(match.group(4))
      gridDimY = int(match.group(5))
      res.append((size, gridDimX, gridDimY, blockDimX, blockDimY, time))
  res.sort(key=lambda tup: (tup[1], tup[2], tup[3], tup[4]))
  for i in res:
    print("%10s %10s %10s %10s" % (i[0], str((i[1],i[2])), str((i[3],i[4])), i[5]))

