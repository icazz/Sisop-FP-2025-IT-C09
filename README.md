## Kelompok C09

| Nama                        | NRP        |
| --------------------------- | ---------- |
| Hansen Chang                | 5027241028 |
| Ica Zika Hamizah            | 5027241058 |
| Muhammad Khosyi Syehab      | 5027241089 |
| Mohamad Arkan Zahir Asyafiq | 5027241120 |

## Deskripsi Soal (Testcase 29)

**[NO TITIK-TITIK]**    
Buat sebuah FUSE file system yang berfungsi sebagai pembersih tampilan. Saat sebuah direktori di-mount, file system akan menyembunyikan semua file dan folder yang namanya diawali dengan titik (.).

### Catatan       
**Penggunaan:**    
1. Compile 
```
gcc -Wall cleanfs.c `pkg-config fuse3 --cflags --libs` -o cleanfs
```

2. Siapkan directory `repo` untuk direktori sumber dan `mount_dir` sebagai tempat mount FUSE
```
mkdir repo
mkdir mount_dir
```

3. Running
```
./cleanfs mount_dir/
```

4. tes dengan membuat file/folder di dalam `repo`
```
touch file.txt
touch .hidden_file.txt
mkdir folder
mkdir .hidden_folder
```

**Struktur repository:**    
1. Sebelum FUSE dijalankan
```
├── cleanfs.c
├── mount_dir
├── README.md
└── repo
    ├── file.txt
    └── folder

3 directories, 3 files
```

2. Setelah FUSE dijalankan
```
├── cleanfs
├── cleanfs.c
├── log.txt
├── mount_dir
│   ├── file.txt
│   └── folder
├── README.md
└── repo
    ├── file.txt
    └── folder

4 directories, 6 files
```

3. Isi file log
```
[13:38:35] [visible] folder "folder" berhasil ditambahkan
[13:38:35] [visible] file "file.txt" berhasil ditambahkan
[13:38:35] [hidden] ".hidden_folder" merupakan folder tersembunyi
[13:38:35] [hidden] ".hidden_file.txt" merupakan file tersembunyi
[13:38:47] [delete] ".hidden_folder" berhasil dihapus
[13:38:54] [delete] "file.txt" berhasil dihapus
```

### Pengerjaan
**Teori**    
Pengembangan file sistem di user-space menawarkan pendekatan yang lebih fleksibel dan aman dibanding pengembangan langsung di kernel. Menurut Aditya Rajgarhia dan Ashish Gehani, pengembangan filesystem di ruang pengguna, terutama dengan framework FUSE, memungkinkan programmer menghindari kompleksitas pemrograman kernel seperti pointer raw, race condition, serta debugging yang sulit.

FUSE (Filesystem in Userspace) merupakan antarmuka yang memungkinkan pembuatan filesystem tanpa hak akses superuser dan tanpa perlu menyentuh kode kernel. Dalam studi mereka, Rajgarhia dan Gehani menunjukkan bahwa FUSE telah menjadi populer karena menawarkan API sederhana dan dapat digunakan oleh pemrogram C maupun bahasa tingkat tinggi lainnya, tanpa perlu reboot atau patch kernel. Dalam kaitannya dengan penyembunyian file, teknik menyembunyikan file yang dinamai dengan awalan titik (.) adalah konvensi sistem Unix yang umum. Namun demikian, seperti dijelaskan dalam makalah oleh Jokay dan Kosdy (2013), penyembunyian di sistem file dapat lebih maju, termasuk teknik steganografi atau manipulasi metadata. Penelitian-penelitian lain juga membahas pendekatan seperti DupeFile untuk menyembunyikan file menggunakan nama yang mirip atau berulang, sebagai bentuk information hiding dalam sistem file.

Namun berbeda dengan pendekatan kompleks tersebut, tugas ini mengambil jalan yang sederhana namun efektif:    
Menyaring entri direktori agar tidak menampilkan file tersembunyi secara visual, dengan tetap menyimpan struktur dan isinya di belakang layar. Pendekatan ini serupa dengan desain awal `ConvertFS` (Baliga et al.), yang juga mengimplementasikan sistem file tersembunyi melalui lapisan logika di atas FUSE.

