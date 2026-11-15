# Konsolowy Saper

Projekt w C++17 na podstawie instrukcji z pliku `Projekt zaliczeniowy C++.pdf`.
Obecna wersja obejmuje menu i podglad generatora plansz. Wlasciwa rozgrywka
bedzie kolejnym etapem.

## Co juz dziala

- Sztuczne ladowanie przez okolo 7 sekund: piec zapisanych na sztywno etapow
  paska i migajacy napis SAPER (700 ms widoczny, 700 ms ukryty).
  Pasek nie mierzy zadnego postepu. Miganie dziala w terminalu z obsluga ANSI.
- Menu: Graj, Tablica Wynikow, Ustawienia, Wyjdz.
- Dwa tryby: Klasyczny (docelowo jedna gra na punkty) oraz Nieskonczony
  (docelowo kolejne plansze do przegranej).
- Podglad wszystkich min i liczby min na osmiu sasiednich polach.
- Ponowne losowanie planszy w obu trybach, bez odkrywania pol i flagowania.
- Cztery kolorystyki, piec poziomow trudnosci, przelaczanie animacji i reset ustawien.
- Kolorowe cyfry, bordowe miny, tlo konsoli i ramki w wybranym motywie.
- Pelna siatka ASCII z osobna ramka kazdego pola.
- Osobne ekrany ustawien oraz czyszczenie ekranu i historii konsoli przy przejsciach.
- Zapis ustawien, licznikow podgladow, tablicy wynikow i ostatniej planszy do TXT.
- Odczyt danych po kolejnym uruchomieniu oraz komunikaty o blednych plikach.

Wszystkie napisy i ramki w konsoli sa w ASCII. Sterowanie odbywa sie przez
wpisanie numeru i nacisniecie Enter. Nie ma obslugi strzalek ani myszy.
Tablica wynikow na poczatku jest pusta: samo losowanie nie daje punktow.

## Kompilacja i uruchomienie

Potrzebny jest kompilator z obsluga C++17. Nie ma dodatkowych bibliotek.
Polecenia wykonuj w glownym folderze projektu.

### Windows, MinGW-w64

```sh
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp src/Ustawienia.cpp src/Plansza.cpp src/TrybGry.cpp src/Gra.cpp -o saper.exe
```

Uruchomienie w PowerShell:

```powershell
.\saper.exe
```

Kolory dzialaja w terminalu obslugujacym sekwencje ANSI, np. Windows Terminal.
Program wlacza ich obsluge w konsoli Windows, jezeli jest dostepna.
Gdy nie jest dostepna, wyswietla zwykly tekst. Przy przekierowaniu wyjscia
do pliku kolory i czyszczenie ekranu sa wylaczone.

### Linux / WSL

```sh
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp src/Ustawienia.cpp src/Plansza.cpp src/TrybGry.cpp src/Gra.cpp -o saper
./saper
```

### CMake (opcjonalnie)

```sh
cmake -S . -B build
cmake --build build
```

Uruchom `./build/saper` na Linuxie, `.\build\saper.exe` przy MinGW
lub `.\build\Debug\saper.exe` przy domyslnej konfiguracji Visual Studio.

## Przyklad

Wybierz `1` (Graj), potem `1` (Klasyczny) lub `2` (Nieskonczony), a nastepnie
poziom trudnosci. Oba tryby korzystaja z tych samych pieciu poziomow:

| Poziom | Plansza | Miny |
| --- | --- | --- |
| 1. Bardzo latwy | 6 x 6 | 5 |
| 2. Latwy | 9 x 9 | 10 |
| 3. Sredni | 12 x 12 | 24 |
| 4. Trudny | 16 x 16 | 40 |
| 5. Ekspert | 18 x 18 | 65 |

Domyslny poziom to Latwy: 9 x 9. Ostatnio wybrany poziom jest zapamietywany.
Zobaczysz plansze podobna do tej ponizej (tu tylko fragment):

```text
       1   2   3
     +---+---+---+
  1  | . | 1 | * |
     +---+---+---+
  2  | 1 | 2 | 1 |
     +---+---+---+
  3  | * | 1 | . |
     +---+---+---+
```

`*` oznacza mine, `.` pole bez min w sasiedztwie, a cyfry liczbe pobliskich min.
Opcja `1` pod plansza losuje kolejny uklad, `0` wraca do wyboru trybu.
Cyfry maja stale kolory: 1 niebieski, 2 zielony, 3 czerwony, 4 niebieskofioletowy,
5 ciemnoczerwony, 6 turkusowy, 7 jasnoszary, 8 szary. Miny sa bordowe.
Pola zachowuja tlo konsoli. Wybor "Bez kolorow" wylacza rowniez
kolory planszy. Motywy menu i ramek dzialaja tak jak wczesniej.
Plansza jest zawsze wypisywana w calosci, z pelna siatka, bez podzialu na strony.
W klasycznym kazdy podglad przedstawia nowa pojedyncza gre. W nieskonczonym
numer podgladu rosnie w ramach wybranego trybu. To demonstracja generatora,
a nie przechodzenie poziomow przez gracza.

