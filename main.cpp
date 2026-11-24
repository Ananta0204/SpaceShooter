#include <ncurses.h>
#include <cstdlib>      // rand(), srand()
#include <ctime>        // time()
#include <fstream>      // ifstream, ofstream (textfile)
#include <iostream>     // cout
using namespace std;

/* ---------- KONSTANTA ---------- */
const int LEBAR = 40;           // lebar area bermain
const int TINGGI = 20;          // tinggi area bermain
const int OFFY = 2;             // geser area main ke bawah (ada baris skor di atas)
const int OFFX = 2;             // geser area main ke kanan
const int MAX_PELURU = 15;      // jumlah slot peluru
const int MAX_MUSUH = 15;       // jumlah slot musuh
const int NYAWA_AWAL = 10;
const int JUMLAH_SKOR = 5;      // papan skor menyimpan 5 skor teratas
const char FILE_SKOR[] = "skor.txt";
const int JUMLAH_GELOMBANG = 3;

/* ---------- STRUCT ---------- */
// Satu struct dipakai untuk peluru dan musuh:
// posisi (x, y) dan status aktif (true = sedang ada di layar)
struct Objek {
    int x;
    int y;
    bool aktif;
};

/* ---------- VARIABEL GLOBAL ---------- */
Objek peluru[MAX_PELURU];       // array peluru
Objek musuh[MAX_MUSUH];         // array musuh
int papanSkor[JUMLAH_SKOR];     // array 5 skor tertinggi
int px;                         // posisi x pemain (y selalu di baris paling bawah)
int skor;
int nyawa;
int frame;                      // penghitung putaran loop, untuk mengatur kecepatan musuh
bool rekorBaru;                 // true jika skor sesi ini jadi skor tertinggi
int gelombang;
int musuhDibuat;
bool menang;
bool berhenti;

/* =========================================================
 *  BAGIAN 1: TEXTFILE DAN SORTING (papan skor)
 * ========================================================= */

// Baca 5 skor dari file. Kalau file belum ada, semua skor = 0.
void bacaPapanSkor() {
    for (int i = 0; i < JUMLAH_SKOR; i++) {
        papanSkor[i] = 0;
    }

    ifstream file(FILE_SKOR);
    if (file.is_open()) {
        for (int i = 0; i < JUMLAH_SKOR; i++) {
            file >> papanSkor[i];
        }
        file.close();
    }
}

// Tulis 5 skor ke file, satu skor per baris.
void simpanPapanSkor() {
    ofstream file(FILE_SKOR);
    for (int i = 0; i < JUMLAH_SKOR; i++) {
        file << papanSkor[i] << "\n";
    }
    file.close();
}