**Solusi**    
Solusi praktikum ini mengimplementasikan sebuah program bernama `cleanfs.c`, yang memanfaatkan FUSE untuk membuat filesystem sederhana yang berfungsi menyembunyikan semua file atau folder yang diawali dengan titik (.) dari tampilan direktori mount. Program ini disusun menggunakan bahasa C, sesuai dengan rekomendasi dari Rajgarhia dan Gehani bahwa FUSE menyediakan binding kuat untuk C, serta memungkinkan kontrol penuh atas operasi dasar seperti `readdir`, `getattr`, `read`, dan `open`.

Secara arsitektur, program berkerja dengan mengambil isi dari direktori `repo` sebagai sumber datanya. Direktori `mount_dir` adalh titik mounting tempat pengguna melihat tampilan file. fungsi `x_readdir` adalah komponen utama yang menyaring file yang dimulai dengan titik, sehingga tidak ditampilkan meskipun secara fisik tetap ada di disk. Fitur ini selaras dengan prinsip "user-level file filtering" yang disarankan oleh FiST (Zadok dan Nieh), di mana operasi file dapat diubah tanpa memengaruhi kernel atau struktur disk yang sebenarnya.

Selanjutnya, program ini juga dilengkapi dengan sistem pencatatan aktivitas ke dalam `log.txt`. Setiap file yang ditampilkan pertama kali, disembunyikan, atau dihapus akan dicatat dengan timestamp, jenis objek (file/folder), serta status (visible/hidden/deleted). Implementasi ini mencerminkan rekomendasi dari Baliga et al. dalam membangun sistem file berbasis lapisan pengguna dengan fitur audit log untuk keperluan forensik atau keamanan cyber. 

Adapun struktur direktori dna otutput setelah mounting akan menunjukkan bahwa `mount_dir` hanya menampilkan file non-dot(`file.txt, folder/`) sedangkan `.hidden_file.txt` atau `.hiden_folder/` tetap ada dalam repo namun tidak ditampilkan. Ini adalah bentuk *obfuscation* atau *view sanitization* yang banyak digunkakan dalam sistem yang menargetkan dalam privasi pengguna.

Proses pencatatan `delete` dilakukan dengan membnadingkan isi direktori pada iterasi sebelumnya dan iterasi saat ini, kemudian mencatat file yang sudah tidak ada lagi di `repo`. Ini merupakan bentuk *state tracking* sederhana yang mirip dengan yang digunakan dalam sistem distributed versioned file system seperti Ceph atau TierStore, walaupun dalam konteks yang jauh lebih ringan.

Secara keseluruhan, solusi ini menggabungkan konsep teori FUSE modern, teknik *hiding based name*, serta logika pengawasan file dalam satu kesatuan program C yang efisien dan mudah diuji.

**Video Menjalankan Program**    
[Akses Video dalam Assets](./assets/demo%20fp-sisop.mp4)

https://github.com/user-attachments/assets/0bc73f31-aa55-481e-9d87-510fa73b0cf5

## Daftar Pustaka
Rajgarhia, A., & Gehani, A. (2010). Performance and extension of user space file systems. In Proceedings of the 2010 ACM Symposium on Applied Computing (pp. 
206–213). ACM.

Srinivasan, A., Kolli, S., & Wu, J. (2013). Steganographic information hiding that exploits a novel file system vulnerability. International Journal of Security 
and Networks, 8(2), 82–93.

Jokay, M., & Kosdy, M. (2013). Steganographic file system based on JPEG files. Tatra Mountains Mathematical Publications, 57, 65–83.

Baliga, A., Kilian, J., & Iftode, L. (2007). A Web-based covert file system (Technical Report). Rutgers University, Department of Computer Science.

Zadok, E., & Nieh, J. (2000). FiST: A language for stackable file systems. In USENIX Annual Technical Conference (pp. 55–70).
