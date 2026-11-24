#include "Gra.h"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <iomanip>
#include <limits>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace {
std::string czasTekst(int sekundy) {
    std::ostringstream napis;
    napis << sekundy / 60 << ':' << std::setw(2) << std::setfill('0') << sekundy % 60;
    return napis.str();
}
}

Gra::Gra() : generator(std::random_device{}()) {
#ifdef _WIN32
    HANDLE konsola = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD tryb = 0;
    koloryDostepne = GetConsoleMode(konsola, &tryb) &&
        SetConsoleMode(konsola, tryb | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#else
    koloryDostepne = isatty(STDOUT_FILENO);
#endif
    wczytajDane();
}

void Gra::wczytajDane() {
    try {
        ustawienia.wczytaj("dane/ustawienia.txt");
    } catch (const std::exception& blad) {
        komunikaty.push_back(std::string(blad.what()) + " Uzyto ustawien domyslnych.");
    }
    try {
        ostatniaPlansza.wczytaj("dane/ostatnia_plansza.txt");
    } catch (const std::exception& blad) {
        komunikaty.push_back(std::string(blad.what()) + " Pominieto ostatnia plansze.");
    }
    if (std::filesystem::exists("dane/statystyki.txt")) {
        std::ifstream plik("dane/statystyki.txt");
        int k, n;
        std::string nadmiar;
        if ((plik >> k >> n) && k >= 0 && n >= 0 && !(plik >> nadmiar)) {
            klasyczne = k;
            nieskonczone = n;
        } else komunikaty.push_back("Bledne statystyki. Liczniki zaczynaja od zera.");
    }
    if (std::filesystem::exists("dane/profil.txt")) {
        std::ifstream plik("dane/profil.txt");
        std::string zapisany, nadmiar;
        if ((plik >> zapisany) && !(plik >> nadmiar) && Wynik::poprawnyNick(zapisany)) nick = zapisany;
        else komunikaty.push_back("Bledny profil. Uzyto nazwy Gracz.");
    }
    if (std::filesystem::exists("dane/wyniki.txt")) {
        std::ifstream plik("dane/wyniki.txt");
        if (!plik) komunikaty.push_back("Nie mozna odczytac tablicy wynikow.");
        std::string linia;
        bool bledne = false;
        while (std::getline(plik, linia)) {
            if (!linia.empty() && linia.back() == '\r') linia.pop_back();
            if (linia.empty() || linia == "WYNIKI_V1") continue;
            try {
                wyniki.push_back(Wynik::zLinii(linia));
            } catch (const std::exception&) { bledne = true; }
        }
        if (bledne) komunikaty.push_back("Pominieto nieprawidlowe wpisy w tablicy wynikow.");
        std::stable_sort(wyniki.begin(), wyniki.end());
    }
    if (std::filesystem::exists("dane/zapis_gry.txt")) {
        try {
            std::ifstream plik("dane/zapis_gry.txt");
            std::string naglowek;
            if (!(plik >> naglowek)) throw std::runtime_error("Pusty zapis gry.");
            if (naglowek == "BRAK") {
                std::string nadmiar;
                if (plik >> nadmiar) throw std::runtime_error("Bledny zapis gry.");
            } else {
                plik.seekg(0);
                aktywnaGra = TrybGry::wczytaj(plik);
            }
        } catch (const std::exception& blad) {
            komunikaty.push_back(std::string(blad.what()) + " Nie wczytano przerwanej gry.");
        }
    }
}

bool Gra::zapiszDane() const {
    try {
        std::filesystem::create_directories("dane");
    } catch (const std::exception& blad) {
        std::cerr << "Nie mozna utworzyc folderu danych: " << blad.what() << '\n';
        return false;
    }
    bool poprawnie = true;
    try {
        ustawienia.zapisz("dane/ustawienia.txt");
    } catch (const std::exception& blad) {
        std::cerr << blad.what() << '\n';
        poprawnie = false;
    }
    try {
        if (!ostatniaPlansza.czyPusta()) ostatniaPlansza.zapisz("dane/ostatnia_plansza.txt");
    } catch (const std::exception& blad) {
        std::cerr << blad.what() << '\n';
        poprawnie = false;
    }
    std::ostringstream tabela, zapis;
    tabela << "WYNIKI_V1\n";
    for (const Wynik& wynik : wyniki) tabela << wynik << '\n';
    if (aktywnaGra) aktywnaGra->zapisz(zapis);
    else zapis << "BRAK\n";
    std::vector<std::pair<std::string, std::string>> pliki = {
        {"statystyki.txt", std::to_string(klasyczne) + " " + std::to_string(nieskonczone) + "\n"},
        {"profil.txt", nick + "\n"}, {"wyniki.txt", tabela.str()}, {"zapis_gry.txt", zapis.str()}
    };
    for (const auto& plik : pliki) {
        std::ofstream wyjscie("dane/" + plik.first);
        wyjscie << plik.second;
        wyjscie.close();
        if (!wyjscie) {
            std::cerr << "Nie udalo sie zapisac pliku " << plik.first << ".\n";
            poprawnie = false;
        }
    }
    return poprawnie;
}

std::string Gra::kodKoloru() const {
    if (!koloryDostepne) return "";
    const char* kolory[] = {"\033[0m", "\033[0;92m", "\033[0;96m", "\033[0;93m"};
    return kolory[ustawienia.getKolor()];
}

void Gra::zastosujKolor() const {
    std::cout << kodKoloru();
}

void Gra::wyczyscEkran() const {
    if (koloryDostepne) {
        std::cout << "\033[0m\033[H\033[2J\033[3J\033[H";
    }
#ifdef _WIN32
    else {
        HANDLE konsola = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO informacje;
        if (GetConsoleScreenBufferInfo(konsola, &informacje)) {
            COORD poczatek = {0, 0};
            DWORD zapisane;
            DWORD rozmiar = DWORD(informacje.dwSize.X) * DWORD(informacje.dwSize.Y);
            FillConsoleOutputCharacterA(konsola, ' ', rozmiar, poczatek, &zapisane);
            FillConsoleOutputAttribute(konsola, informacje.wAttributes, rozmiar, poczatek, &zapisane);
            SetConsoleCursorPosition(konsola, poczatek);
        }
    }
#endif
}

void Gra::naglowek(const std::string& tytul, bool pokazLogo) const {
    wyczyscEkran();
    zastosujKolor();
    std::cout << "\n  +--------------------------------------------------+\n"
                 "  |                                                  |\n";
    if (pokazLogo) {
        std::cout << "  |       SSS   AAA  PPPP  EEEEE RRRR                |\n"
                 "  |      S     A   A P   P E     R   R               |\n"
                 "  |       SSS  AAAAA PPPP  EEE   RRRR                |\n"
                 "  |          S A   A P     E     R  R                |\n"
                 "  |       SSS  A   A P     EEEEE R   R               |\n";
    } else {
        for (int i = 0; i < 5; ++i)
            std::cout << "  |                                                  |\n";
    }
    std::cout << "  |                                                  |\n"
                 "  +--------------------------------------------------+\n"
              << "\n  " << tytul << "\n  " << std::string(50, '-') << "\n\n";
}

void Gra::ladowanie() const {
    naglowek("URUCHAMIANIE");
    const char* kroki[] = {
        "[..........]   0%", "[##........]  20%", "[#####.....]  50%",
        "[########..]  80%", "[##########] 100%"
    };
    for (const char* krok : kroki) {
        for (int klatka = 0; klatka < 2; ++klatka) {
            if (koloryDostepne) naglowek("URUCHAMIANIE", klatka == 0);
            std::cout << "\r  Ladowanie " << krok << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(700));
        }
    }
    std::cout << "\n\n";
}

