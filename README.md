## Kelompok C09

| Nama                        | NRP        |
| --------------------------- | ---------- |
| Hansen Chang                | 5027241028 |
| Ica Zika Hamizah            | 5027241058 |
| Muhammad Khosyi Syehab      | 5027241089 |
| Mohamad Arkan Zahir Asyafiq | 5027241120 |

## Deskripsi Soal (Testcase 29)

NO TITIK-TITIK
Buat sebuah FUSE file system yang berfungsi sebagai pembersih tampilan. Saat sebuah direktori di-mount, file system akan menyembunyikan semua file dan folder yang namanya diawali dengan titik (.).

### Catatan

Penggunaan:
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

Struktur repository:

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

## Pengerjaan

> Insert poin soal...

**Teori**

...

**Solusi**

...

> Insert poin soal...

**Teori**

...

**Solusi**

...

**Video Menjalankan Program**
...

## Daftar Pustaka

Sitasi 1
Sitasi 2
Sitasi 3
