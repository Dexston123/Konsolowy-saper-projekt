# Saper - etap 3

Menu, ladowanie, wybor trybu i pieciu poziomow trudnosci oraz ustawienia.
Mozna zmienic kolorystyke, domyslny poziom, animacje i przywrocic wartosci
domyslne. Ekran trybu pokazuje wybrany rozmiar i liczbe min.
Ustawienia sa zapisywane do dane/ustawienia.txt przy wyjsciu i wczytywane
po ponownym uruchomieniu. Folder danych zalezy od katalogu roboczego.

## Kompilacja i uruchomienie

Potrzebny jest C++17. Polecenia wykonuj w folderze tego etapu.

```sh
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp src/Gra.cpp src/TrybGry.cpp src/Ustawienia.cpp -o saper
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
