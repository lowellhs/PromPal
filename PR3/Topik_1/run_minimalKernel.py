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

THIS_FOLDER = os.path.dirname(os.path.abspath(__file__))
MINIMALKERNEL_RESULT_PATH = os.path.join(THIS_FOLDER, 'result/minimalKernel/')
if not os.path.exists(MINIMALKERNEL_RESULT_PATH):
    os.makedirs(MINIMALKERNEL_RESULT_PATH)

for item in r:
        n, block, thread = item
        cmd = './minimalKernel.o {0} {1} {2}'.format(n, block, thread).split(" ")
        res = subprocess.check_output(cmd)
        result_file = os.path.join(MINIMALKERNEL_RESULT_PATH, 'result_minimalKernel_{0}_{1}.txt'.format(block, thread))
        file = open(result_file, 'w') 
        file.write(res.decode('utf-8'))
        file.close()
        print('<<<{0}, {1}>>>'.format(block, thread))
        print(res.decode('utf-8'))
        print()
