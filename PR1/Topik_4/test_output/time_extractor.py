import sys
import os
import math

def findMin(arr):
  minVal = math.inf
  c = -1
  for i in range(len(arr)):
    val = arr[i]
    if val < minVal:
      minVal = val
      c = i
  return minVal, c

def extractTime(lines):
  totalTime = []
  commTime = []
  procs = [1, 2, 5, 10, 15, 30]
  currProcs = 0
  currLine = 1
  while currProcs < len(procs) and currLine < len(lines):
    totals = []
    comms = []
    while lines[currLine] != "":
      splitted = lines[currLine].split(" ")
      if splitted[0] == "TOTAL":
        totals.append(float(splitted[3]))
        comms.append(float(splitted[8]))
      currLine += 1
    minTot, ix = findMin(totals)
    totalTime.append(minTot)
    commTime.append(comms[ix])
    currProcs += 1
    currLine += 1
  return totalTime, commTime

env = sys.argv[1]
sizes = [int(x) for x in sys.argv[2::]]

files = {}

for out in os.listdir():
  splitted = out.split(".")
  if len(splitted) == 2 and splitted[1] == "txt":
    splitted2 = splitted[0].split("_")
    if splitted2[0] == env and splitted2[1] == "time":
      files[int(splitted2[2])] = out

for size in sizes:
  with open(files[size], "r") as ff:
    lines = ff.read().split("\n")
  totalTime, commTime = extractTime(lines)
  print(" ".join(["%.6f" % (x) for x in totalTime]))
  print(" ".join(["%.6f" % (x) for x in commTime]))
