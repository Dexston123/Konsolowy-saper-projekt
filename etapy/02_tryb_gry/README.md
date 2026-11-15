# Saper - etap 2

Menu i ladowanie oraz wybor trybu klasycznego lub nieskonczonego.
Po wyborze wyswietlane sa zasady danego trybu. Klasy trybow korzystaja
z dziedziczenia oraz metod wirtualnych wywolywanych przez referencje do bazy.

## Kompilacja i uruchomienie

Potrzebny jest C++17. Polecenia wykonuj w folderze tego etapu.

```sh
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp src/Gra.cpp src/TrybGry.cpp -o saper
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
