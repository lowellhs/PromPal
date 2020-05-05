import subprocess
import sys
import os
import re

for sizePow in [28, 30]:
        size = 2**sizePow
        for nBlocksPow in range(sizePow-10, sizePow+1, 2):
                nBlocks = 2**nBlocksPow
                blockSize = size // nBlocks
                cmd = 'nvprof --log-file result/result_incArr_{0}_{1}_{2}.txt ./incrementArrays.o {0} {1}'.format(size, nBlocks, blockSize).split(" ")
                res = subprocess.check_output(cmd)
                print(" ".join([line.decode("utf-8") for line in res.splitlines()]))
'''
for size in [2**20]:
	for dim in [2**5, 2**6, 2**7, 2**8, 2**9, 2**10]:
		nBlocks = dim**2
		blockSize = size // nBlocks
		cmd = 'nvprof --log-file result/result_incArr2_{0}_{1}_{2}.txt ./incrementArrays_2d.o {0} {3} {3}'.format(size, nBlocks, blockSize, dim).split(" ")
		res = subprocess.check_output(cmd)
		print(" ".join([line.decode("utf-8") for line in res.splitlines()]))
'''
