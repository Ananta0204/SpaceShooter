#include <ncurses.h>
#include <locale.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "config.h"
#include "audio.h"
#include "skor.h"
#include "logika.h"
#include "tampilan.h"

using namespace std;

// Jalankan satu sesi permainan dari awal sampai nyawa habis atau pemain keluar.
void mainkan() {
    resetGame();
    mulaiMusik("bgm_game.wav");
    timeout(40);

    while (nyawa > 0 && !berhenti) {
        int t = getch();

        if ((t == KEY_LEFT  || t == 'a' || t == 'A') && px > 0)         px -= 2;
        if ((t == KEY_RIGHT || t == 'd' || t == 'D') && px + 5 < LEBAR) px += 2;
        if ((t == KEY_UP    || t == 'w' || t == 'W') && py > 1)         py--;
        if ((t == KEY_DOWN  || t == 's' || t == 'S') && py + 3 < TINGGI) py++;
        if (px < 0)        px = 0;
        if (px + 5 > LEBAR) px = LEBAR - 5;

        if (t == ' ')              tembak();
        if (t == 'q' || t == 'Q') { berhenti = true; break; }

        // Naikkan gelombang atau munculkan boss setiap skor target tercapai.
        if (!boss.aktif && skor >= targetSkorWave) {
            gelombang++;
            if (gelombang % 3 == 0) {
                buatBoss();
            } else {
                targetSkorWave = (gelombang == 2) ? 200 : skor + 100;
            }
        }

        if (jedaTembak > 0) jedaTembak--;
        if (kebal > 0)      kebal--;

        gerakBintang(LEBAR, TINGGI);
        gerakPeluru();
        gerakMusuh();
        gerakBoss();
        munculkanMusuh();

        frame++;
        gambarSemua();
    }

    if (skor > papanSkor[0]) rekorBaru = true;
    if (skor > 0)            tambahSkor(skor);
}

// Fungsi utama: atur layar ncurses, warna, baca skor dari file, lalu jalankan menu.
int main() {
    if (!setlocale(LC_ALL, "C.UTF-8")) setlocale(LC_ALL, "");
    srand(time(0));
    cout << "\033[9;1t" << flush;

    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, LAYAR_H, LAYAR_W);

    if (LAYAR_H < 24 || LAYAR_W < 78) {
        endwin();
        cout << "Terminal terlalu kecil! Perbesar/F11 dulu (minimal 78x24).\n";
        return 1;
    }

    LEBAR  = LAYAR_W - 4;
    TINGGI = LAYAR_H - 5;

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_RED,     COLOR_BLACK);
        init_pair(2, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(3, COLOR_GREEN,   COLOR_BLACK);
        init_pair(4, COLOR_CYAN,    COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6, COLOR_WHITE,   COLOR_BLACK);
    }

    bacaPapanSkor();

    while (tampilMenu()) {
        mainkan();
        tampilGameOver();
    }

    hentiMusik();
    endwin();
    return 0;
}