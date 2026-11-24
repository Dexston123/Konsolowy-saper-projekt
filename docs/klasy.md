# Schemat klas

Diagram mozna wyswietlic na GitHubie (Mermaid). `+` oznacza public,
`-` private, `#` protected. Dla czytelnosci pominieto std:: oraz const
i referencje w sygnaturach. Typ int64 oznacza long long.

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
        -vector~bool~ odkryte
        -vector~bool~ flagi
        -bool rozpoczeta
        -int trafiona
        -indeks(int wiersz, int kolumna) int
        -policzSasiadow() void
        -rozmiescMiny(mt19937 generator, int bezpiecznyW, int bezpiecznyK) void
        -odkryjObszar(int wiersz, int kolumna) int
        -sprawdzCzyMoznaGrac() void
        +Plansza()
        +Plansza(int wiersze, int kolumny, int miny)
        +losuj(mt19937 generator) void
        +getWiersze() int
        +getKolumny() int
        +getMiny() int
        +getPole(int wiersz, int kolumna) int
        +liczbaOdkrytych() int
        +liczbaFlag() int
        +czyOdkryte(int wiersz, int kolumna) bool
        +czyFlaga(int wiersz, int kolumna) bool
        +czyPusta() bool
        +czyRozpoczeta() bool
        +czyWygrana() bool
        +czyPrzegrana() bool
        +odkryj(int wiersz, int kolumna, mt19937 generator) int
        +odkryjSasiadow(int wiersz, int kolumna) int
        +przelaczFlage(int wiersz, int kolumna) void
        +wypisz(ostream wyjscie, string kolorRamki) void
        +wczytaj(istream wejscie) void
        +zapisz(ostream wyjscie) void
        +wczytaj(string plik) void
        +zapisz(string plik) void
    }
    class TrybGry {
        <<abstract>>
        #Plansza plansza
        #int numerPlanszy
        #int poziom
        #int wygranePlansze
        #int64 punktyZakonczonych
        #int64 czasMs
        #string gracz
        +TrybGry(Ustawienia ustawienia, string gracz)
        +~TrybGry()
        +nazwa() string*
        +opis() string*
        +czyKolejnaPlansza() bool*
        +przygotuj(mt19937 generator) void
        +getPlansza() Plansza
        +getNumerPlanszy() int
        +getPoziom() int
        +getWygranePlansze() int
        +getSekundy() int
        +getGracz() string
        +getPunkty() int64
        +dodajCzas(int64 milisekundy) void
        +zaliczPlansze() void
        +zapisz(ostream wyjscie) void
        +wczytaj(istream wejscie) unique_ptr~TrybGry~
    }
    class TrybKlasyczny {
        +TrybKlasyczny(Ustawienia ustawienia, string gracz)
        +nazwa() string
        +opis() string
        +czyKolejnaPlansza() bool
        +przygotuj(mt19937 generator) void
    }
    class TrybNieskonczony {
        +TrybNieskonczony(Ustawienia ustawienia, string gracz)
        +nazwa() string
        +opis() string
        +czyKolejnaPlansza() bool
        +przygotuj(mt19937 generator) void
    }
    class Wynik {
        -string gracz
        -int64 punkty
        -string tryb
        -int poziom
        -int plansze
        -int sekundy
        -string status
        +Wynik(string gracz, int64 punkty, string tryb, int poziom, int plansze, int sekundy, string status)
        +getGracz() string
        +getPunkty() int64
        +getTryb() string
        +getPoziom() int
        +getPlansze() int
        +getSekundy() int
        +getStatus() string
        +operator<(Wynik inny) bool
        +poprawnyNick(string nick) bool
        +zLinii(string linia) Wynik
    }
    class Gra {
        -Ustawienia ustawienia
        -Plansza ostatniaPlansza
        -mt19937 generator
        -int klasyczne
        -int nieskonczone
        -string nick
        -vector~Wynik~ wyniki
        -vector~string~ komunikaty
        -unique_ptr~TrybGry~ aktywnaGra
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
        -pokazPlansze(TrybGry tryb, string informacja) void
        -rozegraj() void
        -zakonczGre(bool wygrana) void
        -nastepnaPlansza() void
        -pomoc() void
        -menuUstawien() void
        -zmienNick() void
        -tablicaWynikow() void
        +Gra()
        +uruchom() int
    }
    TrybGry <|-- TrybKlasyczny
    TrybGry <|-- TrybNieskonczony
    TrybGry "1" *-- "1" Plansza : biezaca plansza
    Gra "1" *-- "1" Ustawienia
    Gra "1" *-- "1" Plansza : ostatnia plansza
    Gra "1" *-- "0..1" TrybGry : aktywna gra
    Gra "1" *-- "0..*" Wynik : ranking
    Gra ..> TrybKlasyczny : tworzy
    Gra ..> TrybNieskonczony : tworzy
    TrybGry ..> Ustawienia : parametry konstruktora
    TrybGry ..> Wynik : sprawdza nick
```

Metody nazwa, opis i czyKolejnaPlansza sa czysto wirtualne w bazie.
Przygotuj i destruktor sa wirtualne. Oba tryby przeslaniaja metody przez override.
W praktyce Gra wywoluje metody obiektu przechowywanego jako unique_ptr do bazy,
a czyKolejnaPlansza rozstrzyga o zakonczeniu klasycznego lub dalszej serii.

GetPlansza wystepuje w dwoch wersjach: const do odczytu i modyfikujacej do ruchow.
TrybGry::wczytaj jest metoda statyczna tworzaca odpowiednia klase pochodna.
Wynik::poprawnyNick i Wynik::zLinii takze sa statyczne.
Operator< okresla kolejnosc wynikow.

Zaprzyjaznione operator<<(ostream&, const Plansza&) i
operator<<(ostream&, const Wynik&) sa funkcjami zewnetrznymi. Pierwszy rysuje
plansze bez kolorow, drugi formatuje rekord do pliku. Operator== ustawien
wykrywa zmiane preferencji, a operator< wyniku ustawia najlepsze rekordy
na poczatku podczas sortowania.

Kompozycja oznacza wlasne pola obiektu, wektor rekordow lub obiekt posiadany
przez unique_ptr. Przerwana zaleznosc oznacza utworzenie obiektu lub uzycie
jego interfejsu. Nie wystepuje trwala agregacja zewnetrznych obiektow.
Dokladne sygnatury i dostep do metod znajduja sie w plikach include/*.h.
