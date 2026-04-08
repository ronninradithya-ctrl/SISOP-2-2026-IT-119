#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CONTRACT_FILE "contract.txt"
#define LOG_FILE "work.log"

const char *PROMISE = "A promise to keep going, even when unseen.\n";

volatile sig_atomic_t running = 1;
char original_content[256];

void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

void write_log(const char *message) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log != NULL) {
        fprintf(log, "%s\n", message);
        fclose(log);
    }
}

void create_contract_file(const char *label) {
    FILE *fp = fopen(CONTRACT_FILE, "w");
    if (fp == NULL) return;

    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));

    fprintf(fp, "%s\n%s at: %s\n", PROMISE, label, timestamp);
    fclose(fp);

    // Simpan isi asli ke memory
    snprintf(original_content, sizeof(original_content),
             "%s\n%s at: %s\n", PROMISE, label, timestamp);
}

void restore_deleted_contract() {
    create_contract_file("restored");
}

void restore_modified_contract() {
    FILE *fp = fopen(CONTRACT_FILE, "w");
    if (fp == NULL) return;

    fputs(original_content, fp);
    fclose(fp);
}

int file_exists(const char *filename) {
    struct stat st;
    return stat(filename, &st) == 0;
}

int file_modified() {
    FILE *fp = fopen(CONTRACT_FILE, "r");
    if (fp == NULL) return 0;

    char buffer[512] = {0};
    fread(buffer, 1, sizeof(buffer) - 1, fp);
    fclose(fp);

    return strcmp(buffer, original_content) != 0;
}

void handle_signal(int sig) {
    running = 0;
}

void daemonize() {
    pid_t pid = fork();

    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // parent keluar

    if (setsid() < 0) exit(EXIT_FAILURE);

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    chdir(".");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main() {
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    srand(time(NULL));

    daemonize();

    // Buat file awal
    create_contract_file("created");

    // Pastikan work.log ada
    FILE *log = fopen(LOG_FILE, "a");
    if (log != NULL) fclose(log);

    const char *statuses[] = {"[awake]", "[drifting]", "[numbness]"};

    while (running) {
        // Kalau contract.txt hilang → restore
        if (!file_exists(CONTRACT_FILE)) {
            restore_deleted_contract();
        } else {
            // Kalau isinya diubah → log violation + restore
            if (file_modified()) {
                write_log("contract violated.");
                restore_modified_contract();
            }
        }

        // Tulis still working setiap 5 detik
        char logline[128];
        int random_index = rand() % 3;
        snprintf(logline, sizeof(logline), "still working... %s", statuses[random_index]);
        write_log(logline);

        sleep(5);
    }

    write_log("We really weren’t meant to be together");

    return 0;
}
