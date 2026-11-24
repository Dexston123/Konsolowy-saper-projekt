# Konsolowy Saper

Gra w C++17 na podstawie instrukcji z pliku `Projekt zaliczeniowy C++.pdf`.
Menu, ramki, komendy i komunikaty konsoli sa w ASCII.

## Rozgrywka

- Klasyczny: jedna plansza, wygrana po odkryciu wszystkich pol bez min.
- Nieskonczony: po wyczyszczeniu planszy przechodzisz do kolejnej na tym samym
  poziomie. Punkty zostaja, a seria konczy sie po trafieniu na mine.
- Pierwsze odkrycie jest bezpieczne. Na dostepnych poziomach rowniez sasiednie
  pola sa wolne od min, wiec pierwsze odkrycie otwiera pusty obszar.
- Puste obszary i otaczajace je liczby odkrywaja sie automatycznie.
- Flagi oznaczaja podejrzane pola. Mozna je zdejmowac; ich liczba nie moze
  przekroczyc liczby min. Do wygranej wystarczy odkrycie wszystkich bezpiecznych
  pol, nie trzeba oznaczac kazdej miny.
- Odkrywanie sasiadow liczby dziala po postawieniu obok odpowiedniej liczby flag.
  Blednie postawione flagi moga wtedy spowodowac trafienie na mine.
- Miny pozostaja ukryte do zakonczenia planszy. Na przegranej planszy widac
  trafiona mine i zle postawione flagi.
- Przerwana gra jest zapamietywana, lacznie z flagami, punktami, czasem i seria.
  Z menu Graj mozna ja wznowic, rowniez po ponownym uruchomieniu programu.
- Tablica wynikow zawiera zakonczone partie, nick, punkty, tryb, poziom,
  liczbe ukonczonych plansz, czas oraz wynik gry. Pokazuje 10 najlepszych wpisow
  i pozwala filtrowac tryby. Przerwanie gry przez menu nie dopisuje wyniku.

Menu glowne ma cztery opcje: Graj, Tablica Wynikow, Ustawienia, Wyjdz.
Pozostaly cztery motywy kolorystyczne, siedmiosekundowe sztuczne ladowanie
z migajacym logo oraz pelna siatka bez podzialu na strony. Duze plansze
najwygodniej ogladac w powiekszonym terminalu.

## Sterowanie

Najpierw wybierz tryb i poziom trudnosci. Numery w menu zatwierdz Enterem.
W trakcie gry wpisuj komende i wspolrzedne oddzielone spacjami, np.:

```text
o 3 5
f 2 4
a 3 5
```

| Komenda | Dzialanie |
| --- | --- |
| `o 3 5` | Odkrycie pola w wierszu 3, kolumnie 5. |
| `f 3 5` | Postawienie lub zdjecie flagi w wierszu 3, kolumnie 5. |
| `a 3 5` | Odkrycie nieoznaczonych sasiadow liczby w wierszu 3, kolumnie 5. |
| `0` | Zapisanie przerwanej gry i powrot do wyboru trybu. |
| `?` | Zasady i pomoc, bez utraty partii. |

Wspolrzedne zaczynaja sie od **1**. Litery komend moga byc male lub duze.
Puste, nieprawidlowe i nadmiarowe dane sa odrzucane. Nie ma obslugi myszy
ani strzalek; sterowanie pozostaje tekstowe.

Legenda: `#` zakryte, `F` flaga, `.` puste, `1`-`8` miny w sasiedztwie,
`*` mina, `X` trafiona mina, `!` blednie oznaczone pole po przegranej.
Kolory cyfr sa stale, miny bordowe, flagi bursztynowe. Tlo pol jest tlem konsoli.
Po zakonczeniu planszy miny sa pokazywane jako `*`, a trafiona mina jako `X`.
Wybrany motyw dotyczy menu i ramek. Opcja Bez kolorow wylacza tez kolory pol.

## Poziomy i punkty

| Poziom | Plansza | Miny |
| --- | --- | --- |
| 1. Bardzo latwy | 6 x 6 | 5 |
| 2. Latwy | 9 x 9 | 10 |
| 3. Sredni | 12 x 12 | 24 |
| 4. Trudny | 16 x 16 | 40 |
| 5. Ekspert | 18 x 18 | 65 |