int Gra::wybor(int od, int doIlu) const {
    if (koloryDostepne) std::cout << "\0337";
    bool bledny = false;
    while (true) {
        if (koloryDostepne) std::cout << "\0338\033[J";
        if (bledny)
            std::cout << "\n  Wpisz liczbe od " << od << " do " << doIlu << ". Twoj wybor: ";
        else
            std::cout << "\n  Twoj wybor: ";
        std::cout << std::flush;
        std::string linia;
        if (!std::getline(std::cin, linia)) return -1;
        std::istringstream wejscie(linia);
        int numer;
        std::string reszta;
        if ((wejscie >> numer) && !(wejscie >> reszta) && numer >= od && numer <= doIlu)
            return numer;
        bledny = true;
    }
}

void Gra::pauza() const {
    std::cout << "\n  Nacisnij Enter, aby wrocic..." << std::flush;
    std::string linia;
    std::getline(std::cin, linia);
}

void Gra::tablicaWynikow() const {
    int filtr = 3;
    while (std::cin) {
        naglowek("TABLICA WYNIKOW / " + std::string(filtr == 1 ? "KLASYCZNY" : (filtr == 2 ? "NIESKONCZONY" : "WSZYSTKIE")));
        std::cout << "  LP " << std::left << std::setw(16) << "Gracz" << ' '
                  << std::right << std::setw(8) << "Punkty" << ' ' << std::setw(4) << "Tryb" << ' '
                  << std::setw(6) << "Poziom" << ' ' << std::setw(7) << "Plansze" << ' '
                  << std::setw(8) << "Czas" << " Wynik\n"
                     "  -----------------------------------------------------------------------\n";
        int miejsce = 0, razem = 0;
        for (const Wynik& wynik : wyniki) {
            if (filtr == 1 && wynik.getTryb() != "KLASYCZNY") continue;
            if (filtr == 2 && wynik.getTryb() != "NIESKONCZONY") continue;
            ++razem;
            if (miejsce >= 10) continue;
            std::cout << "  " << std::right << std::setw(2) << ++miejsce << ' '
                      << std::left << std::setw(16) << wynik.getGracz() << ' '
                      << std::right << std::setw(8) << wynik.getPunkty() << ' '
                      << std::setw(4) << (wynik.getTryb() == "KLASYCZNY" ? 'K' : 'N') << ' '
                      << std::setw(6) << wynik.getPoziom() << ' ' << std::setw(7) << wynik.getPlansze()
                      << ' ' << std::setw(8) << czasTekst(wynik.getSekundy())
                      << ' ' << wynik.getStatus() << '\n';
        }
        if (razem == 0) std::cout << "  Brak zapisanych wynikow.\n";
        else std::cout << "\n  Pokazano " << miejsce << " z " << razem << " wynikow.\n";
        std::cout << "\n  K = klasyczny, N = nieskonczony\n"
                  << "  Wygenerowane plansze: klasyczny " << klasyczne << ", nieskonczony " << nieskonczone << '\n'
                  << "\n  [1] Klasyczny  [2] Nieskonczony  [3] Wszystkie  [0] Powrot\n";
        int opcja = wybor(0, 3);
        if (opcja <= 0) return;
        filtr = opcja;
    }
}

