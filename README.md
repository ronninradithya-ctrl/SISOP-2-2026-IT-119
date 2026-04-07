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

```
Nama,Pesanan,Jumlah,Status
Mat Jenin,Teh Tarik,3000,Belum Lunas
Siti,Roti Canai,5000,Lunas
Ah Tong,Nasi Lemak,7000,Belum Lunas
Upin,Milo Ais,2000,Lunas
Ipin,Kuih Cara,1500,Belum Lunas
Mail,Char Kway Teow,8000,Belum Lunas
Ehsan,Bandung,4000,Lunas
```

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
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// Fungsi helper untuk menjalankan child process dan menunggu hasilnya
// Mengembalikan exit status dari child process
int run_child(char *argv[]) {
    pid_t pid = fork();

    if (pid < 0) {
        // fork() gagal
        perror("fork gagal");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Ini adalah Child Process
        // execvp akan menggantikan proses ini dengan perintah yang diberikan
        execvp(argv[0], argv);
        // Jika execvp gagal (misal perintah tidak ditemukan), cetak error dan keluar
        perror("execvp gagal");
        exit(EXIT_FAILURE);
    }

    // Ini adalah Parent Process
    // Tunggu child selesai dan ambil status keluarnya
    int status;
    waitpid(pid, &status, 0);

    // Kembalikan exit code dari child (0 = sukses, non-zero = gagal)
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return -1; // Proses tidak berakhir normal
}

