#include "tampilan.h"
#include "config.h"
#include "audio.h"
#include "logika.h"
#include <ncurses.h>
#include <ctime>
#include <string>

using namespace std;

// Tampilkan teks rata tengah secara horizontal pada baris y.
void gambarTeksTengah(int y, string teks, int warna) {
    int x = (LAYAR_W - (int)teks.length()) / 2;
    if (x < 0) x = 0;
    attron(COLOR_PAIR(warna) | A_BOLD);
    mvprintw(y, x, "%s", teks.c_str());
    attroff(COLOR_PAIR(warna) | A_BOLD);
}

// Gambar garis pinggir kotak area bermain.
void gambarBingkai() {
    attron(COLOR_PAIR(4) | A_BOLD);
    for (int i = 0; i < LEBAR; i++) {
        mvaddch(OFFY - 1,      OFFX + i, ACS_HLINE);
        mvaddch(OFFY + TINGGI, OFFX + i, ACS_HLINE);
    }
    for (int i = 0; i < TINGGI; i++) {
        mvaddch(OFFY + i, OFFX - 1,     ACS_VLINE);
        mvaddch(OFFY + i, OFFX + LEBAR, ACS_VLINE);
    }
    mvaddch(OFFY - 1,      OFFX - 1,     ACS_ULCORNER);
    mvaddch(OFFY - 1,      OFFX + LEBAR, ACS_URCORNER);
    mvaddch(OFFY + TINGGI, OFFX - 1,     ACS_LLCORNER);
    mvaddch(OFFY + TINGGI, OFFX + LEBAR, ACS_LRCORNER);
    attroff(COLOR_PAIR(4) | A_BOLD);
}

// Gambar satu frame penuh: bintang, info skor/HP, bingkai, boss, musuh, peluru, dan pesawat.
void gambarSemua() {
    erase();

    for (int i = 0; i < MAX_BINTANG; i++) {
        int w = (bintang[i].arah == 1) ? 4 : 6;
        attron(COLOR_PAIR(w));
        mvaddch(OFFY + bintang[i].y, OFFX + bintang[i].x, '.');
        attroff(COLOR_PAIR(w));
    }

    mvprintw(0, OFFX, "WAVE: %d   SKOR: %05d   REKOR: %05d   HP: ", gelombang, skor, papanSkor[0]);
    for (int i = 0; i < NYAWA_AWAL; i++) {
        if (i < nyawa) {
            attron(COLOR_PAIR(1) | A_BOLD); printw("♥"); attroff(COLOR_PAIR(1) | A_BOLD);
        } else {
            attron(COLOR_PAIR(1));          printw("♡"); attroff(COLOR_PAIR(1));
        }
    }
    printw(" (%d/10)", nyawa);

    if (boss.aktif) {
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(1, OFFX, "BOSS HP: [");
        int isi = (boss.hp * 25 + boss.maxHp - 1) / boss.maxHp;
        for (int i = 0; i < 25; i++) printw(i < isi ? "=" : "-");
        printw("] %d/%d", boss.hp, boss.maxHp);
        int sisa = 30 - (int)(time(0) - boss.waktuMulai);
        if (sisa > 0) printw("   |  BOSS MAJU DALAM: %d dtk", sisa);
        else          printw("   |  !! BOSS SEDANG MENYERBU !!");
        attroff(COLOR_PAIR(1) | A_BOLD);
    }

    gambarBingkai();

    if (boss.aktif) {
        string bentuk[4] = {
            "  /=======\\  ",
            " | (O) (O) | ",
            " |===\\_/===| ",
            "  \\V--V--V/  "
        };
        attron(COLOR_PAIR(5) | A_BOLD);
        for (int r = 0; r < 4; r++)
            if (boss.y + r >= 0 && boss.y + r < TINGGI)
                mvprintw(OFFY + boss.y + r, OFFX + boss.x, "%s", bentuk[r].c_str());
        attroff(COLOR_PAIR(5) | A_BOLD);
    }

    attron(COLOR_PAIR(1) | A_BOLD);
    for (int i = 0; i < MAX_MUSUH; i++) {
        if (!musuh[i].aktif) continue;
        if (musuh[i].y     >= 0 && musuh[i].y     < TINGGI) mvprintw(OFFY + musuh[i].y,     OFFX + musuh[i].x, "\\===/");
        if (musuh[i].y + 1 >= 0 && musuh[i].y + 1 < TINGGI) mvprintw(OFFY + musuh[i].y + 1, OFFX + musuh[i].x, " \\V/ ");
    }
    attroff(COLOR_PAIR(1) | A_BOLD);

    attron(COLOR_PAIR(2) | A_BOLD);
    for (int i = 0; i < MAX_PELURU; i++)
        if (peluru[i].aktif) mvaddch(OFFY + peluru[i].y, OFFX + peluru[i].x, '|');
    attroff(COLOR_PAIR(2) | A_BOLD);

    attron(COLOR_PAIR(1) | A_BOLD);
    for (int i = 0; i < MAX_PELURU_MUSUH; i++)
        if (peluruMusuh[i].aktif) mvaddch(OFFY + peluruMusuh[i].y, OFFX + peluruMusuh[i].x, '*');
    attroff(COLOR_PAIR(1) | A_BOLD);

    if (kebal == 0 || (kebal / 3) % 2 == 0) {
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(OFFY + py,     OFFX + px, "  ^  ");
        mvprintw(OFFY + py + 1, OFFX + px, " /|\\ ");
        mvprintw(OFFY + py + 2, OFFX + px, "/_|_\\");
        attroff(COLOR_PAIR(3) | A_BOLD);
    }

    mvprintw(OFFY + TINGGI + 1, OFFX, "WASD/Panah: Gerak | SPASI: Tembak | Q: Keluar");
    refresh();
}