Za kazde odkryte bezpieczne pole otrzymujesz `10 * poziom` punktow, a za
ukonczenie planszy dodatkowe `500 * poziom`. Automatycznie odkryte pola licza
sie tak samo. Flagi, powtorne odkrycie i samo wznowienie nie daja punktow.
Przegrana zachowuje punkty za poprawnie odkryte pola. W nieskonczonym sumowane
sa punkty z poprzednich plansz oraz biezacej.

Przyklad: wygrana na 6 x 6 z 5 minami daje `(36 - 5) * 10 + 500 = 810 pkt`.
Po wygraniu dwoch takich plansz w nieskonczonym masz 1620 pkt.
Ranking sortuje malejaco po punktach, potem po liczbie wyczyszczonych plansz,
a nastepnie po krotszym czasie i nazwie gracza.

Czas jest liczony po pierwszym odkryciu podczas oczekiwania na komendy,
lacznie z poprawianiem blednych ruchow. Menu, ekran pomocy i oczekiwanie na
kolejna plansze nie naliczaja czasu. Wyswietlany czas odswieza sie po komendzie.
Nie ma premii za czas, zegar sluzy do informacji i rozstrzygania remisow.

W Ustawieniach mozna zmienic nick (1-16 znakow ASCII: litery, cyfry, `_`, `-`).
Domyslnie jest to Gracz. Przerwana partia zachowuje nick, z ktorym ja rozpoczeto.
Zmiana domyslnego poziomu nie zmienia juz zapisanej planszy.

## Kompilacja i uruchomienie

Wymagany jest kompilator C++17. Nie sa potrzebne dodatkowe biblioteki.
Polecenia wykonuj w glownym folderze projektu, nie w katalogu etapy.

### Windows z MinGW-w64

```cmd
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp src/Ustawienia.cpp src/Plansza.cpp src/TrybGry.cpp src/Gra.cpp src/Wynik.cpp -o saper.exe
.\saper.exe
```

### Linux / WSL

```sh
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp src/Ustawienia.cpp src/Plansza.cpp src/TrybGry.cpp src/Gra.cpp src/Wynik.cpp -o build/saper
./build/saper
```

Z terminala Windows, jesli plik zostal skompilowany w WSL:

```cmd
wsl ./build/saper
```

### CMake (opcjonalnie)

```sh
cmake -S . -B build
cmake --build build
```

Uruchom `./build/saper` na Linuxie, `.\build\saper.exe` dla MinGW albo
`.\build\Debug\saper.exe` dla Visual Studio.
Kolory wymagaja terminala ANSI, np. Windows Terminal. W konsoli Windows
program wlacza ich obsluge, jezeli jest dostepna. Przekierowanie wyjscia
wylacza kolory i sekwencje czyszczenia.

## Zapis w TXT

Folder `dane` jest tworzony w **biezacym katalogu roboczym**. Uruchamiaj program
z tego samego miejsca, aby korzystac z tych samych zapisow. Nie zmieniaj
plikow podczas dzialania gry. Nowa gra po potwierdzeniu zastepuje poprzednia
niedokonczona partie; tablica wynikow jest zachowywana.

Dane sa zapisywane po poprawnym ruchu, przy przerwaniu i zakonczeniu gry oraz
przy wyjsciu z aplikacji. EOF tez zapisuje stan. Zamkniecie procesu na sile
moze utracic czas od ostatniego zapisu. Blad zapisu jest wyswietlany w konsoli,
a przy wyjsciu program zwraca kod 1, jesli nie zapisal wszystkich plikow.

| Plik | Format |
| --- | --- |
| `ustawienia.txt` | `USTAWIENIA_V2`, potem `kolor trudnosc animacja`. Kolor 0-3, poziom 1-5, animacja 0/1. Starszy format dwoch rozmiarow nadal jest odczytywany. |
| `profil.txt` | Jedna linia z nickiem gracza. |
| `statystyki.txt` | Dwie liczby: wygenerowane plansze klasyczne i nieskonczone. Starsze liczniki generatora sa zachowane; nie sa to liczniki zwyciestw. |
| `wyniki.txt` | `WYNIKI_V1`, dalej po jednym rekordzie: `nick punkty tryb poziom plansze sekundy status`. Status to WYGRANA lub PRZEGRANA. Dawne trzykolumnowe wpisy sa zachowywane jako ARCHIWUM z domyslnym poziomem 1. |
| `zapis_gry.txt` | `BRAK` albo `GRA_V1`, potem `tryb poziom numerPlanszy wygranePlansze punktyZakonczonych czasMs nick` oraz zapis planszy. Tryb 1 = klasyczny, 2 = nieskonczony. |
| `ostatnia_plansza.txt` | Ostatnio wyswietlana plansza, w takim samym formacie jak plansza wewnatrz zapisu gry. Sam ten plik nie wznawia partii. |

