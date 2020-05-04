import subprocess
import sys
import os
import re

for size in [2**20]:
	for blockSize in [1, 2, 4, 8, 16, 32, 64, 128, 256, 512]:
		nBlocks = size // blockSize
		cmd = 'nvprof --log-file result/result_incArr_{0}_{1}_{2}.txt ./incrementArrays.o {0} {1} {2}'.format(size, nBlocks, blockSize).split(" ")
		res = subprocess.check_output(cmd)
		print(" ".join([line.decode("utf-8") for line in res.splitlines()]))