// Tampilkan daftar 5 skor tertinggi mulai dari baris tertentu.
void tampilPapanSkor(int baris) {
    gambarTeksTengah(baris, "=== 5 SKOR TERTINGGI ===", 2);
    for (int i = 0; i < JUMLAH_SKOR; i++)
        gambarTeksTengah(baris + 2 + i, to_string(i + 1) + ". " + to_string(papanSkor[i]), 6);
}

// Menu awal. Return true kalau pemain pilih PLAY, false kalau keluar.
bool tampilMenu() {
    mulaiMusik("bgm_menu.wav");
    timeout(80);
    inisialisasiBintang(LAYAR_W, LAYAR_H);
    int pilihan = 0;

    string judul[5] = {
        " ____  ____   _    ____ _____   ____  _   _  ___   ___ _____ _____ ____  ",
        "/ ___||  _ \\ / \\  / ___| ____| / ___|| | | |/ _ \\ / _ \\_   _| ____|  _ \\ ",
        "\\___ \\| |_) / _ \\| |   |  _|   \\___ \\| |_| | | | | | | || | |  _| | |_) |",
        " ___) |  __/ ___ \\ |___| |___   ___) |  _  | |_| | |_| || | | |___|  _ < ",
        "|____/|_| /_/   \\_\\____|_____| |____/|_| |_|\\___/ \\___/ |_| |_____|_| \\_\\"
    };

    while (true) {
        frame++;
        gerakBintang(LAYAR_W, LAYAR_H);
        erase();

        for (int i = 0; i < MAX_BINTANG; i++) {
            attron(COLOR_PAIR(4));
            mvaddch(bintang[i].y, bintang[i].x, '.');
            attroff(COLOR_PAIR(4));
        }

        int y = (LAYAR_H - 19) / 2;
        if (y < 1) y = 1;

        int xJ = (LAYAR_W - 74) / 2;
        if (xJ >= 0) {
            attron(COLOR_PAIR(4) | A_BOLD);
            for (int r = 0; r < 5; r++) mvprintw(y + r, xJ, "%s", judul[r].c_str());
            attroff(COLOR_PAIR(4) | A_BOLD);
        } else {
            gambarTeksTengah(y + 2, "=== SPACE SHOOTER ===", 4);
        }

        tampilPapanSkor(y + 7);

        if (pilihan == 0) {
            gambarTeksTengah(y + 15, "[ > PLAY < ]", 2);
            gambarTeksTengah(y + 16, "[   EXIT   ]", 6);
        } else {
            gambarTeksTengah(y + 15, "[   PLAY   ]", 6);
            gambarTeksTengah(y + 16, "[ > EXIT < ]", 2);
        }

        gambarTeksTengah(y + 18, "W/S atau Panah: Pilih   |   ENTER: Konfirmasi", 6);
        refresh();

        int t = getch();
        if (t == KEY_UP || t == 'w' || t == 'W' || t == KEY_DOWN || t == 's' || t == 'S')
            pilihan = 1 - pilihan;
        else if (t == '\n' || t == '\r' || t == KEY_ENTER) return (pilihan == 0);
        else if (t == 'q'  || t == 'Q') return false;
    }
}

// Gambar kotak berbingkai dan kosongkan bagian dalamnya agar bintang tidak tembus.
static void gambarKotak(int x, int y, int w, int h, int warna) {
    for (int r = 1; r < h - 1; r++)
        for (int c = 1; c < w - 1; c++)
            mvaddch(y + r, x + c, ' ');

    attron(COLOR_PAIR(warna) | A_BOLD);
    for (int i = 1; i < w - 1; i++) {
        mvaddch(y,         x + i, ACS_HLINE);
        mvaddch(y + h - 1, x + i, ACS_HLINE);
    }
    for (int i = 1; i < h - 1; i++) {
        mvaddch(y + i, x,         ACS_VLINE);
        mvaddch(y + i, x + w - 1, ACS_VLINE);
    }
    mvaddch(y,         x,         ACS_ULCORNER);
    mvaddch(y,         x + w - 1, ACS_URCORNER);
    mvaddch(y + h - 1, x,         ACS_LLCORNER);
    mvaddch(y + h - 1, x + w - 1, ACS_LRCORNER);
    attroff(COLOR_PAIR(warna) | A_BOLD);
}