Generator `mt19937` dostaje ziarno z `random_device` raz przy uruchomieniu.
Tasujemy wszystkie indeksy pol i bierzemy tyle pozycji na miny, ile przewiduje poziom.
Pozycje nie powtarzaja sie w jednej planszy. Dwa niezalezne losowania moga
teoretycznie dac ten sam uklad.

## Pliki danych

Folder `dane` powstaje przy wyjsciu w **biezacym katalogu roboczym**.
Uruchamiaj program z tego samego folderu, aby uzywal tych samych zapisow.
Dane nie sa dodawane do Gita.

| Plik | Format |
| --- | --- |
| `ustawienia.txt` | Pierwsza linia: `USTAWIENIA_V2`. Druga: `kolor trudnosc animacja`, np. `1 2 1`. Kolor 0-3, trudnosc 1-5, animacja 0/1. Starszy format z dwoma rozmiarami jest odczytywany i przeliczany na poziom 2 lub 4. |
| `statystyki.txt` | Jedna linia: liczba wygenerowanych plansz klasycznych i nieskonczonych, np. `3 2`. To liczniki podgladow, nie wyniki gracza. |
| `wyniki.txt` | Na razie pusty. Przewidziano jeden wynik na linie: `nick punkty tryb`. Istniejace linie sa odczytywane, wyswietlane i zachowywane. |
| `ostatnia_plansza.txt` | Pierwsza linia: `wiersze kolumny miny`. Dalej wiersze planszy, bez spacji: `*` to mina, `0`-`8` liczby sasiadow. Powstaje po pierwszym losowaniu. |

Ostatnia plansza jest odczytywana i zachowywana miedzy uruchomieniami.
Wybranie trybu zawsze losuje nowa. Nie ma jeszcze wznawiania partii.
Ustawienia i statystyki sa zapisywane po opcji Wyjdz oraz po zakonczeniu wejscia
(EOF). Zmiany ustawien dotycza kolejnych podgladow; animacji kolejnego uruchomienia.
Zamkniecie procesu na sile nie zapewnia zapisu.

Brak plikow oznacza pierwszy start. Nieprawidlowe ustawienia i statystyki sa
zastepowane domyslnymi, a uszkodzona plansza jest pomijana. Przy zwyklym wyjsciu
poprawne dane zastepuja poprzedni zapis. Blad zapisu jest wypisywany w konsoli,
a program zwraca kod 1 zamiast komunikatu o udanym zapisie.

## Budowa programu

| Klasa | Odpowiedzialnosc |
| --- | --- |
| `Ustawienia` | Kolor, wielkosc planszy, animacja, sprawdzanie wartosci i zapis ustawien. |
| `Plansza` | Losowanie min, liczenie sasiadow, podglad oraz zapis i odczyt ukladu. |
| `TrybGry` | Abstrakcyjna baza trybow, wspolna plansza i przygotowanie losowania. |
| `TrybKlasyczny` | Opis trybu pojedynczego i przygotowanie jednej planszy. |
| `TrybNieskonczony` | Opis trybu ciaglego i numerowanie kolejnych podgladow. |
| `Gra` | Menu, wejscie uzytkownika, ustawienia terminala i obsluga plikow. |

Naglowki sa w `include`, implementacje w `src`.
`podglad(TrybGry&)` korzysta z metod wirtualnych obiektu wybranego trybu,
wiec dziedziczenie faktycznie dziala w programie.
`operator<<` wypisuje plansze, a `operator==` porownuje ustawienia przed i po
zmianie. Wyjatki obsluguja nieprawidlowe parametry, dane oraz bledy zapisu.
Prywatne pola, gettery, settery i metody `const` sa uzywane przez menu.

Schemat wszystkich klas: [docs/klasy.md](docs/klasy.md).

Projekt docelowo ma spelniac prog bardzo dobry. Ten etap obejmuje mechanizmy
C++ wymagane w instrukcji, ale nie zawiera jeszcze rozgrywki i naliczania punktow.
Kolejne zakresy aplikacji sa zapisane w czterech commitach i jako samodzielne
wersje w folderze [spis etapow](../README.md). Jest to podzial istniejacej aplikacji
na dzialajace etapy. Daty i autorstwo wczesniejszej historii pozostaja bez zmian.
Na oddanie potrzebna jest tez umiejetnosc wyjasnienia kodu.