// Bubble sort: urutkan array dari yang terbesar ke terkecil.
void urutkanTurun(int data[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (data[j] < data[j + 1]) {
                int temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
}

// Masukkan skor baru ke papan: gabungkan, urutkan, ambil 5 teratas, simpan.
void tambahSkor(int skorBaru) {
    int sementara[JUMLAH_SKOR + 1];     // 5 skor lama + 1 skor baru

    for (int i = 0; i < JUMLAH_SKOR; i++) {
        sementara[i] = papanSkor[i];
    }
    sementara[JUMLAH_SKOR] = skorBaru;

    urutkanTurun(sementara, JUMLAH_SKOR + 1);

    for (int i = 0; i < JUMLAH_SKOR; i++) {
        papanSkor[i] = sementara[i];
    }
    simpanPapanSkor();
}


/* =========================================================
 *  BAGIAN 2: LOGIKA GAME
 * ========================================================= */

// Kosongkan semua peluru dan musuh, kembalikan nilai awal.
void resetGame() {
    for (int i = 0; i < MAX_PELURU; i++) {
        peluru[i].aktif = false;
    }
    for (int i = 0; i < MAX_MUSUH; i++) {
        musuh[i].aktif = false;
    }
    px = LEBAR / 2;
    skor = 0;
    nyawa = NYAWA_AWAL;
    frame = 0;
    rekorBaru = false;
    gelombang = 1;
    musuhDibuat = 0;
    menang = false;
    berhenti = false;
}

// Cari slot peluru yang kosong, lalu aktifkan di depan pesawat.
void tembak() {
    for (int i = 0; i < MAX_PELURU; i++) {
        if (!peluru[i].aktif) {
            peluru[i].x = px;
            peluru[i].y = TINGGI - 2;
            peluru[i].aktif = true;
            return;                     // satu tembakan saja, lalu selesai
        }
    }
}

int jumlahMusuhGelombang() {
    // Gelombang 1: 8 musuh, gelombang 2: 12, gelombang 3: 16.
    return 8 + (gelombang - 1) * 4;
}

void munculkanMusuh() {
    if (musuhDibuat >= jumlahMusuhGelombang()) {
        return;
    }

    // Musuh muncul lebih sering di gelombang yang lebih tinggi.
    int intervalMuncul = 11 - (gelombang - 1) * 3;
    if (frame % intervalMuncul != 0) {
        return;
    }

    for (int i = 0; i < MAX_MUSUH; i++) {
        if (!musuh[i].aktif) {
            musuh[i].x = rand() % LEBAR;
            musuh[i].y = 0;
            musuh[i].aktif = true;
            musuhDibuat++;
            return;
        }
    }
}

void gerakMusuh() {
    // Musuh turun lebih cepat di setiap gelombang.
    int kecepatan = 8 - (gelombang - 1) * 2;
    if (kecepatan < 2) {
        kecepatan = 2;
    }

    if (frame % kecepatan != 0) {
        return;
    }

    for (int i = 0; i < MAX_MUSUH; i++) {
        if (musuh[i].aktif) {
            musuh[i].y++;
            if (musuh[i].y >= TINGGI) {
                musuh[i].aktif = false;
                nyawa--;
            }
        }
    }
    cekTabrakan();
}

// Cek peluru kena musuh, dan musuh menabrak pemain.
void cekTabrakan() {
    // Setiap peluru dibandingkan dengan setiap musuh
    for (int i = 0; i < MAX_PELURU; i++) {
        if (!peluru[i].aktif) {
            continue;                   // lewati peluru yang tidak aktif
        }
        for (int j = 0; j < MAX_MUSUH; j++) {
            if (musuh[j].aktif &&
                peluru[i].x == musuh[j].x &&
                peluru[i].y == musuh[j].y) {
                peluru[i].aktif = false;
                musuh[j].aktif = false;
                skor += 10;
                break;                  // peluru ini sudah habis, lanjut peluru berikutnya
            }
        }
    }

    // Musuh menabrak pemain
    for (int j = 0; j < MAX_MUSUH; j++) {
        if (musuh[j].aktif &&
            musuh[j].y == TINGGI - 1 &&
            musuh[j].x == px) {
            musuh[j].aktif = false;
            nyawa--;
            flash();                    // layar berkedip sebentar
        }
    }
}

// Peluru naik 1 baris tiap frame.
void gerakPeluru() {
    for (int i = 0; i < MAX_PELURU; i++) {
        if (peluru[i].aktif) {
            peluru[i].y--;
            if (peluru[i].y < 0) {      // keluar dari layar
                peluru[i].aktif = false;
            }
        }
    }
    cekTabrakan();
}

// Musuh turun 1 baris, tapi hanya setiap beberapa frame (lebih lambat dari peluru).
void gerakMusuh() {
    int kecepatan = 8 - skor / 100;     // makin kecil angkanya, makin cepat musuh turun
    if (kecepatan < 2) {
        kecepatan = 2;
    }

    if (frame % kecepatan != 0) {
        return;
    }

    for (int i = 0; i < MAX_MUSUH; i++) {
        if (musuh[i].aktif) {
            musuh[i].y++;
            if (musuh[i].y >= TINGGI) { // lolos sampai bawah
                musuh[i].aktif = false;
                nyawa--;
            }
        }
    }
    cekTabrakan();
}

bool adaMusuhAktif() {
    for (int i = 0; i < MAX_MUSUH; i++) {
        if (musuh[i].aktif) {
            return true;
        }
    }
    return false;
}

/* =========================================================
 *  BAGIAN 3: TAMPILAN (ncurses)
 * ========================================================= */

// Gambar garis pinggir area bermain.
void gambarBingkai() {
    attron(COLOR_PAIR(4));
    for (int i = -1; i <= LEBAR; i++) {
        mvaddch(OFFY - 1,      OFFX + i, '-');
        mvaddch(OFFY + TINGGI, OFFX + i, '-');
    }
    for (int i = 0; i < TINGGI; i++) {
        mvaddch(OFFY + i, OFFX - 1,     '|');
        mvaddch(OFFY + i, OFFX + LEBAR, '|');
    }
    attroff(COLOR_PAIR(4));
}

// Gambar satu frame penuh: hapus layar, gambar semua, tampilkan.
void gambarSemua() {
    erase();

    mvprintw(0, OFFX, "W:%d/%d HP:%d Skor:%d Rekor:%d",
         gelombang, JUMLAH_GELOMBANG, nyawa, skor, papanSkor[0]);

    // Musuh (merah)
    attron(COLOR_PAIR(1));
    for (int i = 0; i < MAX_MUSUH; i++) {
        if (musuh[i].aktif) {
            mvaddch(OFFY + musuh[i].y, OFFX + musuh[i].x, 'V');
        }
    }
    attroff(COLOR_PAIR(1));

    // Peluru (kuning)
    attron(COLOR_PAIR(2));
    for (int i = 0; i < MAX_PELURU; i++) {
        if (peluru[i].aktif) {
            mvaddch(OFFY + peluru[i].y, OFFX + peluru[i].x, '|');
        }
    }
    attroff(COLOR_PAIR(2));

    // Pemain (hijau)
    attron(COLOR_PAIR(3));
    mvaddch(OFFY + TINGGI - 1, OFFX + px, 'A');
    attroff(COLOR_PAIR(3));

    mvprintw(OFFY + TINGGI + 1, OFFX, "Panah/A,D: gerak  SPASI: tembak  Q: keluar");
    refresh();
}

// Tampilkan 5 skor tertinggi mulai dari baris tertentu.
void tampilPapanSkor(int baris) {
    mvprintw(baris, 10, "5 SKOR TERTINGGI");
    for (int i = 0; i < JUMLAH_SKOR; i++) {
        mvprintw(baris + 1 + i, 10, "%d. %d", i + 1, papanSkor[i]);
    }
}

// Menu awal. Return true kalau pemain mau main, false kalau keluar.
bool tampilMenu() {
    timeout(-1);                        // getch() menunggu sampai ada tombol
    erase();
    mvprintw(2, 10, "=== SPACE SHOOTER ===");
    mvprintw(4, 10, "Hancurkan musuh (V) sebelum sampai bawah!");
mvprintw(6, 10, "Kamu punya 10 HP dan harus melewati 3 gelombang.");
mvprintw(7, 10, "ENTER = main     Q = keluar");
tampilPapanSkor(9);
    refresh();

    while (true) {
        int tombol = getch();
        if (tombol == '\n' || tombol == KEY_ENTER) {
            return true;
        }
        if (tombol == 'q' || tombol == 'Q') {
            return false;
        }
    }
}

// Layar setelah nyawa habis.
void tampilGameOver() {
    timeout(-1);
    erase();
    if (menang) {
    mvprintw(2, 10, "=== KAMU MENANG! ===");
} else if (berhenti) {
    mvprintw(2, 10, "=== PERMAINAN DIHENTIKAN ===");
} else {
    mvprintw(2, 10, "=== GAME OVER ===");
}
}


/* =========================================================
 *  BAGIAN 4: GAME LOOP
 * ========================================================= */

void mainkan() {
    resetGame();
    timeout(50);

    while (nyawa > 0 && gelombang <= JUMLAH_GELOMBANG) {
        int tombol = getch();

        if ((tombol == KEY_LEFT || tombol == 'a') && px > 0) {
            px--;
        }
        if ((tombol == KEY_RIGHT || tombol == 'd') && px < LEBAR - 1) {
            px++;
        }
        if (tombol == ' ') {
            tembak();
        }
        if (tombol == 'q' || tombol == 'Q') {
            berhenti = true;
            break;
        }

        gerakPeluru();
        gerakMusuh();
        munculkanMusuh();

        // Lanjut ke gelombang berikutnya setelah semua musuh selesai.
        if (musuhDibuat >= jumlahMusuhGelombang() && !adaMusuhAktif()) {
            gelombang++;
            musuhDibuat = 0;
            frame = 0;
        }

        frame++;
        gambarSemua();
    }

    menang = (gelombang > JUMLAH_GELOMBANG && nyawa > 0);

    if (skor > papanSkor[0]) {
        rekorBaru = true;
    }
    if (skor > 0) {
        tambahSkor(skor);
    }
}



/* =========================================================
 *  MAIN
 * ========================================================= */

int main() {
    srand(time(0));                     // supaya angka acak berbeda tiap main

    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    // Pastikan terminal cukup besar
    int baris, kolom;
    getmaxyx(stdscr, baris, kolom);
    if (baris < TINGGI + 5 || kolom < LEBAR + 6) {
        endwin();
        cout << "Terminal terlalu kecil! Minimal " << LEBAR + 6 << " kolom x "
             << TINGGI + 5 << " baris." << endl;
        return 1;
    }

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_RED,    COLOR_BLACK);    // musuh
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);    // peluru
        init_pair(3, COLOR_GREEN,  COLOR_BLACK);    // pemain
        init_pair(4, COLOR_CYAN,   COLOR_BLACK);    // bingkai
    }

    bacaPapanSkor();




    while (tampilMenu()) {
        mainkan();
        tampilGameOver();
    }

    endwin();                           // wajib: kembalikan terminal ke normal
    return 0;
}