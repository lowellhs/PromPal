import subprocess
import sys
import os
import re


for sizePow in [22]:
        size = 2**sizePow
        for nBlocksPow in range(sizePow-10, sizePow+1, 2):
                nBlocks = 2**nBlocksPow
                blockSize = size // nBlocks
                cmd = 'nvprof --log-file result/result_incArr_{0}_{1}_{2}.txt ./incrementArrays.o {0} {1}'.format(size, nBlocks, blockSize).split(" ")
                res = subprocess.check_output(cmd)
                print(" ".join([line.decode("utf-8") for line in res.splitlines()]))
'''
r  = [
        (1024,1,1024,1),
        (512,2,512,2),
        (256,4,256,4),
        (128,8,128,8),
        (64,16,64,16),
        (32,32,32,32)
]
for sizePow in [20]:
        size = 2**sizePow
        for item in r:
                blockDimX, blockDimY, gridDimX, gridDimY = item
                cmd = 'nvprof --log-file result/result_incArr2_{0}_{1}-{2}_{3}-{4}.txt ./incrementArrays_2d.o {0} {1} {2} {3} {4}'.format(size, blockDimX, blockDimY, gridDimX, gridDimY).split(" ")
                res = subprocess.check_output(cmd)
                print(" ".join([line.decode("utf-8") for line in res.splitlines()]))
'''
