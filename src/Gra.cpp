#include "Gra.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <unistd.h>
#endif

Gra::Gra() {
#ifdef _WIN32
    HANDLE konsola = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD tryb = 0;
    koloryDostepne = GetConsoleMode(konsola, &tryb) &&
        SetConsoleMode(konsola, tryb | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#else
    koloryDostepne = isatty(STDOUT_FILENO);
#endif
}

std::string Gra::kodKoloru() const {
    return koloryDostepne ? "\033[0;92m" : "";
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

void Gra::oGrze() const {
    naglowek("O GRZE");
    std::cout << "  Celem Sapera jest odkrycie wszystkich pol bez min.\n"
                 "  Cyfra na polu oznacza liczbe min w jego sasiedztwie.\n"
                 "  Mina moze znajdowac sie rowniez po przekatnej.\n\n"
                 "  Menu obslugujesz numerami i klawiszem Enter.\n";
    pauza();
}

void Gra::pokazTryb(const TrybGry& tryb) const {
    naglowek(tryb.nazwa());
    std::cout << "  " << tryb.opis() << '\n';
    pauza();
}

void Gra::menuGraj() {
    while (std::cin) {
        naglowek("WYBIERZ TRYB");
        std::cout << "  [1] Klasyczny    - jedna gra na punkty\n"
                     "  [2] Nieskonczony - kolejne plansze do przegranej\n"
                     "  [0] Powrot\n";
        int opcja = wybor(0, 2);
        if (opcja <= 0) return;
        if (opcja == 1) {
            TrybKlasyczny tryb;
            pokazTryb(tryb);
        } else {
            TrybNieskonczony tryb;
            pokazTryb(tryb);
        }
    }
}

int Gra::uruchom() {
    ladowanie();
    bool dziala = true;
    while (dziala && std::cin) {
        naglowek("MENU GLOWNE");
        std::cout << "  [1] Tryby gry\n\n  [2] O grze\n\n  [3] Wyjdz\n";
        int opcja = wybor(1, 3);
        if (opcja == 1) menuGraj();
        else if (opcja == 2) oGrze();
        else dziala = false;
    }
    std::cout << "\n  Do zobaczenia!\n";
    if (koloryDostepne) std::cout << "\033[0m" << std::flush;
    return 0;
}
