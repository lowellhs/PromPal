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


def totalTimeExtractor(procs, fileData):
  results = []
  times = []
  currLine = 25
  while currLine < len(fileData):
    if (fileData[currLine] == "\n" or currLine == len(fileData)-1) :
      results.append(findMin(times))
      times = []
    else:
      lineData = fileData[currLine].split(" ")
      if (lineData[0] == "TOTAL" and lineData[1] == "TIME"):
        times.append(float(lineData[3]))
    currLine += 1
  return results

def commpTimeExtractor(totalTimes, fileData):
  results = []
  currLine = 25
  counter = 0

  for y in range(3):
    comm, comp = [], []
    cursor = -1
    while (fileData[currLine] != "\n"):
      if (fileData[currLine] == "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"):
        cursor += 1
      else:
        splitted = fileData[currLine].split(" ")
        if cursor == totalTimes[counter][1]:
          if (splitted[0] == "Send-Receive"):
            comm.append(float(splitted[-2]))
          elif (splitted[0] == "Process"):
            comp.append(float(splitted[-2]))
      currLine += 1
    results.append((comm, comp))
    currLine += 1
    counter += 1

  tmpResults = []
  for x,y in results:
    realComm = []
    for i in range(len(x)):
      realComm.append(x[i] - y[i])
    tmpResults.append((realComm, y))
  results = tmpResults

  currLine += 1
  
  for y in range(3):
    comm, comp = [], []
    cursor = -1
    while currLine < len(fileData) and (fileData[currLine] != "\n"):
      if (fileData[currLine] == "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"):
        cursor += 1
      else:
        splitted = fileData[currLine].split(" ")
        if cursor == totalTimes[counter][1]:
          if (splitted[0] == "Process" and splitted[1] == "0,"):
            comp.append(float(splitted[3]))
            comm.append(float(splitted[6]) + float(splitted[9]))
      currLine += 1
    results.append((comm, comp))
    currLine += 1
    counter += 1
  
  return results

environment = sys.argv[1]
option = sys.argv[2]
select = sys.argv[3]
for size in ["128", "256", "512", "1024"]:
  files = {}
  for output in os.listdir():
    temp1 = output.split(".")
    if temp1[1] == "txt":
      temp2 = temp1[0].split("_")
      if temp2[0] == environment and temp2[2].split("x")[0] == size:
        files[int(temp2[-1])] = output

  algosTotalTime = {}
  algosCommTime = {}
  algosCompTime = {}

  keys = [
    "point-to-point matvec row",
    "point-to-point matvec col",
    "point-to-point matmat row",
    "collective matvec row",
    "collective matvec col",
    "collective matmat row",
  ]
  for i in [algosTotalTime, algosCommTime, algosCompTime]:
    for key in keys:
      i[key] = []

  for procs in [2, 4, 8, 16, 32]:
    inFile = files[procs]
    with open(inFile, "r") as f:
      lines = [""] + f.readlines()
    totalTime = totalTimeExtractor(procs, lines)
    commp = commpTimeExtractor(totalTime, lines)
    totalTimeF = [x for x,y in totalTime]
    avgCommTimeF = []
    avgCompTimeF = []
    for x,y in commp:
      avgx = sum(x) / len(x)
      avgy = sum(y) / len(y)
      avgCommTimeF.append(avgx)
      avgCompTimeF.append(avgy)

    for i in range(len(totalTime)):
      algosTotalTime[keys[i]].append(totalTimeF[i])
      algosCommTime[keys[i]].append(avgCommTimeF[i])
      algosCompTime[keys[i]].append(avgCompTimeF[i])
  
  if option == "0":
    print(" ".join(["%.6f" % (x) for x in algosTotalTime[keys[int(select)]]]))
  elif option == "1":
    print(" ".join(["%.6f" % (x) for x in algosCommTime[keys[int(select)]]]))
  elif option == "2":
    print(" ".join(["%.6f" % (x) for x in algosCompTime[keys[int(select)]]]))

