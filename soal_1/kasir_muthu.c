#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void cek_error(int status) {
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) != 0) {
            printf("[ERROR] Aiyaa! Proses gagal, file atau folder tidak ditemukan.\n");
            exit(EXIT_FAILURE);
        }
    }
}

int main() {
    pid_t pid;
    int status;

    // Langkah 2: Buat folder brankas_kedai
    pid = fork();
    if (pid == 0) {
        execlp("mkdir", "mkdir", "brankas_kedai", NULL);
        exit(1);
    } else {
        waitpid(pid, &status, 0);
        cek_error(status);
    }

    // Langkah 3: Copy buku_hutang.csv ke dalam brankas_kedai
    pid = fork();
    if (pid == 0) {
        execlp("cp", "cp", "buku_hutang.csv", "brankas_kedai/", NULL);
        exit(1);
    } else {
        waitpid(pid, &status, 0);
        cek_error(status);
    }

    // Langkah 4: Filter "Belum Lunas" -> daftar_penunggak.txt
    pid = fork();
    if (pid == 0) {
        execlp("sh", "sh", "-c", "grep 'Belum Lunas' brankas_kedai/buku_hutang.csv > brankas_kedai/daftar_penunggak.txt", NULL);
        exit(1);
    } else {
        waitpid(pid, &status, 0);
        cek_error(status);
    }

    // Langkah 5: Zip folder menjadi rahasia_muthu.zip
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
