# SOAL 1 : Kasbon Warga Kampung Durian Runtuh

> **Mata Kuliah:** Sistem Operasi  
> **Author:** NINN  
> **Topik:** Sequential Process menggunakan `fork()`, `exec()`, dan `waitpid()`

---

## Deskripsi Soal

Siang itu Kedai Uncle Muthu sangat ramai. Namun tiba-tiba, komputer kasir jadul milik Uncle Muthu terserang **virus**! Di dalam komputer tersebut terdapat file penting bernama `buku_hutang.csv` yang berisi daftar utang seluruh warga Kampung Durian Runtuh.

Uncle Muthu panik! Ia harus segera mengamankan data tersebut sebelum virusnya menyebar lebih jauh. Selain itu, mumpung datanya sedang dibongkar, Uncle Muthu juga ingin memisahkan daftar orang-orang yang status utangnya **"Belum Lunas"** agar ia bisa menyuruh Abang Iz menagih mereka hari ini juga.

Uncle Muthu pun meminta tolong **Upin dan Ipin** untuk membuat sebuah program C bernama `kasir_muthu.c` guna mengamankan dan mengekstrak data tersebut secara otomatis dan berurutan, dengan ketentuan sebagai berikut:

1. **Sequential Process** — Program menjalankan proses secara berurutan menggunakan **Parent** dan **Child process**.
2. **Dilarang keras** menggunakan fungsi `system()`. Wajib menggunakan kombinasi `fork()`, `exec()`, dan `waitpid()`.
3. **Child Process 1** — Membuat folder `brankas_kedai` menggunakan perintah `mkdir`.
4. **Child Process 2** — Menyalin file `buku_hutang.csv` ke dalam folder `brankas_kedai` menggunakan perintah `cp`.
5. **Child Process 3** — Mencari semua data pelanggan berstatus **"Belum Lunas"** menggunakan `grep`, lalu menyimpannya ke file `daftar_penunggak.txt` di dalam brankas. Karena menggunakan redirection `>`, trik `sh -c` digunakan di dalam `execlp`.
6. **Child Process 4** — Mengompres folder `brankas_kedai` menjadi arsip `rahasia_muthu.zip` menggunakan perintah `zip -r`.
7. **Error Handling** — Setiap setelah Child selesai, Parent wajib mengecek status keberhasilan. Jika gagal, program **langsung berhenti** dan mencetak:
   ```
   [ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.
   ```
8. **Pesan Sukses** — Jika seluruh langkah berhasil, cetak:
   ```
   [INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.
   ```

---

## Struktur Direktori

Setelah program berhasil dijalankan, struktur folder akan terlihat seperti ini:

```text
soal_1/
├── buku_hutang.csv           # File CSV sumber utama (data utang warga)
├── kasir_muthu.c             # Source code program C
├── kasir_muthu               # Hasil kompilasi (file executable)
├── rahasia_muthu.zip         # Hasil akhir kompresi folder brankas
└── brankas_kedai/            # Folder brankas yang dibuat oleh program
    ├── buku_hutang.csv       # Salinan file CSV dari direktori asal
    └── daftar_penunggak.txt  # Hasil filter warga berstatus "Belum Lunas"
```

<img width="866" height="204" alt="Screenshot 2026-04-08 114527" src="https://github.com/user-attachments/assets/9439f3f8-b615-4cfe-90f2-334f352b6d19" />


---

## Langkah-Langkah Pembuatan di Ubuntu

### 1. Persiapan Lingkungan Kerja

Buat direktori kerja baru agar file tidak berantakan dengan file lain, lalu masuk ke dalamnya:

```bash
mkdir soal_1 && cd soal_1
```

---

### 2. Membuat File Data (`buku_hutang.csv`)

File CSV ini adalah data utang warga yang akan diproses oleh program. Buka editor `nano` dan masukkan data secara manual:

```bash
nano buku_hutang.csv
```

Isi file dengan format berikut (baris pertama adalah header):


<img width="1257" height="228" alt="Screenshot 2026-04-08 114541" src="https://github.com/user-attachments/assets/02315052-33d4-4273-aea1-8d4832975290" />


Simpan file: tekan `Ctrl+O` → `Enter` → `Ctrl+X` untuk keluar.

---

### 3. Membuat Kode Program (`kasir_muthu.c`)

Buka editor nano dan tulis program C-nya:

```bash
nano kasir_muthu.c
```

Berikut adalah **kode lengkap** program `kasir_muthu.c`:

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void cek_error(int status) {
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) != 0) {
            printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
            exit(1);
        }
    } else {
        printf("[ERROR] Aiyaa! Proses gagal.\n");
        exit(1);
    }
}

