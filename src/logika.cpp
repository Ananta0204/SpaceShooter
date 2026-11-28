#include "logika.h"
#include "config.h"
#include "audio.h"
#include <ncurses.h>
#include <cstdlib>
#include <ctime>

// Acak posisi dan kecepatan awal bintang di latar belakang.
void inisialisasiBintang(int w, int h) {
    for (int i = 0; i < MAX_BINTANG; i++) {
        bintang[i].x    = rand() % w;
        bintang[i].y    = rand() % h;
        bintang[i].arah = (rand() % 2) + 1;
        bintang[i].aktif = true;
    }
}

// Turunkan posisi bintang tiap beberapa frame agar terlihat bergerak.
void gerakBintang(int w, int h) {
    for (int i = 0; i < MAX_BINTANG; i++) {
        int jeda = (bintang[i].arah == 1) ? 2 : 4;
        if (frame % jeda == 0) {
            bintang[i].y++;
            if (bintang[i].y >= h) {
                bintang[i].y = 0;
                bintang[i].x = rand() % w;
            }
        }
    }
}

// Kosongkan peluru dan musuh, lalu kembalikan skor, nyawa, dan posisi ke awal.
void resetGame() {
    for (int i = 0; i < MAX_PELURU; i++)       peluru[i].aktif      = false;
    for (int i = 0; i < MAX_PELURU_MUSUH; i++) peluruMusuh[i].aktif = false;
    for (int i = 0; i < MAX_MUSUH; i++)        musuh[i].aktif       = false;

    px = LEBAR / 2 - 2;
    py = TINGGI - 4;
    skor = nyawa = 0;
    nyawa         = NYAWA_AWAL;
    kebal         = 0;
    jedaTembak    = 0;
    frame         = 0;
    rekorBaru     = false;
    gelombang     = 1;
    targetSkorWave = 100;
    berhenti      = false;
    boss.aktif    = false;

    inisialisasiBintang(LEBAR, TINGGI);
}

// Cari slot peluru yang kosong, lalu munculkan di depan moncong pesawat.
void tembak() {
    if (jedaTembak > 0) return;
    for (int i = 0; i < MAX_PELURU; i++) {
        if (!peluru[i].aktif) {
            peluru[i].x    = px + 2;
            peluru[i].y    = py - 1;
            peluru[i].aktif = true;
            jedaTembak     = 3;
            putarSuara("shoot.wav");
            return;
        }
    }
}

// Cari slot peluru musuh yang kosong, lalu tembakkan ke bawah.
void musuhTembak(int x, int y, int arahX) {
    for (int i = 0; i < MAX_PELURU_MUSUH; i++) {
        if (!peluruMusuh[i].aktif) {
            peluruMusuh[i].x    = x;
            peluruMusuh[i].y    = y;
            peluruMusuh[i].arah = arahX;
            peluruMusuh[i].aktif = true;
            return;
        }
    }
}

// Hapus musuh biasa yang tersisa, lalu munculkan Boss di atas dan ganti musik.
void buatBoss() {
    for (int i = 0; i < MAX_MUSUH; i++)        musuh[i].aktif       = false;
    for (int i = 0; i < MAX_PELURU_MUSUH; i++) peluruMusuh[i].aktif = false;

    boss.aktif      = true;
    boss.x          = (LEBAR - 13) / 2;
    boss.y          = 1;
    boss.maxHp      = 35 + gelombang * 10;
    boss.hp         = boss.maxHp;
    boss.arah       = 1;
    boss.jedaTembak = 25;
    boss.waktuMulai = time(0);
    boss.faseMaju   = false;
    putarSuara("boss.wav");
    mulaiMusik("bgm_boss.wav");
}

// Kurangi nyawa pemain dan aktifkan kebal sementara agar tidak langsung habis.
void kenaHit(int damage) {
    if (kebal > 0) return;
    nyawa -= damage;
    if (nyawa < 0) nyawa = 0;
    kebal = 25;
    putarSuara("hit.wav");
    flash();
}

