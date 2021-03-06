## Hasil Pengujian Program untuk Algoritma Convolutional Neural Network
| Lingkungan                | Jumlah _epochs_ | _Test Loss_ | Akurasi | Waktu(s) |
|---------------------------|:---------------:|:-----------:|:-------:|:--------:|
| CPU                       | 12              | 0.0277      | 0.9913  | 295.4479 |
| GPU (GeForce GTX 1080)    | 12              | 0.0266      | 0.9922  | 56.7259  |
| GPU (GeForce RTX 2080 Ti) | 12              | 0.0264      | 0.9918  | 47.9811  |
## Cara Penggunaan Program
Kepada Yth. Tim Dosen Pemrograman Paralel. <br>
#### Untuk menjalankan program:<br>
Masuk ke pods (untuk penjalanan program di CPU semua pods dapat digunakan)
- Dengan menggunakan GPU NVIDIA GeForce GTX 1080:

```bash
$ kubectl exec -it group-07-nfs-cuda /bin/bash
``` 

- Dengan menggunakan GPU NVIDIA GeForce RTX 2080 Ti:

```bash
$ kubectl exec -it group-07-nfs-cuda-02 /bin/bash
``` 
<br>

Kemudian buka direktori dengan _command_ berikut  
```bash 
cd /var/nfs/group07/TEMP/PromPal/PR3/Topik_4/deep-learning-implementation-on-gpu/convolutional-neural-network
```

Kemudian buka _environment_ TensorFlow GPU dengan _command_ berikut:  
- Dengan menggunakan GPU NVIDIA GeForce GTX 1080:   
```bash
source activate deeplearning
```    
Lalu jalankan   
```bash
conda activate tensorflow_gpu
```
- Dengan menggunakan GPU NVIDIA GeForce RTX 2080 Ti:    
```bash
source activate deep2080
```  
Lalu jalankan   
```bash 
conda activate tf_gpu2080
```

#### Menjalankan Program:
- CPU : jalankan 
```bash 
python3 mnist-knn.py
```   
kemudian input `cpu`
- GPU NVIDIA GeForce GTX 1080: jalankan 
```bash
python3 mnist-knn.py
```
input `gpu 0` atau `gpu 1`
<br>
- GPU NVIDIA GeForce RTX 2080 Ti: jalankan 
```bash 
python3 cnn2080.py
``` 
input `gpu 0` atau `gpu 1`    

#### Hasil Pengujian
Hasil dapat dilihat pada direktori atau _folder_ ```results/```. 

---
<div align="right">
Group 07 - Pemrograman Paralel 
</div>
