import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

names = ["sequential", "mpi", "nonshared", "shared", "cublas"]
resultsArr = []
for name in names:
  df = pd.read_csv('./result_matmul_{}_v2.txt'.format(name), header=None, delim_whitespace=True)
  df = df.loc[df.groupby(0)[1].idxmin()].set_index(0)
  resultsArr.append(df[1])

results = pd.concat(resultsArr, axis=1, sort=False).iloc[0:, 0:]
results.columns = ['Sequential', 'Multi-core', 'CUDA non-shared', 'CUDA shared', 'cuBLAS']
results.index.names = ['Ukuran']
print(results)

if False:
  # All sizes, all program
  results = pd.concat(resultsArr, axis=1, sort=False).iloc[0:, 0:]
  results.columns = ['Sequential', 'Multi-core', 'CUDA non-shared', 'CUDA shared', 'cuBLAS']
  ax = results.plot.line(marker="o")
  ax.set_xlabel("Ukuran matriks")
  ax.set_ylabel("Waktu (s)")
  plt.show()

  # All sizes, only cuda program
  results = pd.concat(resultsArr, axis=1, sort=False).iloc[0:, 2:]
  results.columns = ['CUDA non-shared', 'CUDA shared', 'cuBLAS']
  ax = results.plot.line(marker="o")
  ax.set_xlabel("Ukuran matriks")
  ax.set_ylabel("Waktu (s)")
  plt.show()

  # Small sizes, only cuda program
  results = pd.concat(resultsArr, axis=1, sort=False).iloc[0:10, 2:]
  results.columns = ['CUDA non-shared', 'CUDA shared', 'cuBLAS']
  ax = results.plot.line(marker="o")
  ax.set_xlabel("Ukuran matriks")
  ax.set_ylabel("Waktu (s)")
  plt.show()
