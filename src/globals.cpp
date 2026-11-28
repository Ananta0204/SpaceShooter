#include "config.h"

// Ukuran layar terminal dan ukuran area bermain.
int LAYAR_W = 80;
int LAYAR_H = 24;
int LEBAR   = 76;
int TINGGI  = 19;

// Nama file teks untuk menyimpan papan skor.
const char FILE_SKOR[] = "skor.txt";

// Array objek di layar dan daftar 5 skor tertinggi.
Objek peluru[MAX_PELURU];
Objek peluruMusuh[MAX_PELURU_MUSUH];
Objek musuh[MAX_MUSUH];
Objek bintang[MAX_BINTANG];
int   papanSkor[JUMLAH_SKOR];

// Posisi pemain dan status permainan yang dipakai di semua file.
int  px, py;
int  skor, nyawa, kebal, jedaTembak, frame, gelombang, targetSkorWave;
bool rekorBaru, berhenti;
Boss boss;