void Gra::menuUstawien() {
    while (std::cin) {
        naglowek("USTAWIENIA");
        std::cout << "  [1] Kolorystyka: " << ustawienia.nazwaKoloru() << '\n'
                  << "  [2] Domyslny poziom: " << ustawienia.nazwaTrudnosci() << ", "
                  << ustawienia.bokPlanszy() << " x "
                  << ustawienia.bokPlanszy() << ", miny: " << ustawienia.liczbaMin() << '\n'
                  << "  [3] Animacja ladowania: " << (ustawienia.czyAnimacja() ? "TAK" : "NIE") << '\n'
                  << "  [4] Przywroc ustawienia domyslne\n"
                  << "  [5] Nazwa gracza: " << nick << '\n'
                  << "  [0] Powrot\n";
        int opcja = wybor(0, 5);
        if (opcja <= 0) return;
        Ustawienia poprzednie = ustawienia;
        std::string poprzedniNick = nick;
        if (opcja == 1) {
            naglowek("KOLORYSTYKA");
            std::cout << "  [0] Bez kolorow\n  [1] Zielona\n  [2] Blekitna\n  [3] Bursztynowa\n";
            int kolor = wybor(0, 3);
            if (kolor == -1) return;
            ustawienia.setKolor(kolor);
        } else if (opcja == 2) {
            int trudnosc = wybierzTrudnosc("DOMYSLNY POZIOM TRUDNOSCI");
            if (trudnosc == -1) return;
            if (trudnosc != 0) ustawienia.setTrudnosc(trudnosc);
        } else if (opcja == 3) {
            ustawienia.setAnimacja(!ustawienia.czyAnimacja());
        } else if (opcja == 4) {
            ustawienia = Ustawienia();
        } else {
            zmienNick();
        }
        naglowek("USTAWIENIA");
        std::cout << (ustawienia == poprzednie && nick == poprzedniNick ? "\n  Ustawienia bez zmian.\n" : "\n  Ustawienia zmienione. Zapis przy wyjsciu.\n");
        pauza();
    }
}

