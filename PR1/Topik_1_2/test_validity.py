import subprocess
import os
import sys

hostfile = sys.argv[1]
methods = sys.argv[2]

def run(cmd):
  os.system(cmd)

def output(cmd):
  res = subprocess.check_output(cmd.split(" "))
  return [line.decode("utf-8") for line in res.splitlines()]

def diff_vector(vecA, vecB):
  abs_error = 0
  for i in range(len(vecA)):
    abs_error += abs(vecA[i] - vecB[i])
  return abs_error

def diff_matrix(matA, matB):
  abs_error = 0
  for i in range(len(matA)):
    for j in range(len(matA[0])):
      abs_error += abs(matA[i][j] - matB[i][j])
  return abs_error

def read_vector(filePath):
  with open(filePath, "r") as file_obj:
    lines = file_obj.readlines()
  return [float(x.strip()) for x in lines]

def read_matrix(filePath):
  matrix = []
  with open(filePath, "r") as file_obj:
    lines = file_obj.readlines()
  for line in lines:
    matrix.append([float(x) for x in line.strip().split(" ")])
  return matrix

for procs in [2, 3]:
  m = 128
  n = 128

  run('echo "#define m {0}" >  init.c'.format(m))
  run('echo "#define n {0}" >> init.c'.format(n))
  run('bash compile.sh')
  run('rm -f init.c')

  matrix_i      = 'test_input/matrix_i_{0}x{1}.txt'.format(m, n)
  matrix        = 'test_input/matrix_{0}x{1}.txt'.format(m, n)
  vector        = 'test_input/vector_{0}x1.txt'.format(m)
  valid_matrix  = read_matrix(matrix)
  valid_vector  = read_vector(vector)

  if methods == "matvec":
    # MATRIX-VECTOR
    cmds = [
      'mpirun --hostfile {0} -np {1} sequential/seq_matvec.o 1 0 {2} {3}',
      'mpirun --hostfile {0} -np {1} point_to_point/mpi_matvec_row.o 1 0 {2} {3}',
      'mpirun --hostfile {0} -np {1} point_to_point/mpi_matvec_col.o 1 0 {2} {3}',
      'mpirun --hostfile {0} -np {1} collective/mpi_matvec_row.o 1 0 {2} {3}',
      'mpirun --hostfile {0} -np {1} collective/mpi_matvec_col.o 1 0 {2} {3}',
    ]
    cmds[0] = cmds[0].format(hostfile, "1", matrix_i, vector)
    for i in range(1, len(cmds)):
      cmds[i] = cmds[i].format(hostfile, str(procs), matrix_i, vector)

    results = []
    for cmd in cmds:
      results.append([float(x) for x in output(cmd)])

    print("\nMatrix size : {}x{}".format(m, m))
    print("Differences with valid vector (Ix = b) -> (x == b)?")
    print("--------------------------------------------------")
    print("{:40s}: {:.6f}".format("Sequential", diff_vector(valid_vector, results[0])))
    print("{:40s}: {:.6f}".format("Row-wise (point-to-point) "+str(procs)+" processors", diff_vector(valid_vector, results[1])))
    print("{:40s}: {:.6f}".format("Col-wise (point-to-point) "+str(procs)+" processors", diff_vector(valid_vector, results[2])))
    print("{:40s}: {:.6f}".format("Row-wise (collective) "+str(procs)+" processors", diff_vector(valid_vector, results[3])))
    print("{:40s}: {:.6f}".format("Col-wise (collective) "+str(procs)+" processors", diff_vector(valid_vector, results[4])))
  elif methods == "matmat":
    # MATRIX-MATRIX
    cmds = [
      'mpirun --hostfile {0} -np {1} sequential/seq_matmat.o 1 0 {2} {3}',
      'mpirun --hostfile {0} -np {1} point_to_point/mpi_matmat_row.o 1 0 {2} {3}',
      'mpirun --hostfile {0} -np {1} collective/mpi_matmat_row.o 1 0 {2} {3}',
    ]
    cmds[0] = cmds[0].format(hostfile, "1", matrix_i, matrix)
    for i in range(1, len(cmds)):
      cmds[i] = cmds[i].format(hostfile, str(procs), matrix_i, matrix)

    results = []
    for cmd in cmds:
      lines = output(cmd)
      temp = []
      for line in lines:
        temp.append([float(x) for x in line.split(" ")])
      results.append(temp)

    print("\nMatrix size : {}x{}".format(m, m))
    print("Differences with valid matrix (IB = C) -> (B == C)?")
    print("--------------------------------------------------")
    print("{:40s}: {:.6f}".format("Sequential", diff_matrix(valid_matrix, results[0])))
    print("{:40s}: {:.6f}".format("Row-wise (point-to-point) "+str(procs)+" processors", diff_matrix(valid_matrix, results[1])))
    print("{:40s}: {:.6f}".format("Row-wise (collective) "+str(procs)+" processors", diff_matrix(valid_matrix, results[2])))
print()
