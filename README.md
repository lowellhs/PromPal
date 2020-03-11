# PromPal

## Cara compile :
~~~
./compile.sh
~~~
___

### Dokummen PR1 Group 7
___
Anggota Kelompok :<br>
Alif Fadila Safriyanto - 1506688765<br>
Lowell Hagabeon Sianipar - 1606881001<br>
Melkisedek Bernadius Sibarani - 1606826432<br> 
Muhammad Rasyid Gatra Wijaya - 1606875781<br>

Pada folder source-code_group-7 :
- Terdiri dari 3 folder yang berkaitan dengan masing-masing topik :
  - Topik_1_2/
  - Topik_3/
  - Topik_4/
- Data uji pada setiap topik disimpan pada folder test_input :
  - Topik_1_2/test_input
  - Topik_3/test_input
  - Topik_4/test_input
- Terdapat script untuk melakukan otomasi pengujian paralel yaitu test_time.sh<br>
  Contoh cara eksekusi :
  - `bash Topik_1_2/test_time.sh 128 128 B hostfile 2 4 8 16 32`<br>
	Perintah ini melakukan pengujian **perkalian matriks-vekor dan matriks-matriks** dengan ukuran matriks 128*128 pada lingkungan B (multi-core Fasilkom) dengan variasi prosesor adalah 2, 4, 8, 16 dan 32. Hasil disimpan berupa .txt pada folder test_output yang bersesuaian.<br>
  - `bash Topik_3/test_time.sh 128 B hostfile 2 4 8 16 32`<br>
	Perintah ini melakukan pengujian **jacobi iteration** dengan ukuran matriks 128*128 pada lingkungan B (multi-core Fasilkom) dengan variasi prosesor adalah 2, 4, 8, 16 dan 32. Hasil disimpan berupa .txt pada folder test_output yang bersesuaian.<br>
  - `bash Topik_1_2/test_time.sh 150 B hostfile 2 5`<br>
	Perintah ini melakukan pengujian dengan ukuran data train 150 baris pada lingkungan B (multi-core Fasilkom dengan variasi prosesor adalah 2 dan 5. Hasil disimpan berupa .txt pada folder test_output yang bersesuaian.<br>
- Hasil eksekusi pengujian waktu disimpan pada folder test_output
  - Topik_1_2/test_output
  - Topik_3/test_output
  - Topik_4/test_output
- Terdapat juga script untuk melakukan otomasi pengujian kebenaran, untuk topik 1 dan 2 menggunakan script python (test_validity.py) sedangkan topik 3 menggunakan script bash (test_validity.sh). Topik 4 cukup melihat akurasi dari output pengujian waktu.
- Terdapat script Python untuk memudahkan dalam mengambil waktu komputasi dan komunikasi dari setiap file output (pada folder test_output).
