# Pengaturan compiler, flag warning, folder include, dan library ncursesw.
CXX = g++
CXXFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lncursesw

# Daftar semua file .cpp yang digabungkan dan nama file hasil compile.
SRCS = main.cpp src/globals.cpp src/audio.cpp src/skor.cpp src/logika.cpp src/tampilan.cpp
TARGET = main

# Target bawaan saat mengetik "make": compile semua file menjadi ./main.
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

# Jalankan game langsung dengan perintah "make run".
run: $(TARGET)
	./$(TARGET)

# Hapus file hasil compile dengan perintah "make clean".
clean:
	rm -f $(TARGET)

.PHONY: all run clean