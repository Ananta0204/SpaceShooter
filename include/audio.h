#ifndef AUDIO_H
#define AUDIO_H

#include <string>

// Putar file efek suara dari folder assets di latar belakang.
void putarSuara(std::string namaFile);

// Hentikan musik latar yang sedang berjalan.
void hentiMusik();

// Putar musik latar berulang-ulang sampai dihentikan.
void mulaiMusik(std::string namaFile);

#endif