void Gra::pokazPlansze(const TrybGry& tryb, const std::string& informacja) const {
    const Plansza& plansza = tryb.getPlansza();
    Ustawienia parametry(ustawienia.getKolor(), tryb.getPoziom(), ustawienia.czyAnimacja());
    wyczyscEkran();
    zastosujKolor();
    std::cout << "  SAPER / " << tryb.nazwa() << " / " << tryb.getGracz() << '\n'
              << "  " << std::string(50, '-') << '\n'
              << "  " << tryb.opis() << '\n'
              << "  " << parametry.nazwaTrudnosci() << " | " << plansza.getWiersze() << " x " << plansza.getKolumny()
              << " | Miny: " << plansza.getMiny() << " | Flagi: " << plansza.liczbaFlag() << '/' << plansza.getMiny() << '\n'
              << "  Plansza: " << tryb.getNumerPlanszy() << " | Punkty: " << tryb.getPunkty()
              << " | Czas: " << czasTekst(tryb.getSekundy()) << '\n'
              << "  Odkryte pola: " << plansza.liczbaOdkrytych() - (plansza.czyPrzegrana() ? 1 : 0)
              << '/' << plansza.getWiersze() * plansza.getKolumny() - plansza.getMiny() << "\n\n";
    if (koloryDostepne && ustawienia.getKolor() != 0) plansza.wypisz(std::cout, kodKoloru());
    else std::cout << plansza;
    std::cout << "  # = zakryte  F = flaga  . = puste  1-8 = miny obok\n";
    if (plansza.czyPrzegrana()) std::cout << "  * = mina  X = trafiona mina  ! = bledna flaga\n";
    else if (plansza.czyWygrana()) std::cout << "  * = mina\n";
    if (!informacja.empty()) std::cout << "\n  " << informacja << '\n';
    if (!plansza.czyWygrana() && !plansza.czyPrzegrana())
        std::cout << "  o = odkryj  f = flaga  a = odkryj sasiadow\n"
                     "  [0] Zapis i menu  [?] Pomoc\n";
}

void Gra::pomoc() const {
    naglowek("JAK GRAC");
    std::cout << "  Odkryj wszystkie pola bez min. Flagi nie sa wymagane do wygranej.\n"
                 "  Wpisuj litere akcji, wiersz i kolumne, np.:\n\n"
                 "    o 3 5  - odkryj pole w wierszu 3, kolumnie 5\n"
                 "    f 3 5  - postaw lub zdejmij flage\n"
                 "    a 3 5  - odkryj sasiadow odkrytej liczby\n\n"
                 "  Akcja a dziala, gdy liczba flag obok zgadza sie z cyfra.\n"
                 "  Blednie postawione flagi moga wtedy doprowadzic do przegranej.\n"
                 "  Pierwsze odkrycie jest bezpieczne i otwiera pusty obszar.\n"
                 "  Numeracja wierszy i kolumn zaczyna sie od 1.\n\n"
                 "  Punkty: 10 x poziom za odkryte bezpieczne pole,\n"
                 "  dodatkowo 500 x poziom za ukonczenie kazdej planszy.\n"
                 "  Klasyczny: jedna plansza. Nieskonczony: seria do przegranej.\n"
                 "  0 zapisuje gre i wraca do menu. Wznowisz ja przez Graj.\n"
                 "  Miny sa ukryte do konca planszy. Zegar rusza po odkryciu pola.\n";
    pauza();
}

void Gra::nastepnaPlansza() {
    aktywnaGra->przygotuj(generator);
    int& licznik = aktywnaGra->czyKolejnaPlansza() ? nieskonczone : klasyczne;
    if (licznik < std::numeric_limits<int>::max()) ++licznik;
    ostatniaPlansza = aktywnaGra->getPlansza();
}

void Gra::zakonczGre(bool wygrana) {
    TrybGry& tryb = *aktywnaGra;
    if (wygrana) tryb.zaliczPlansze();
    wyniki.emplace_back(tryb.getGracz(), tryb.getPunkty(), tryb.nazwa(), tryb.getPoziom(),
                        tryb.getWygranePlansze(), tryb.getSekundy(), wygrana ? "WYGRANA" : "PRZEGRANA");
    std::stable_sort(wyniki.begin(), wyniki.end());
    pokazPlansze(tryb, wygrana ? "WYGRANA! Wszystkie bezpieczne pola zostaly odkryte." : "BUM! Trafiles na mine. Koniec gry.");
    std::cout << "  Wynik koncowy: " << tryb.getPunkty() << " pkt | Ukonczone plansze: " << tryb.getWygranePlansze() << '\n';
    ostatniaPlansza = tryb.getPlansza();
    aktywnaGra.reset();
    if (zapiszDane()) std::cout << "  Wynik zapisany w tablicy wynikow.\n";
    else std::cout << "  Nie udalo sie zapisac wszystkich danych.\n";
    pauza();
}

