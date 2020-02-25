
import matplotlib.pyplot as plt

matSize = [128, 256, 512, 1024]

sequentialMatVec = [
  0.000067055,
  0.000270357,
  0.001051702,
  0.004210508
]
procs2RowAMatVec = [
  0.000159539,
  0.000529272,
  0.002005101,
  0.007742794
]
procs2ColAMatVec = [
  0.000165040,
  0.000568246,
  0.002461437,
  0.006789074
]
procs2RowBMatVec = [
  0.000086993,
  0.000238337,
  0.001258601,
  0.003815407
]
procs2ColBMatVec = [
  0.000135525,
  0.000316443,
  0.001242911,
  0.004191372
]

plt.plot(matSize, sequentialMatVec, '-x', label="Sekuensial")
plt.plot(matSize, procs2RowAMatVec, '--o', label="Point-to-Point Row-wise")
plt.plot(matSize, procs2ColAMatVec, '--o', label="Point-to-Point Col-wise")
plt.plot(matSize, procs2RowBMatVec, '--o', label="Collective Row-wise")
plt.plot(matSize, procs2ColBMatVec, '--o', label="Collective Col-wise")
plt.ylabel("Waktu (detik)")
plt.xlabel("n (ukuran matriks)")
plt.legend(loc="upper left")
plt.title("Perkalian Matriks-Vektor, Sekuensial dan Paralel (2 Processor)")
plt.show()

procs4RowAMatVec = [
  0.000138830,
  0.000290274,
  0.001050410,
  0.003695162
]
procs4ColAMatVec = [
  0.000222137,
  0.000604885,
  0.002241484,
  0.005017041
]
procs4RowBMatVec = [
  0.000089097,
  0.000201087,
  0.000564068,
  0.003270565
]
procs4ColBMatVec = [
  0.000136306,
  0.000303940,
  0.000729579,
  0.003287646
]

plt.plot(matSize, sequentialMatVec, '-x', label="Sekuensial")
plt.plot(matSize, procs4RowAMatVec, '--o', label="Point-to-Point Row-wise")
plt.plot(matSize, procs4ColAMatVec, '--o', label="Point-to-Point Col-wise")
plt.plot(matSize, procs4RowBMatVec, '--o', label="Collective Row-wise")
plt.plot(matSize, procs4ColBMatVec, '--o', label="Collective Col-wise")
plt.ylabel("Waktu (detik)")
plt.xlabel("n (ukuran matriks)")
plt.legend(loc="upper left")
plt.title("Perkalian Matriks-Vektor, Sekuensial dan Paralel (4 Processor)")
plt.show()

procs8RowAMatVec = [
  0.000225233,
  0.000422462,
  0.000899237,
  0.004070786
]
procs8ColAMatVec = [
  0.000446137,
  0.001106586,
  0.004129446,
  0.010044240
]
procs8RowBMatVec = [
  0.000169639,
  0.000266049,
  0.000430127,
  0.002549803
]
procs8ColBMatVec = [
  0.000176391,
  0.000308839,
  0.000502021,
  0.002672553
]

plt.plot(matSize, sequentialMatVec, '-x', label="Sekuensial")
plt.plot(matSize, procs8RowAMatVec, '--o', label="Point-to-Point Row-wise")
plt.plot(matSize, procs8ColAMatVec, '--o', label="Point-to-Point Col-wise")
plt.plot(matSize, procs8RowBMatVec, '--o', label="Collective Row-wise")
plt.plot(matSize, procs8ColBMatVec, '--o', label="Collective Col-wise")
plt.ylabel("Waktu (detik)")
plt.xlabel("n (ukuran matriks)")
plt.legend(loc="upper left")
plt.title("Perkalian Matriks-Vektor, Sekuensial dan Paralel (8 Processor)")
plt.show()

procs16RowAMatVec = [
  0.000441508,
  0.000610415,
  0.001208717,
  0.003168073
]
procs16ColAMatVec = [
  0.001008601,
  0.002042401,
  0.006854457,
  0.014700935
]
procs16RowBMatVec = [
  0.000342152,
  0.000404078,
  0.000588464,
  0.001563934
]
procs16ColBMatVec = [
  0.000399580,
  0.000464571,
  0.000910037,
  0.002516881
]

plt.plot(matSize, sequentialMatVec, '-x', label="Sekuensial")
plt.plot(matSize, procs16RowAMatVec, '--o', label="Point-to-Point Row-wise")
plt.plot(matSize, procs16ColAMatVec, '--o', label="Point-to-Point Col-wise")
plt.plot(matSize, procs16RowBMatVec, '--o', label="Collective Row-wise")
plt.plot(matSize, procs16ColBMatVec, '--o', label="Collective Col-wise")
plt.ylabel("Waktu (detik)")
plt.xlabel("n (ukuran matriks)")
plt.legend(loc="upper left")
plt.title("Perkalian Matriks-Vektor, Sekuensial dan Paralel (16 Processor)")
plt.show()