int main() {
    int result;

    printf("======================================\n");
    printf("  Kasir Muthu - Pengaman Buku Hutang  \n");
    printf("======================================\n\n");

    // -------------------------------------------------------
    // LANGKAH 1: Buat folder brankas_kedai
    // -------------------------------------------------------
    printf("[STEP 1] Upin memanggil Ipin untuk membuat folder brankas...\n");
    char *cmd1[] = {"mkdir", "brankas_kedai", NULL};
    result = run_child(cmd1);
    if (result != 0) {
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(EXIT_FAILURE);
    }
    printf("[OK] Folder brankas_kedai berhasil dibuat!\n\n");

    // -------------------------------------------------------
    // LANGKAH 2: Salin buku_hutang.csv ke dalam brankas
    // -------------------------------------------------------
    printf("[STEP 2] Upin memanggil Ipin untuk menyalin file CSV...\n");
    char *cmd2[] = {"cp", "buku_hutang.csv", "brankas_kedai/", NULL};
    result = run_child(cmd2);
    if (result != 0) {
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(EXIT_FAILURE);
    }
    printf("[OK] File buku_hutang.csv berhasil disalin ke brankas_kedai!\n\n");

    // -------------------------------------------------------
    // LANGKAH 3: Filter "Belum Lunas" ke daftar_penunggak.txt
    // Menggunakan sh -c karena ada karakter redirection (>)
    // yang tidak bisa langsung dipakai di exec tanpa shell
    // -------------------------------------------------------
    printf("[STEP 3] Upin memanggil Ipin untuk memfilter data penunggak...\n");
    char *cmd3[] = {
        "sh", "-c",
        "grep 'Belum Lunas' brankas_kedai/buku_hutang.csv > brankas_kedai/daftar_penunggak.txt",
        NULL
    };
    result = run_child(cmd3);
    if (result != 0) {
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(EXIT_FAILURE);
    }
    printf("[OK] File daftar_penunggak.txt berhasil dibuat!\n\n");

    // -------------------------------------------------------
    // LANGKAH 4: Kompres brankas_kedai menjadi rahasia_muthu.zip
    // -------------------------------------------------------
    printf("[STEP 4] Upin memanggil Ipin untuk mengompres brankas...\n");
    char *cmd4[] = {"zip", "-r", "rahasia_muthu.zip", "brankas_kedai", NULL};
    result = run_child(cmd4);
    if (result != 0) {
        printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
        exit(EXIT_FAILURE);
    }
    printf("[OK] brankas_kedai berhasil dikompres menjadi rahasia_muthu.zip!\n\n");

    // -------------------------------------------------------
    // SEMUA LANGKAH SUKSES
    // -------------------------------------------------------
    printf("======================================\n");
    printf("[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.\n");
    printf("======================================\n");

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

**Contoh output yang diharapkan:**

```
======================================
  Kasir Muthu - Pengaman Buku Hutang  
======================================

[STEP 1] Upin memanggil Ipin untuk membuat folder brankas...
[OK] Folder brankas_kedai berhasil dibuat!

[STEP 2] Upin memanggil Ipin untuk menyalin file CSV...
[OK] File buku_hutang.csv berhasil disalin ke brankas_kedai!

[STEP 3] Upin memanggil Ipin untuk memfilter data penunggak...
[OK] File daftar_penunggak.txt berhasil dibuat!

[STEP 4] Upin memanggil Ipin untuk mengompres brankas...
[OK] brankas_kedai berhasil dikompres menjadi rahasia_muthu.zip!

======================================
[INFO] Fuhh, selamat! Buku hutang dan daftar penagihan berhasil diamankan.
======================================
```

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

### Mengapa Langkah 3 Menggunakan `sh -c`?

Perintah `grep 'Belum Lunas' file.csv > output.txt` mengandung karakter **redirection** (`>`). Karakter ini adalah fitur dari **shell**, bukan dari program `grep` itu sendiri. 

Karena kita tidak boleh menggunakan `system()`, dan `exec()` hanya bisa menjalankan satu program tanpa interpretasi shell, maka solusinya adalah **memanggil shell (`sh`) sebagai program, lalu memberikan perintah lengkap sebagai argumen `-c`**. Dengan begitu, shell-lah yang menginterpretasikan redirection tersebut, bukan program `grep` secara langsung.

```c
// Cara yang benar: panggil sh sebagai "perantara"
char *cmd3[] = {
    "sh", "-c",
    "grep 'Belum Lunas' brankas_kedai/buku_hutang.csv > brankas_kedai/daftar_penunggak.txt",
    NULL
};
execvp(cmd3[0], cmd3);
```

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

## Konsep Sistem Operasi yang Digunakan

### `fork()`
Fungsi ini membuat **salinan persis** dari proses yang sedang berjalan. Setelah `fork()` dipanggil, ada dua proses yang identik:
- **Parent** mendapatkan nilai PID dari child (nilai > 0)
- **Child** mendapatkan nilai `0`
- Jika terjadi error, kedua proses mendapatkan nilai `-1`

### `exec()` (keluarga fungsi)
Fungsi ini **menggantikan** program yang sedang berjalan di dalam proses tersebut dengan program baru. Ada beberapa variannya:
- `execvp(file, argv[])` — Menerima array argumen, mencari program di `PATH`
- `execlp(file, arg0, arg1, ..., NULL)` — Menerima argumen satu per satu
- Setelah `exec()` berhasil, kode setelahnya **tidak akan pernah dieksekusi** karena program sudah digantikan

### `waitpid()`
Fungsi ini membuat **parent process menunggu** hingga child process tertentu selesai. Ini adalah kunci dari **Sequential Process** — parent tidak akan melanjutkan ke langkah berikutnya sebelum child selesai.

### Mengapa Tidak Boleh `system()`?
Fungsi `system()` sebenarnya memanggil shell secara internal (`/bin/sh -c "perintah"`). Ia memiliki risiko keamanan berupa **shell injection** dan dianggap sebagai cara "malas" yang menyembunyikan detail proses. Dengan `fork()` + `exec()`, kita memiliki **kontrol penuh** atas proses yang dibuat.

---

*README ini dibuat untuk mendokumentasikan tugas Sistem Operasi - Soal 1: Kasbon Warga Kampung Durian Runtuh.*
