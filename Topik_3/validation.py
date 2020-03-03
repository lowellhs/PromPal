import sys
import os
import re

fileX = sys.argv[1]
env = sys.argv[2]

def fileToVector(f_obj):
  return [float(x.strip()) for x in f_obj.readlines()]

def distanceVector(vecA, vecB):
  res = 0.0
  for i in range(len(vecA)):
    res += ((vecA[i] - vecB[i])**2)
  return res**(0.5)

with open(fileX, "r") as f_obj:
  validVectorX = fileToVector(f_obj)
pattern = re.compile("^{0}_[0-9]+_output_[0-9]+.txt$".format(env))
outputFile = [f for f in os.listdir("test_output/") if pattern.match(f)]

procsToFile = {}
for file_out in outputFile:
  splitted = file_out.split(".")[0].split("_")
  procsToFile[int(splitted[1])] = file_out

stdOut = "Distance calculated vector x with valid vector x (matrix size %dx%d, %d processors, environment %s)"
for keyFile in sorted(procsToFile.keys()):
  fileX_hat = procsToFile[keyFile]
  splitted = fileX_hat.split(".")[0].split("_")
  with open("test_output/" + fileX_hat, "r") as f_obj:
    x_hat = fileToVector(f_obj)
  stdOutStr = stdOut % (int(splitted[3]), int(splitted[3]), int(splitted[1]), splitted[0])
  stdOutStr2 = "%.9f" % (distanceVector(x_hat, validVectorX))
  print('{:98s} : {}'.format(stdOutStr, stdOutStr2))
  