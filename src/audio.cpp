#include "audio.h"
#include <cstdlib>
#include <string>

using namespace std;

// Putar efek suara dari folder assets di latar belakang agar game tidak macet.
void putarSuara(string namaFile) {
    string cmd = "paplay --latency-msec=50 assets/" + namaFile + " >/dev/null 2>&1 &";
    system(cmd.c_str());
}

// Hentikan semua musik latar yang sedang berputar.
void hentiMusik() {
    system("pkill -f \"while paplay\" >/dev/null 2>&1");
    system("pkill -f \"assets/bgm_\" >/dev/null 2>&1");
}

// Putar musik latar berulang-ulang sampai dihentikan.
void mulaiMusik(string namaFile) {
    hentiMusik();
    string cmd = "sh -c 'while paplay --latency-msec=120 assets/" + namaFile + " >/dev/null 2>&1; do :; done' >/dev/null 2>&1 &";
    system(cmd.c_str());
}