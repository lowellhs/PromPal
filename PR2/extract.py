import os
import re

def toSeconds(tuple):
  n = float(tuple[0])
  ss = tuple[1]
  if ss == "μs":
    n /= 1E+6
  elif ss == "ms":
    n /= 1E+3
  return n

ns = [32, 64, 128, 256, 512]
ps = [1, 2, 4]

maps = {}
for p in ps:
  maps[p] = {}
  for n in ns:
    maps[p][n] = None

for filePath in os.listdir():
  match = re.match('test\_([0-9])\_([0-9]+).txt', filePath)
  if match:
    with open(filePath, "r") as fObj:
      textLine = fObj.readlines()
    p = int(match.group(1))
    n = int(match.group(2))
    seq = textLine[0].split(" ")[2:4]
    par = textLine[1].split(" ")[2:4]
    maps[p][n] = toSeconds(par)
    maps[1][n] = toSeconds(seq)

for p in ps:
  print(p, " Processors")
  for n in ns:
    print("%.6f" % maps[p][n])
  print()
