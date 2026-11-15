#include "Gra.h"

#include <chrono>
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
        komunikaty.push_back(std::string(blad.what()) + " Pominieto zapis planszy.");
    }
    if (std::filesystem::exists("dane/statystyki.txt")) {
        std::ifstream plik("dane/statystyki.txt");
        int k, n;
        std::string nadmiar;
        if ((plik >> k >> n) && k >= 0 && n >= 0 && !(plik >> nadmiar)) {
            klasyczne = k;
            nieskonczone = n;
        } else {
            komunikaty.push_back("Nie mozna odczytac statystyk. Liczniki zaczynaja od zera.");
        }
    }
    if (std::filesystem::exists("dane/wyniki.txt")) {
        std::ifstream plik("dane/wyniki.txt");
        if (!plik) komunikaty.push_back("Nie mozna odczytac tablicy wynikow.");
        std::string linia;
        while (std::getline(plik, linia)) {
            if (!linia.empty()) wyniki.push_back(linia);
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
    // Blad jednego pliku nie powinien zatrzymac zapisu pozostalych.
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
    std::ofstream statystyki("dane/statystyki.txt");
    statystyki << klasyczne << ' ' << nieskonczone << '\n';
    statystyki.close();
    if (!statystyki) {
        std::cerr << "Nie udalo sie zapisac statystyk.\n";
        poprawnie = false;
    }
    std::ofstream tabela("dane/wyniki.txt");
    for (const std::string& wynik : wyniki) tabela << wynik << '\n';
    tabela.close();
    if (!tabela) {
        std::cerr << "Nie udalo sie zapisac wynikow.\n";
        poprawnie = false;
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
    naglowek("TABLICA WYNIKOW");
    if (wyniki.empty()) {
        std::cout << "  Brak zapisanych wynikow.\n";
    } else {
        std::cout << "  Gracz / punkty / tryb\n\n";
        for (const std::string& wynik : wyniki) std::cout << "  " << wynik << '\n';
    }
    pauza();
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
                     "  [0] Powrot\n";
        int opcja = wybor(0, 4);
        if (opcja <= 0) return;
        Ustawienia poprzednie = ustawienia;
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
        } else {
            ustawienia = Ustawienia();
        }
        naglowek("USTAWIENIA");
        std::cout << (ustawienia == poprzednie ? "\n  Ustawienia bez zmian.\n" : "\n  Ustawienia zmienione. Zapis przy wyjsciu.\n");
        pauza();
    }
}

void Gra::podglad(TrybGry& tryb, bool klasyczny) {
    while (std::cin) {
        tryb.przygotuj(generator);
        ostatniaPlansza = tryb.getPlansza();
        if (klasyczny) ++klasyczne;
        else ++nieskonczone;

        const Plansza& plansza = tryb.getPlansza();
        wyczyscEkran();
        zastosujKolor();
        std::cout << "  SAPER / " << tryb.nazwa() << '\n'
                  << "  " << std::string(50, '-') << '\n'
                  << "  " << tryb.opis() << '\n'
                  << "  " << ustawienia.nazwaTrudnosci() << " | "
                  << plansza.getWiersze() << " x " << plansza.getKolumny()
                  << " | Miny: " << plansza.getMiny() << '\n'
                  << "  Plansza: " << tryb.getNumerPlanszy() << "\n\n";
        if (koloryDostepne && ustawienia.getKolor() != 0)
            plansza.wypisz(std::cout, kodKoloru());
        else
            std::cout << plansza;
        std::cout << "  * = mina   . = puste pole   1-8 = miny w sasiedztwie\n"
                     "  [1] Nowa plansza   [0] Powrot do wyboru trybu\n";
        if (wybor(0, 1) != 1) return;
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
                     "  [2] Nieskonczony - grasz, az trafisz na mine\n"
                     "  [0] Powrot\n";
        int opcja = wybor(0, 2);
        if (opcja <= 0) return;
        int trudnosc = wybierzTrudnosc(opcja == 1 ? "KLASYCZNY / POZIOM TRUDNOSCI" : "NIESKONCZONY / POZIOM TRUDNOSCI");
        if (trudnosc == -1) return;
        if (trudnosc == 0) continue;
        ustawienia.setTrudnosc(trudnosc);
        if (opcja == 1) {
            TrybKlasyczny tryb(ustawienia);
            podglad(tryb, true);
        } else {
            TrybNieskonczony tryb(ustawienia);
            podglad(tryb, false);
        }
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
