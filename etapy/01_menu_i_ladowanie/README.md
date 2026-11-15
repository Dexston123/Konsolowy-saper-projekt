# Saper - etap 1

Menu, opis zasad Sapera, walidacja wpisywanego numeru i sztuczne ladowanie.
Ladowanie trwa okolo 7 sekund; napis SAPER miga co 700 ms.

## Kompilacja i uruchomienie

Potrzebny jest C++17. Polecenia wykonuj w folderze tego etapu.

```sh
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp src/Gra.cpp -o saper
./saper
```

Na Windows z MinGW-w64 zmien koncowke polecenia na `-o saper.exe`,
a program uruchom przez `.\saper.exe` w PowerShell.

Mozna tez uzyc CMake:

```sh
cmake -S . -B build
cmake --build build
```

Numer opcji zatwierdz Enterem. Wszystkie napisy konsoli sa w ASCII.
Kod znajduje sie w `src`, deklaracje klas w `include`.
