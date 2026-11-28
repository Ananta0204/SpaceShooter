#ifndef CONFIG_H
#define CONFIG_H

#include <ctime>

// Ukuran layar terminal dan ukuran area bermain (diisi di main.cpp).
extern int LAYAR_W;
extern int LAYAR_H;
extern int LEBAR;
extern int TINGGI;

// Jarak geser area bermain dari tepi atas dan kiri layar.
const int OFFY = 3;
const int OFFX = 2;

// Batas maksimal objek, jumlah nyawa awal, dan jumlah skor tersimpan.
const int MAX_PELURU       = 25;
const int MAX_PELURU_MUSUH = 40;
const int MAX_MUSUH        = 20;
const int MAX_BINTANG      = 50;
const int NYAWA_AWAL       = 10;
const int JUMLAH_SKOR      = 5;

// Nama file teks untuk menyimpan papan skor.
extern const char FILE_SKOR[];

// Data untuk peluru, musuh biasa, dan bintang latar.
struct Objek {
    int  x, y;
    bool aktif;
    int  hp, arah, jeda;
};

// Data khusus untuk Boss (posisi, darah, arah gerak, dan timer maju 30 detik).
struct Boss {
    bool   aktif;
    int    x, y, hp, maxHp, arah, jedaTembak;
    time_t waktuMulai;
    bool   faseMaju;
};

// Deklarasi array dan variabel global (nilai aslinya disimpan di globals.cpp).
extern Objek peluru[MAX_PELURU];
extern Objek peluruMusuh[MAX_PELURU_MUSUH];
extern Objek musuh[MAX_MUSUH];
extern Objek bintang[MAX_BINTANG];
extern int   papanSkor[JUMLAH_SKOR];

extern int  px, py;
extern int  skor, nyawa, kebal, jedaTembak, frame, gelombang, targetSkorWave;
extern bool rekorBaru, berhenti;
extern Boss boss;

#endif