// Cek semua tabrakan: peluru kena musuh/boss, peluru musuh, dan badan pemain.
void cekTabrakan() {
    for (int i = 0; i < MAX_PELURU; i++) {
        if (!peluru[i].aktif) continue;

        for (int j = 0; j < MAX_MUSUH; j++) {
            if (musuh[j].aktif &&
                peluru[i].x >= musuh[j].x && peluru[i].x < musuh[j].x + 5 &&
                peluru[i].y >= musuh[j].y && peluru[i].y < musuh[j].y + 2) {
                peluru[i].aktif = false;
                musuh[j].hp--;
                if (musuh[j].hp <= 0) {
                    musuh[j].aktif = false;
                    skor += 10;
                    putarSuara("boom.wav");
                } else {
                    putarSuara("hit.wav");
                }
                break;
            }
        }

        if (peluru[i].aktif && boss.aktif &&
            peluru[i].x >= boss.x && peluru[i].x < boss.x + 13 &&
            peluru[i].y >= boss.y && peluru[i].y < boss.y + 4) {
            peluru[i].aktif = false;
            boss.hp--;
            putarSuara("hit.wav");
            if (boss.hp <= 0) {
                boss.aktif      = false;
                skor           += 150;
                gelombang++;
                targetSkorWave  = skor + 100;
                putarSuara("boom.wav");
                putarSuara("win.wav");
                mulaiMusik("bgm_game.wav");
            }
        }
    }

    for (int i = 0; i < MAX_PELURU_MUSUH; i++) {
        if (peluruMusuh[i].aktif &&
            peluruMusuh[i].x >= px && peluruMusuh[i].x < px + 5 &&
            peluruMusuh[i].y >= py && peluruMusuh[i].y < py + 3) {
            peluruMusuh[i].aktif = false;
            kenaHit(1);
        }
    }

    for (int j = 0; j < MAX_MUSUH; j++) {
        if (musuh[j].aktif &&
            musuh[j].x < px + 5 && musuh[j].x + 5 > px &&
            musuh[j].y < py + 3 && musuh[j].y + 2 > py) {
            musuh[j].aktif = false;
            kenaHit(1);
        }
    }

    if (boss.aktif &&
        boss.x < px + 5 && boss.x + 13 > px &&
        boss.y < py + 3 && boss.y + 4  > py) {
        kenaHit(2);
        boss.y = (boss.y > 6) ? boss.y - 5 : 1;
    }
}

// Munculkan musuh biasa di posisi x acak sesuai interval gelombang.
void munculkanMusuh() {
    if (boss.aktif) return;

    int interval = 18 - (gelombang - 1) * 3;
    if (interval < 6) interval = 6;
    if (frame % interval != 0) return;

    for (int i = 0; i < MAX_MUSUH; i++) {
        if (!musuh[i].aktif) {
            musuh[i].hp    = (gelombang == 1) ? 1 : 2;
            musuh[i].x     = rand() % (LEBAR - 5);
            musuh[i].y     = 0;
            musuh[i].jeda  = 12 + rand() % 15;
            musuh[i].aktif = true;
            return;
        }
    }
}

// Gerakkan peluru pemain ke atas, dan peluru musuh ke bawah tiap 2 frame.
void gerakPeluru() {
    for (int i = 0; i < MAX_PELURU; i++) {
        if (peluru[i].aktif) {
            peluru[i].y--;
            if (peluru[i].y < 0) peluru[i].aktif = false;
        }
    }

    if (frame % 2 == 0) {
        for (int i = 0; i < MAX_PELURU_MUSUH; i++) {
            if (peluruMusuh[i].aktif) {
                peluruMusuh[i].y++;
                peluruMusuh[i].x += peluruMusuh[i].arah;
                if (peluruMusuh[i].y >= TINGGI || peluruMusuh[i].x < 0 || peluruMusuh[i].x >= LEBAR)
                    peluruMusuh[i].aktif = false;
            }
        }
    }
    cekTabrakan();
}

// Turunkan musuh tiap beberapa frame, dan tembak balik mulai gelombang 2.
void gerakMusuh() {
    int kecepatan = 6 - (gelombang - 1);
    if (kecepatan < 2) kecepatan = 2;

    if (frame % kecepatan == 0) {
        for (int i = 0; i < MAX_MUSUH; i++) {
            if (!musuh[i].aktif) continue;
            musuh[i].y++;

            if (gelombang >= 2) {
                musuh[i].jeda--;
                if (musuh[i].jeda <= 0) {
                    musuhTembak(musuh[i].x + 2, musuh[i].y + 2, 0);
                    musuh[i].jeda = 10 + rand() % 10;
                }
            }
            if (musuh[i].y >= TINGGI) musuh[i].aktif = false;
        }
    }
    cekTabrakan();
}

// Atur gerak Boss kiri-kanan, tembakan, dan mulai maju ke bawah lewat 30 detik.
void gerakBoss() {
    if (!boss.aktif) return;

    if ((int)(time(0) - boss.waktuMulai) >= 30 && !boss.faseMaju) {
        boss.faseMaju = true;
        putarSuara("boss.wav");
    }

    if (frame % 2 == 0) {
        boss.x += boss.arah;
        if (boss.x <= 1)            { boss.x = 1;          boss.arah =  1; }
        if (boss.x + 13 >= LEBAR-1) { boss.x = LEBAR - 14; boss.arah = -1; }
    }

    if (boss.faseMaju && frame % 8 == 0) {
        if (boss.y + 4 < TINGGI - 1) boss.y++;
        if      (boss.x + 6 < px + 2) boss.x++;
        else if (boss.x + 6 > px + 2) boss.x--;
    }

    boss.jedaTembak--;
    if (boss.jedaTembak <= 0) {
        int t = boss.x + 6;
        if (!boss.faseMaju) {
            musuhTembak(t - 3, boss.y + 4, 0);
            musuhTembak(t + 3, boss.y + 4, 0);
            boss.jedaTembak = 20;
        } else {
            musuhTembak(t,     boss.y + 4,  0);
            musuhTembak(t - 2, boss.y + 4, -1);
            musuhTembak(t + 2, boss.y + 4,  1);
            boss.jedaTembak = 15;
        }
    }
    cekTabrakan();
}