void Gra::rozegraj() {
    std::string informacja;
    while (aktywnaGra && std::cin) {
        TrybGry& tryb = *aktywnaGra;
        Plansza& plansza = tryb.getPlansza();
        if (plansza.czyWygrana()) {
            if (!tryb.czyKolejnaPlansza()) { zakonczGre(true); return; }
            if (tryb.getWygranePlansze() < tryb.getNumerPlanszy()) tryb.zaliczPlansze();
            ostatniaPlansza = plansza;
            bool zapisano = zapiszDane();
            pokazPlansze(tryb, "PLANSZA UKONCZONA! Premia: " + std::to_string(500 * tryb.getPoziom()) + " pkt.");
            if (!zapisano) std::cout << "  Nie udalo sie zapisac wszystkich danych.\n";
            std::cout << "  [1] Kolejna plansza  [0] Zapis i menu\n";
            if (wybor(0, 1) != 1) return;
            nastepnaPlansza();
            informacja.clear();
            if (!zapiszDane()) informacja = "Nie udalo sie zapisac wszystkich danych.";
            continue;
        }
        if (plansza.czyPrzegrana()) { zakonczGre(false); return; }
        pokazPlansze(tryb, informacja);
        std::cout << "\n  Ruch: " << std::flush;
        auto poczatek = std::chrono::steady_clock::now();
        std::string linia;
        bool odczytano = static_cast<bool>(std::getline(std::cin, linia));
        if (plansza.czyRozpoczeta()) {
            auto czas = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - poczatek).count();
            tryb.dodajCzas(czas);
        }
        if (!odczytano) { ostatniaPlansza = plansza; zapiszDane(); return; }
        std::istringstream wejscie(linia);
        std::string akcja, nadmiar;
        wejscie >> akcja;
        if (akcja == "0" && !(wejscie >> nadmiar)) {
            ostatniaPlansza = plansza;
            if (!zapiszDane()) { std::cout << "  Blad zapisu gry.\n"; pauza(); }
            return;
        }
        if (akcja == "?" && !(wejscie >> nadmiar)) { pomoc(); informacja.clear(); continue; }
        int wiersz, kolumna;
        if (akcja.size() != 1 || !(wejscie >> wiersz >> kolumna) || (wejscie >> nadmiar) ||
            wiersz < 1 || wiersz > plansza.getWiersze() || kolumna < 1 || kolumna > plansza.getKolumny()) {
            informacja = "Wpisz np. o 3 5, f 3 5, a 3 5 albo 0, aby wrocic.";
            continue;
        }
        char ruch = static_cast<char>(std::tolower(static_cast<unsigned char>(akcja[0])));
        try {
            int odkryto = 0;
            if (ruch == 'o') odkryto = plansza.odkryj(wiersz - 1, kolumna - 1, generator);
            else if (ruch == 'f') plansza.przelaczFlage(wiersz - 1, kolumna - 1);
            else if (ruch == 'a') odkryto = plansza.odkryjSasiadow(wiersz - 1, kolumna - 1);
            else throw std::invalid_argument("Nieznana akcja. Uzyj o, f albo a.");
            if (ruch == 'f')
                informacja = plansza.czyFlaga(wiersz - 1, kolumna - 1) ? "Flaga postawiona." : "Flaga zdjeta.";
            else
                informacja = odkryto == 0 ? "Nie odkryto nowych pol." : "Odkryto pol: " + std::to_string(odkryto);
            ostatniaPlansza = plansza;
            if (!plansza.czyWygrana() && !plansza.czyPrzegrana() && !zapiszDane())
                informacja = "Nie udalo sie zapisac wszystkich danych.";
        } catch (const std::exception& blad) {
            informacja = blad.what();
        }
    }
}