// Tampilkan layar akhir (judul kalah/berhenti, skor sesi ini, dan papan 5 skor tertinggi).
void tampilGameOver() {
    if (rekorBaru && skor > 0) mulaiMusik("bgm_win.wav");
    else                        mulaiMusik("bgm_lose.wav");
    timeout(80);
    inisialisasiBintang(LAYAR_W, LAYAR_H);

    string judulKalah[5] = {
        "  ____    _    __  __ _____    ___  __     _______ ____  ",
        " / ___|  / \\  |  \\/  | ____|  / _ \\ \\ \\   / / ____|  _ \\ ",
        "| |  _  / _ \\ | |\\/| |  _|   | | | | \\ \\ / /|  _| | |_) |",
        "| |_| |/ ___ \\| |  | | |___  | |_| |  \\ V / | |___|  _ < ",
        " \\____/_/   \\_\\_|  |_|_____|  \\___/    \\_/  |_____|_| \\_\\"
    };
    string judulBerhenti[5] = {
        " ____ _____ ___  ____  ____  _____ ____  ",
        "/ ___|_   _/ _ \\|  _ \\|  _ \\| ____|  _ \\ ",
        "\\___ \\ | || | | | |_) | |_) |  _| | | | |",
        " ___) || || |_| |  __/|  __/| |___| |_| |",
        "|____/ |_| \\___/|_|   |_|   |_____|____/ "
    };

    const int W = 46;
    const int H = 10;

    while (true) {
        frame++;
        gerakBintang(LAYAR_W, LAYAR_H);
        erase();

        for (int i = 0; i < MAX_BINTANG; i++) {
            int w = (bintang[i].arah == 1) ? 4 : 6;
            attron(COLOR_PAIR(w));
            mvaddch(bintang[i].y, bintang[i].x, '.');
            attroff(COLOR_PAIR(w));
        }

        int y = (LAYAR_H - 18) / 2;
        if (y < 1) y = 1;

        if (berhenti) {
            int xJ = (LAYAR_W - 41) / 2;
            if (xJ >= 0) {
                attron(COLOR_PAIR(2) | A_BOLD);
                for (int r = 0; r < 5; r++) mvprintw(y + r, xJ, "%s", judulBerhenti[r].c_str());
                attroff(COLOR_PAIR(2) | A_BOLD);
            } else {
                gambarTeksTengah(y + 2, "=== PERMAINAN DIHENTIKAN ===", 2);
            }
        } else {
            int xJ = (LAYAR_W - 57) / 2;
            if (xJ >= 0) {
                attron(COLOR_PAIR(1) | A_BOLD);
                for (int r = 0; r < 5; r++) mvprintw(y + r, xJ, "%s", judulKalah[r].c_str());
                attroff(COLOR_PAIR(1) | A_BOLD);
            } else {
                gambarTeksTengah(y + 2, "=== GAME OVER ===", 1);
            }
        }

        if (rekorBaru && skor > 0) {
            int wRekor = ((frame / 4) % 2 == 0) ? 2 : 3;
            gambarTeksTengah(y + 5, "*** SELAMAT! REKOR BARU! ***", wRekor);
        } else if (berhenti) {
            gambarTeksTengah(y + 5, "--- MISI DIHENTIKAN PEMAIN ---", 6);
        }

        int kotakX = (LAYAR_W - W) / 2;
        int kotakY = y + 6;
        if (kotakX < 1) kotakX = 1;

        gambarKotak(kotakX, kotakY, W, H, 4);

        // Garis pemisah antara skor sesi ini dan daftar 5 skor tertinggi.
        attron(COLOR_PAIR(4) | A_BOLD);
        mvaddch(kotakY + 2, kotakX, ACS_LTEE);
        for (int i = 1; i < W - 1; i++) mvaddch(kotakY + 2, kotakX + i, ACS_HLINE);
        mvaddch(kotakY + 2, kotakX + W - 1, ACS_RTEE);
        attroff(COLOR_PAIR(4) | A_BOLD);

        gambarTeksTengah(kotakY + 1, "SKOR: " + to_string(skor) + "   |   WAVE: " + to_string(gelombang), 6);
        gambarTeksTengah(kotakY + 3, "=== 5 SKOR TERTINGGI ===", 2);

        bool ditandai = false;
        for (int i = 0; i < JUMLAH_SKOR; i++) {
            string baris  = to_string(i + 1) + ". " + to_string(papanSkor[i]);
            int    warnaB = 6;
            if (!ditandai && skor > 0 && papanSkor[i] == skor) {
                baris  += "  <-- SKOR KAMU";
                warnaB  = 3;
                ditandai = true;
            }
            gambarTeksTengah(kotakY + 4 + i, baris, warnaB);
        }

        gambarTeksTengah(kotakY + H + 1, "Tekan ENTER untuk kembali ke Menu Utama...", 4);
        refresh();

        int t = getch();
        if (t == '\n' || t == '\r' || t == KEY_ENTER || t == ' ') break;
    }
}