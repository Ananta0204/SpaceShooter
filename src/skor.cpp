#include "skor.h"
#include "config.h"
#include <fstream>

using namespace std;

// Baca 5 skor dari file. Kalau file belum ada, semua skor = 0.
void bacaPapanSkor() {
    for (int i = 0; i < JUMLAH_SKOR; i++) papanSkor[i] = 0;

    ifstream file(FILE_SKOR);
    if (file.is_open()) {
        for (int i = 0; i < JUMLAH_SKOR; i++)
            if (!(file >> papanSkor[i])) break;
        file.close();
    }
}

// Tulis 5 skor ke file, satu skor per baris.
void simpanPapanSkor() {
    ofstream file(FILE_SKOR);
    for (int i = 0; i < JUMLAH_SKOR; i++)
        file << papanSkor[i] << "\n";
    file.close();
}

// Bubble sort: urutkan array dari yang terbesar ke terkecil.
void urutkanTurun(int data[], int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - 1 - i; j++)
            if (data[j] < data[j + 1]) {
                int tmp = data[j]; data[j] = data[j + 1]; data[j + 1] = tmp;
            }
}

// Gabungkan skor baru dengan 5 skor lama, urutkan, ambil 5 teratas, lalu simpan.
void tambahSkor(int skorBaru) {
    int buf[JUMLAH_SKOR + 1];
    for (int i = 0; i < JUMLAH_SKOR; i++) buf[i] = papanSkor[i];
    buf[JUMLAH_SKOR] = skorBaru;
    urutkanTurun(buf, JUMLAH_SKOR + 1);
    for (int i = 0; i < JUMLAH_SKOR; i++) papanSkor[i] = buf[i];
    simpanPapanSkor();
}