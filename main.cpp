#include <ncurses.h>

int main() {
    initscr();            // Memulai mode ncurses
    printw("Halo! ncurses berhasil jalan di VS Code!");
    refresh();            // Menampilkan tulisan ke layar
    getch();              // Menunggu user menekan sembarang tombol
    endwin();             // Keluar dari mode ncurses
    return 0;
}