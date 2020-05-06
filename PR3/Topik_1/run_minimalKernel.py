import subprocess
import sys
import os
import re

r  = [
        (20,1,20),
        (20,2,10),
        (20,4,5),
        (20,5,4),
        (20,10,2),
        (20,20,1)
]
for item in r:
    n, block, thread = item
    cmd = 'nvprov --log-file result/minimalKernel/result_minimalKernel_{0}_{1}.txt ./minimalKernel.o {0} {1}'.format(block, thread).split(" ")
    res = subprocess.check_output(cmd)
    print(" ".join([line.decode("utf-8") for line in res.splitlines()]))