int main() {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        execlp("mkdir", "mkdir", "-p", "brankas_kedai", NULL);
        exit(1);
    } else {
        waitpid(pid, &status, 0);
        cek_error(status);
    }

    pid = fork();
    if (pid == 0) {
        execlp("cp", "cp", "buku_hutang.csv", "brankas_kedai/", NULL);
        exit(1);
    } else {
        waitpid(pid, &status, 0);
        cek_error(status);
    }

    pid = fork();
    if (pid == 0) {
        execlp("sh", "sh", "-c",
               "grep 'Belum Lunas' brankas_kedai/buku_hutang.csv > brankas_kedai/daftar_penunggak.txt",
               NULL);
        exit(1);
    } else {
        waitpid(pid, &status, 0);
        cek_error(status);
    }

    pid = fork();
    if (pid == 0) {
        execlp("zip", "zip", "-r", "rahasia_muthu.zip", "brankas_kedai", NULL);
        exit(1);
    } else {
        waitpid(pid, &status, 0);
        cek_error(status);
    }

    printf("[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.\n");

    return 0;
}
```

Simpan file: `Ctrl+O` → `Enter` → `Ctrl+X`.

---

### 4. Kompilasi Program

Ubah source code C menjadi file binary yang bisa dieksekusi oleh sistem Ubuntu:

```bash
gcc kasir_muthu.c -o kasir_muthu
```

Jika tidak ada pesan error yang muncul, berarti kompilasi **berhasil** dan file `kasir_muthu` telah terbentuk.

> **Catatan:** Pastikan compiler `gcc` sudah terinstal. Jika belum, jalankan `sudo apt install gcc` terlebih dahulu.

---

### 5. Eksekusi Program

Jalankan program yang telah dikompilasi:

```bash
./kasir_muthu
```

**Contoh output:**

<img width="939" height="118" alt="Screenshot 2026-04-08 111819" src="https://github.com/user-attachments/assets/dd75b33f-7196-4888-84f7-0461f2e0d8ad" />


---

## Cara Kerja Program (Penjelasan Detail)

### Alur Sequential Process

Program ini mengimplementasikan pola **Parent-Child Process** yang berjalan secara **sekuensial** (satu per satu, berurutan). Inti dari mekanisme ini adalah:

- **`fork()`** — Menciptakan child process baru (salinan dari parent).
- **`execvp()`** / **`execlp()`** — Menggantikan program di dalam child process dengan perintah yang diinginkan (seperti `mkdir`, `cp`, `grep`, `zip`).
- **`waitpid()`** — Membuat parent menunggu hingga child selesai sebelum melanjutkan ke langkah berikutnya.

Berikut adalah visualisasi alur prosesnya:

```
PARENT (Upin)
    │
    ├── fork() ──► CHILD 1 (Ipin) ──► exec("mkdir brankas_kedai")
    │                │
    │            waitpid() ◄── Child 1 selesai [cek status]
    │
    ├── fork() ──► CHILD 2 (Ipin) ──► exec("cp buku_hutang.csv brankas_kedai/")
    │                │
    │            waitpid() ◄── Child 2 selesai [cek status]
    │
    ├── fork() ──► CHILD 3 (Ipin) ──► exec("sh -c grep 'Belum Lunas' ... > daftar_penunggak.txt")
    │                │
    │            waitpid() ◄── Child 3 selesai [cek status]
    │
    ├── fork() ──► CHILD 4 (Ipin) ──► exec("zip -r rahasia_muthu.zip brankas_kedai")
    │                │
    │            waitpid() ◄── Child 4 selesai [cek status]
    │
    └── [INFO] Selesai! Semua langkah sukses.
```

### Penjelasan Per Langkah

| Langkah | Child | Perintah yang Dijalankan | Keterangan |
|---------|-------|--------------------------|------------|
| 1 | Child 1 | `mkdir brankas_kedai` | Membuat folder sebagai "brankas" penyimpanan |
| 2 | Child 2 | `cp buku_hutang.csv brankas_kedai/` | Menyalin file CSV ke dalam brankas |
| 3 | Child 3 | `sh -c "grep 'Belum Lunas' ... > ..."` | Filter data, pakai `sh -c` karena ada `>` |
| 4 | Child 4 | `zip -r rahasia_muthu.zip brankas_kedai` | Mengompres seluruh isi brankas |

### Mekanisme Error Handling

Setiap kali Parent memanggil `waitpid()`, ia akan mendapatkan **exit status** dari child. Dalam Unix/Linux, program yang berhasil mengembalikan nilai `0`, sedangkan yang gagal mengembalikan nilai selain `0`.

```c
// Setelah waitpid() selesai
if (WIFEXITED(status)) {
    return WEXITSTATUS(status); // Ambil exit code dari child
}
```

Jika child mengembalikan nilai selain `0`, program langsung menghentikan eksekusi dan mencetak pesan error:

```
[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.
```

Hal ini mencegah program melanjutkan ke langkah berikutnya ketika ada yang gagal di tengah jalan, misalnya jika file `buku_hutang.csv` tidak ditemukan.

---

**#Soal 2**

