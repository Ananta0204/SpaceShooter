# 🚀 Space Shooter - C++ Ncurses Implementation

> *Tugas Akhir / Final Project Pengenalan Pemrograman C++*  
> Game konsol Space Shooter dengan sistem Wave progresif, Boss Fight (30 Detik), Top 5 Highscore, dan Audio.

---

## 📋 Daftar Isi
1. [Latar Belakang & Studi Kasus](#-latar-belakang--studi-kasus)
2. [Fitur Utama](#-fitur-utama)
3. [Struktur Kode & Arsitektur](#-struktur-kode--arsitektur)
4. [Prasyarat & Instalasi](#-prasyarat--instalasi)
5. [Cara Kompilasi & Menjalankan](#-cara-kompilasi--menjalankan)
6. [Kontrol Permainan](#-kontrol-permainan)
7. [Aset & File Eksternal](#-aset--file-eksternal)
8. [Credits](#-credits)
9. [Footage Game](#-footage-game)

---

## 📖 Latar Belakang & Studi Kasus

Proyek ini dikembangkan sebagai *Proyek Akhir (Final Project)* untuk mata kuliah Pengenalan Pemrograman di S1 Ilmu Komputer Universitas Lampung. Tujuan utamanya adalah mendemonstrasikan penerapan materi dasar pemrograman C++ dalam sebuah aplikasi interaktif, meliputi:

* **Modular Programming & Struct:** Pemisahan kode menjadi file header (`.h`) dan implementasi (`.cpp`), serta penggunaan `struct Objek` dan `struct Boss`.
* **Game Loop Logic:** Logika real-time rendering (~25 FPS), pergerakan 4 arah, deteksi tabrakan (*collision detection*), dan peningkatan gelombang (*wave*).
* **File Handling (I/O) & Sorting:** Membaca dan menulis data 5 skor tertinggi ke file eksternal `skor.txt` serta mengurutkannya menggunakan algoritma **Bubble Sort**.
* **External Library & Audio:** Penggunaan library *Ncurses* (`-lncursesw`) untuk antarmuka terminal berwarna & simbol Unicode (`♥`), serta `paplay` untuk memutar audio `.wav` secara non-blocking di WSL2 Ubuntu.

---

## ✨ Fitur Utama

* **Sistem 10 Nyawa (`♥`) & Invincibility:** Pemain memiliki 10 HP dengan efek kebal sesaat (pesawat berkedip) setelah terkena serangan agar nyawa tidak langsung habis dalam satu sentuhan.
* **Sistem Wave Progresif:**
  * **Wave 1:** Musuh biasa bergerak lurus (1 HP).
  * **Wave 2+:** Musuh lebih cepat (2 HP) dan mulai menembak balik (`*`).
  * **Wave 3, 6, 9, ... (Boss Wave):** Boss muncul di atas layar dengan bar HP tersendiri dan timer **30 detik** (menembak dari atas selama 30 detik pertama, lalu maju menyerbu ke arah pemain jika lewat 30 detik).
* **Persistent Top 5 Highscore:** Menyimpan 5 skor tertinggi secara otomatis ke `skor.txt` menggunakan *Bubble Sort* dan menandai skor terbaru (`<-- SKOR KAMU`).
* **Interactive Menu & Game Over Screen:** Menu utama rata tengah dengan animasi bintang bergerak, judul ASCII Art, navigasi keyboard, serta layar *Game Over* / *Stopped* dengan bingkai rapi.
* **Background Music & Sound Effects:** BGM terpisah untuk Menu (`bgm_menu.wav`), Gameplay (`bgm_game.wav`), Boss (`bgm_boss.wav`), Rekor Baru (`bgm_win.wav`), dan Kalah (`bgm_lose.wav`), ditambah efek suara tembakan, hit, dan ledakan.

---

## 🏗 Struktur Kode & Arsitektur

Proyek ini disusun secara modular ke dalam folder `include/`, `src/`, dan `assets/` agar mudah dikelola:

```text
📦 SpaceShooter
┣ 📂 assets
┃ ┣ 📜 bgm_menu.wav
┃ ┣ 📜 bgm_game.wav
┃ ┣ 📜 bgm_boss.wav
┃ ┣ 📜 bgm_win.wav
┃ ┣ 📜 bgm_lose.wav
┃ ┣ 📜 shoot.wav
┃ ┣ 📜 hit.wav
┃ ┣ 📜 boom.wav
┃ ┣ 📜 boss.wav
┃ ┗ 📜 win.wav
┣ 📂 include
┃ ┣ 📜 audio.h
┃ ┣ 📜 config.h
┃ ┣ 📜 logika.h
┃ ┣ 📜 skor.h
┃ ┗ 📜 tampilan.h
┣ 📂 src
┃ ┣ 📜 audio.cpp
┃ ┣ 📜 globals.cpp
┃ ┣ 📜 logika.cpp
┃ ┣ 📜 skor.cpp
┃ ┗ 📜 tampilan.cpp
┣ 📜 main.cpp
┣ 📜 Makefile
┣ 📜 skor.txt
┗ 📜 README.md
```

**Penjelasan Modul:**
* **`config.h` & `globals.cpp`:** Menyimpan konstanta ukuran layar, batas array objek, definisi `struct Objek` & `struct Boss`, serta variabel global game.
* **`logika.cpp/.h`:** Mengatur animasi bintang, pergerakan peluru, spawner & AI musuh, mekanik Boss 30 detik, serta deteksi tabrakan (*collision*).
* **`tampilan.cpp/.h`:** Menggambar menu awal, arena bermain, HUD (Wave, Skor, 10 Hati, Bar HP Boss), dan layar Game Over.
* **`skor.cpp/.h`:** Membaca dan menulis 5 skor teratas ke `skor.txt` serta mengurutkannya dengan *Bubble Sort*.
* **`audio.cpp/.h`:** Memutar efek suara (SFX) dan musik latar (BGM) di latar belakang menggunakan `paplay`.
* **`main.cpp`:** Mengatur inisialisasi `ncurses`, pasangan warna, dan menjalankan loop utama permainan.

---

## ⚙ Prasyarat & Instalasi

Pastikan lingkungan pengembangan (Ubuntu / WSL2) memiliki paket berikut:

* **Compiler C++ & Make:** `build-essential` (`g++` dan `make`).
* **Library Ncurses:** `libncurses-dev` (dukungan wide-character `-lncursesw`).
* **Audio Player (WSL/Linux):** `pulseaudio-utils` (`paplay`).

Untuk meng-install semuanya di Ubuntu/WSL, jalankan:

```bash
sudo apt update
sudo apt install build-essential libncurses-dev pulseaudio-utils
```

---

## 🛠 Cara Kompilasi & Menjalankan

Buka terminal Ubuntu di direktori proyek (disarankan tekan **F11** untuk layar penuh), lalu jalankan menggunakan `Makefile`:

```bash
make run
```

Atau kompilasi dan jalankan secara manual dengan `g++`:

```bash
g++ -Wall -Wextra -Iinclude main.cpp src/globals.cpp src/audio.cpp src/skor.cpp src/logika.cpp src/tampilan.cpp -o main -lncursesw
./main
```

---

## 🎮 Kontrol Permainan

| Kategori | Aksi           | Tombol Keyboard        |
| -------- | -------------- | ---------------------- |
| Pemain   | Gerak Atas     | `W` atau Panah Atas (↑)   |
|          | Gerak Bawah    | `S` atau Panah Bawah (↓)  |
|          | Gerak Kiri     | `A` atau Panah Kiri (←)   |
|          | Gerak Kanan    | `D` atau Panah Kanan (→)  |
|          | Tembak Laser   | `SPASI`                |
| Sistem   | Pilih Menu     | `Enter`                |
|          | Keluar Game    | `Q`                    |

---

## 📂 Aset & File Eksternal

Pastikan struktur berikut tersedia agar program berjalan optimal:

* **Folder `assets/`**
  * BGM: `bgm_menu.wav`, `bgm_game.wav`, `bgm_boss.wav`, `bgm_win.wav`, `bgm_lose.wav`
  * SFX: `shoot.wav`, `hit.wav`, `boom.wav`, `boss.wav`, `win.wav`
  * Semua file berformat `.wav` agar kompatibel dengan `paplay`.
* **File `skor.txt`**
  * Otomatis dibuat oleh program jika belum ada.
  * Format: 5 baris angka skor tertinggi (satu skor per baris).

---

## 👨‍💻 Credits

Universitas Lampung

| Nama                      | NPM     |
| ------------------------- | ------- |
| Ananta (isi nama lengkap) | Isi NPM |
| Nama Anggota 2            | Isi NPM |
| Nama Anggota 3            | Isi NPM |

Dibuat dengan ❤ menggunakan C++

---

## 🎬 Footage Game

<img width="800" height="450" alt="ezgif-4d53bfbb21a4d8f7" src="https://github.com/user-attachments/assets/5bc278c8-f88f-4e0b-ba9f-07323df2451f" />
