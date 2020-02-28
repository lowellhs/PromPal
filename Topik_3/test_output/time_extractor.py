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
  totalTime = []; allIter = []; allError = []; commTime = []
  procs = [1, 2, 4, 8, 16, 32]
  currProcs = 0
  currLine = 1
  while currProcs < len(procs) and currLine < len(lines):
    totals = []; iters = []; errors = []; commTimes = [0, 0, 0]
    ctr = -1
    while lines[currLine] != "":
      splitted = lines[currLine].split(" ")
      if splitted[0] == "TOTAL":
        totals.append(float(splitted[3]))
        allIter.append(float(splitted[6].replace("count=", "").replace(",", "")))
        allError.append(float(splitted[7].replace("error=", "")))
      elif len(splitted) >= 3 and splitted[2] == "communication" and splitted[0] == "Processor" and splitted[1] == "0,":
        commTimes[ctr] = (float(splitted[5]))
      elif lines[currLine] == "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~":
        ctr += 1
      currLine += 1
    minTot, ix = findMin(totals)
    totalTime.append(minTot)
    commTime.append(commTimes[ix])
    currProcs += 1
    currLine += 1
  return totalTime, commTime, sum(allIter)/len(allIter), sum(allError)/len(allError)

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
  totalTime, commTime, avgIter, avgError = extractTime(lines)
  print(" ".join(["%.6f" % (x) for x in totalTime]))
  print(" ".join(["%.6f" % (x) for x in commTime]))
  print("%.6f" % avgIter)
  print("%.6f" % avgError)