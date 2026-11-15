# Schemat klas

Diagram mozna wyswietlic na GitHubie (Mermaid). `+` oznacza public,
`-` private, `#` protected. Wspolne metody wirtualne zaznaczono w opisach.

```mermaid
classDiagram
    class Ustawienia {
        -int kolor
        -int trudnosc
        -bool animacja
        +Ustawienia()
        +Ustawienia(int kolor, int trudnosc, bool animacja)
        +getKolor() int
        +getTrudnosc() int
        +czyAnimacja() bool
        +bokPlanszy() int
        +liczbaMin() int
        +nazwaKoloru() string
        +nazwaTrudnosci() string
        +setKolor(int nowy) void
        +setTrudnosc(int nowy) void
        +setAnimacja(bool wlaczona) void
        +wczytaj(string plik) void
        +zapisz(string plik) void
        +operator==(Ustawienia inne) bool
    }
    class Plansza {
        -int wiersze
        -int kolumny
        -int miny
        -vector~int~ pola
        -indeks(int wiersz, int kolumna) int
        -policzSasiadow() void
        +Plansza()
        +Plansza(int wiersze, int kolumny, int miny)
        +losuj(mt19937 generator) void
        +getWiersze() int
        +getKolumny() int
        +getMiny() int
        +getPole(int wiersz, int kolumna) int
        +czyPusta() bool
        +wypisz(ostream wyjscie, string kolorRamki) void
        +wczytaj(string plik) void
        +zapisz(string plik) void
    }
    class TrybGry {
        <<abstract>>
        #Plansza plansza
        #int numerPlanszy
        +TrybGry(Ustawienia ustawienia)
        +~TrybGry()
        +nazwa() string*
        +opis() string*
        +przygotuj(mt19937 generator) void
        +getPlansza() Plansza
        +getNumerPlanszy() int
    }
    class TrybKlasyczny {
        +TrybKlasyczny(Ustawienia ustawienia)
        +nazwa() string
        +opis() string
        +przygotuj(mt19937 generator) void
    }
    class TrybNieskonczony {
        +TrybNieskonczony(Ustawienia ustawienia)
        +nazwa() string
        +opis() string
        +przygotuj(mt19937 generator) void
    }
    class Gra {
        -Ustawienia ustawienia
        -Plansza ostatniaPlansza
        -mt19937 generator
        -int klasyczne
        -int nieskonczone
        -vector~string~ wyniki
        -vector~string~ komunikaty
        -bool koloryDostepne
        -wczytajDane() void
        -zapiszDane() bool
        -wyczyscEkran() void
        -kodKoloru() string
        -naglowek(string tytul, bool pokazLogo) void
        -zastosujKolor() void
        -ladowanie() void
        -wybor(int od, int doIlu) int
        -pauza() void
        -menuGraj() void
        -wybierzTrudnosc(string tytul) int
        -podglad(TrybGry tryb, bool klasyczny) void
        -menuUstawien() void
        -tablicaWynikow() void
        +Gra()
        +uruchom() int
    }
    TrybGry <|-- TrybKlasyczny
    TrybGry <|-- TrybNieskonczony
    TrybGry "1" *-- "1" Plansza : plansza trybu
    Gra "1" *-- "1" Ustawienia
    Gra "1" *-- "1" Plansza : ostatni podglad
    Gra ..> TrybGry : podglad przez referencje
    Gra ..> TrybKlasyczny : tworzy lokalnie
    Gra ..> TrybNieskonczony : tworzy lokalnie
    TrybGry ..> Ustawienia : parametry konstruktora
```

`nazwa` i `opis` sa czysto wirtualne w bazie; `przygotuj` i destruktor sa
wirtualne. Klasy pochodne przeslaniaja `nazwa`, `opis` oraz `przygotuj`.
W diagramie pominieto `const` i referencje w sygnaturach dla czytelnosci;
dokladne deklaracje znajduja sie w naglowkach.

Zaprzyjazniona funkcja `operator<<(ostream&, const Plansza&)` jest funkcja
zewnetrzna, a nie metoda klasy. Korzysta z danych planszy przy jej wypisywaniu.
`Ustawienia::operator==` jest metoda klasy i pozwala wykryc zmiane ustawien.

Kompozycja oznacza wlasne pola obiektu. Zaleznosc przerywana oznacza uzycie
parametru albo utworzenie obiektu lokalnego. Nie ma trwalej agregacji obiektow
zewnetrznych, dlatego nie dodano pustego rombu.