void Gra::zmienNick() {
    std::string informacja;
    while (std::cin) {
        naglowek("NAZWA GRACZA");
        std::cout << "  Obecna nazwa: " << nick << '\n'
                  << "  Wpisz 1-16 znakow: litery ASCII, cyfry, _ lub -.\n"
                     "  Pusty Enter zachowuje obecna nazwe.\n";
        if (!informacja.empty()) std::cout << "  " << informacja << '\n';
        std::cout << "\n  Nowa nazwa: " << std::flush;
        std::string nowy;
        if (!std::getline(std::cin, nowy) || nowy.empty()) return;
        if (Wynik::poprawnyNick(nowy)) { nick = nowy; return; }
        informacja = "Nieprawidlowa nazwa gracza.";
    }
}

int Gra::wybierzTrudnosc(const std::string& tytul) const {
    naglowek(tytul);
    for (int poziom = 1; poziom <= 5; ++poziom) {
        Ustawienia opcja(ustawienia.getKolor(), poziom, ustawienia.czyAnimacja());
        std::cout << "  [" << poziom << "] " << opcja.nazwaTrudnosci() << " - "
                  << opcja.bokPlanszy() << " x " << opcja.bokPlanszy()
                  << ", miny: " << opcja.liczbaMin();
        if (poziom == ustawienia.getTrudnosc()) std::cout << " (domyslny)";
        std::cout << '\n';
    }
    std::cout << "  [0] Powrot\n";
    return wybor(0, 5);
}

void Gra::menuGraj() {
    while (std::cin) {
        naglowek("WYBIERZ TRYB");
        std::cout << "  [1] Klasyczny    - jedna gra na punkty\n"
                     "  [2] Nieskonczony - grasz, az trafisz na mine\n";
        if (aktywnaGra)
            std::cout << "  [3] Wznow: " << aktywnaGra->nazwa() << ", poziom " << aktywnaGra->getPoziom()
                      << ", " << aktywnaGra->getPunkty() << " pkt\n";
        std::cout << "  [0] Powrot\n";
        int opcja = wybor(0, aktywnaGra ? 3 : 2);
        if (opcja <= 0) return;
        if (opcja == 3) { rozegraj(); continue; }
        int trudnosc = wybierzTrudnosc(opcja == 1 ? "KLASYCZNY / POZIOM TRUDNOSCI" : "NIESKONCZONY / POZIOM TRUDNOSCI");
        if (trudnosc == -1) return;
        if (trudnosc == 0) continue;
        if (aktywnaGra) {
            naglowek("NOWA GRA");
            std::cout << "  Masz niedokonczona gre. Nowa gra zastapi jej zapis.\n"
                         "  [1] Rozpocznij nowa gre  [0] Zachowaj poprzednia\n";
            int decyzja = wybor(0, 1);
            if (decyzja == -1) return;
            if (decyzja == 0) continue;
        }
        ustawienia.setTrudnosc(trudnosc);
        if (opcja == 1) aktywnaGra = std::make_unique<TrybKlasyczny>(ustawienia, nick);
        else aktywnaGra = std::make_unique<TrybNieskonczony>(ustawienia, nick);
        nastepnaPlansza();
        if (!zapiszDane()) { std::cout << "  Nie udalo sie zapisac gry.\n"; pauza(); }
        rozegraj();
    }
}

int Gra::uruchom() {
    if (ustawienia.czyAnimacja()) ladowanie();
    if (!komunikaty.empty()) {
        for (const std::string& komunikat : komunikaty) std::cout << "  " << komunikat << '\n';
        pauza();
    }
    bool dziala = true;
    while (dziala && std::cin) {
        naglowek("MENU GLOWNE");
        std::cout << "  Gracz: " << nick << "\n\n";
        std::cout << "  [1] Graj\n\n  [2] Tablica Wynikow\n\n  [3] Ustawienia\n\n  [4] Wyjdz\n";
        switch (wybor(1, 4)) {
            case 1: menuGraj(); break;
            case 2: tablicaWynikow(); break;
            case 3: menuUstawien(); break;
            default: dziala = false;
        }
    }
    bool zapisano = zapiszDane();
    std::cout << (zapisano ? "\n  Dane zapisane. Do zobaczenia!\n" : "\n  Nie wszystkie dane zostaly zapisane.\n");
    if (koloryDostepne) std::cout << "\033[0m" << std::flush;
    return zapisano ? 0 : 1;
}