Zapis planszy zaczyna sie od `PLANSZA_V2`. Kolejna linia zawiera
`wiersze kolumny miny rozpoczeta trafiona`, gdzie trafiona to indeks miny
albo -1. Dalej sa wiersze ukladu (`*`, `0`-`8`) oraz wiersze stanow (`#`, `o`, `F`).
Litera `o` w pliku oznacza odkryte pole. Stary zapis samego ukladu jest tez
obslugiwany jako archiwalna, zakryta plansza.

Brak plikow oznacza pierwszy start. Zle ustawienia dostaja wartosci domyslne,
bledne rekordy rankingu sa pomijane z komunikatem, a niespojny zapis gry nie
jest wznawiany. Wczytywanie sprawdza m.in. wymiary, liczbe min, liczby sasiadow,
flagi, stan partii i zgodnosc punktow za ukonczone plansze.

## Budowa programu

| Klasa | Odpowiedzialnosc |
| --- | --- |
| Ustawienia | Kolory, domyslny poziom, animacja, walidacja i zapis preferencji. |
| Plansza | Miny, odkryte pola, flagi, zalewanie pustych obszarow, sasiedzi, stan wygranej/przegranej i rysowanie. |
| TrybGry | Wspolna plansza, poziom, nick partii, punkty, czas i zapis sesji. |
| TrybKlasyczny | Pojedyncza plansza, bez przechodzenia do kolejnej. |
| TrybNieskonczony | Kolejne plansze z zachowaniem punktow i stanu serii. |
| Wynik | Walidacja rekordu rankingu, porownywanie i format zapisu. |
| Gra | Menu, komendy gracza, obsluga czasu, plikow i przebiegu partii. |

Naglowki sa w include, implementacje w src. Gra posiada aktywny tryb przez
`unique_ptr<TrybGry>` i wywoluje jego metody wirtualne. Klasy pochodne
przeslaniaja przygotowanie, nazwe, opis i decyzje o kolejnej planszy.
`Plansza::operator<<` wypisuje plansze bez kolorow, `Ustawienia::operator==`
wykrywa zmiane preferencji, `Wynik::operator<` jest uzywany do sortowania,
a `Wynik::operator<<` zapisuje rekord. Wyjatki obsluguja bledy parametrow,
ruchow i plikow. Prywatne pola, gettery i metody const sa uzywane w programie.

Generator mt19937 jest inicjowany raz z random_device. Tasuje liste dozwolonych
pozycji i wybiera miny bez powtorzen. Pierwszy ruch uklada miny poza swoim
sasiedztwem. Do odkrywania pustego obszaru sluzy kolejka zapisana w wektorze,
dzieki czemu nie ma rekurencyjnego wywolywania dla kazdego pola.

Schemat klas: [docs/klasy.md](docs/klasy.md).
Historyczne, samodzielne wersje menu i generatora pozostaly w
[etapy](etapy/README.md). Biezaca, grywalna wersja znajduje sie w folderze glownym.

## Sprawdzenie logiki

Testy w tests/test_logika.cpp obejmuja 500 losowan, liczby sasiadow,
bezpieczny pierwszy ruch, flagi, puste obszary, odkrywanie sasiadow, wygrana,
przegrana, punkty, oba tryby, zapis i wznowienie oraz ranking.

```sh
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude tests/test_logika.cpp src/Plansza.cpp src/TrybGry.cpp src/Ustawienia.cpp src/Wynik.cpp -o build/test_logika
./build/test_logika
```

Na Windows uzyj koncowki `-o build/test_logika.exe` oraz `.\build\test_logika.exe`.
Po kompilacji CMake mozna tez uruchomic `ctest --test-dir build --output-on-failure`
(dla Visual Studio dodaj `-C Debug`). Testy nie korzystaja z katalogu danych gracza.