procs32RowAMatVec = [
  0.000852809,
  0.000995097,
  0.001733862,
  0.003871913
]
procs32ColAMatVec = [
  0.001554125,
  0.003875529,
  0.013237801,
  0.027606322
]
procs32RowBMatVec = [
  0.000575139,
  0.000655349,
  0.004149113,
  0.003015787
]
procs32ColBMatVec = [
  0.000513793,
  0.000910458,
  0.001351786,
  0.004702992
]

plt.plot(matSize, sequentialMatVec, '-x', label="Sekuensial")
plt.plot(matSize, procs32RowAMatVec, '--o', label="Point-to-Point Row-wise")
plt.plot(matSize, procs32ColAMatVec, '--o', label="Point-to-Point Col-wise")
plt.plot(matSize, procs32RowBMatVec, '--o', label="Collective Row-wise")
plt.plot(matSize, procs32ColBMatVec, '--o', label="Collective Col-wise")
plt.ylabel("Waktu (detik)")
plt.xlabel("n (ukuran matriks)")
plt.legend(loc="upper left")
plt.title("Perkalian Matriks-Vektor, Sekuensial dan Paralel (32 Processor)")
plt.show()


sequentialMatMat = [
  0.009261412,
  0.060055188,
  0.541839449,
  4.370192325
]
procs2RowAMatMat = [
  0.008721629,
  0.062153484,
  0.579139363,
  4.731565447
]
procs2RowBMatMat = [
  0.007039554,
  0.038013504,
  0.273560838,
  2.481837446
]

plt.plot(matSize, sequentialMatMat, '-', label="Sekuensial")
plt.plot(matSize, procs2RowAMatMat, '--o', label="Point-to-Point Row-wise")
plt.plot(matSize, procs2RowBMatMat, '--o', label="Collective Row-wise")
plt.ylabel("Waktu (detik)")
plt.xlabel("n (ukuran matriks)")
plt.legend(loc="upper left")
plt.title("Perkalian Matriks-Matriks, Sekuensial dan Paralel (2 Processor)")
plt.show()

procs4RowAMatMat = [
  0.004236857,
  0.026317766,
  0.199478630,
  2.167932292
]
procs4RowBMatMat = [
  0.004057450,
  0.019609662,
  0.144603137,
  1.364499313
]

plt.plot(matSize, sequentialMatMat, '-', label="Sekuensial")
plt.plot(matSize, procs4RowAMatMat, '--o', label="Point-to-Point Row-wise")
plt.plot(matSize, procs4RowBMatMat, '--o', label="Collective Row-wise")
plt.ylabel("Waktu (detik)")
plt.xlabel("n (ukuran matriks)")
plt.legend(loc="upper left")
plt.title("Perkalian Matriks-Matriks, Sekuensial dan Paralel (4 Processor)")
plt.show()

procs8RowAMatMat = [
  0.003218267,
  0.018229714,
  0.127218558,
  1.605746550
]
procs8RowBMatMat = [
  0.003796832,
  0.014541786,
  0.116115883,
  0.727712337
]

plt.plot(matSize, sequentialMatMat, '-', label="Sekuensial")
plt.plot(matSize, procs8RowAMatMat, '--o', label="Point-to-Point Row-wise")
plt.plot(matSize, procs8RowBMatMat, '--o', label="Collective Row-wise")
plt.ylabel("Waktu (detik)")
plt.xlabel("n (ukuran matriks)")
plt.legend(loc="upper left")
plt.title("Perkalian Matriks-Matriks, Sekuensial dan Paralel (8 Processor)")
plt.show()

procs16RowAMatMat = [
  0.002122552,
  0.009091673,
  0.066872725,
  0.774256182
]
procs16RowBMatMat = [
  0.002074751,
  0.008474265,
  0.060412879,
  0.500455954
]

plt.plot(matSize, sequentialMatMat, '-', label="Sekuensial")
plt.plot(matSize, procs8RowAMatMat, '--o', label="Point-to-Point Row-wise")
plt.plot(matSize, procs8RowBMatMat, '--o', label="Collective Row-wise")
plt.ylabel("Waktu (detik)")
plt.xlabel("n (ukuran matriks)")
plt.legend(loc="upper left")
plt.title("Perkalian Matriks-Matriks, Sekuensial dan Paralel (16 Processor)")
plt.show()

procs32RowAMatMat = [
  0.002707488,
  0.010387614,
  0.066661119,
  0.639280973
]
procs32RowBMatMat = [
  0.002128212,
  0.007384021,
  0.034347467,
  0.306151861
]

plt.plot(matSize, sequentialMatMat, '-', label="Sekuensial")
plt.plot(matSize, procs8RowAMatMat, '--o', label="Point-to-Point Row-wise")
plt.plot(matSize, procs8RowBMatMat, '--o', label="Collective Row-wise")
plt.ylabel("Waktu (detik)")
plt.xlabel("n (ukuran matriks)")
plt.legend(loc="upper left")
plt.title("Perkalian Matriks-Matriks, Sekuensial dan Paralel (32 Processor)")
plt.show()