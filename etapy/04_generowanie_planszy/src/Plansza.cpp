#include "Plansza.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <ostream>
#include <stdexcept>

Plansza::Plansza(int wiersze, int kolumny, int miny)
    : wiersze(wiersze), kolumny(kolumny), miny(miny) {
    if (wiersze < 1 || wiersze > 30 || kolumny < 1 || kolumny > 30 ||
        miny < 1 || miny >= wiersze * kolumny)
        throw std::invalid_argument("Nieprawidlowe wymiary planszy lub liczba min.");
    pola.assign(wiersze * kolumny, 0);
}

int Plansza::indeks(int wiersz, int kolumna) const {
    if (wiersz < 0 || wiersz >= wiersze || kolumna < 0 || kolumna >= kolumny)
        throw std::out_of_range("Pole poza plansza.");
    return wiersz * kolumny + kolumna;
}

int Plansza::getWiersze() const { return wiersze; }
int Plansza::getKolumny() const { return kolumny; }
int Plansza::getMiny() const { return miny; }
int Plansza::getPole(int wiersz, int kolumna) const { return pola[indeks(wiersz, kolumna)]; }
bool Plansza::czyPusta() const { return pola.empty(); }

void Plansza::losuj(std::mt19937& generator) {
    if (czyPusta()) throw std::logic_error("Najpierw ustaw wymiary planszy.");
    std::fill(pola.begin(), pola.end(), 0);

    std::vector<int> pozycje(pola.size());
    std::iota(pozycje.begin(), pozycje.end(), 0);
    std::shuffle(pozycje.begin(), pozycje.end(), generator);
    for (int i = 0; i < miny; ++i) pola[pozycje[i]] = -1;
    policzSasiadow();
}

void Plansza::policzSasiadow() {
    for (int w = 0; w < wiersze; ++w) {
        for (int k = 0; k < kolumny; ++k) {
            if (getPole(w, k) == -1) continue;
            int ile = 0;
            for (int dw = -1; dw <= 1; ++dw) {
                for (int dk = -1; dk <= 1; ++dk) {
                    int sw = w + dw, sk = k + dk;
                    if (sw >= 0 && sw < wiersze && sk >= 0 && sk < kolumny &&
                        getPole(sw, sk) == -1) ++ile;
                }
            }
            pola[indeks(w, k)] = ile;
        }
    }
}

std::ostream& operator<<(std::ostream& wyjscie, const Plansza& plansza) {
    plansza.wypisz(wyjscie);
    return wyjscie;
}

void Plansza::wypisz(std::ostream& wyjscie, const std::string& kolorRamki) const {
    if (czyPusta()) return;
    const char* kolory[] = {
        "\033[38;2;128;128;128m", "\033[38;2;70;120;255m", "\033[38;2;0;180;0m",
        "\033[38;2;255;70;70m", "\033[38;2;100;100;220m", "\033[38;2;180;70;70m",
        "\033[38;2;0;180;180m", "\033[38;2;210;210;210m", "\033[38;2;128;128;128m"
    };
    wyjscie << "     ";
    for (int k = 1; k <= kolumny; ++k) wyjscie << std::setw(3) << k << ' ';
    auto ramka = [&]() {
        wyjscie << "     +";
        for (int k = 0; k < kolumny; ++k) wyjscie << "---+";
        wyjscie << '\n';
    };
    wyjscie << '\n';
    ramka();
    for (int w = 0; w < wiersze; ++w) {
        wyjscie << std::setw(3) << w + 1 << "  |";
        for (int k = 0; k < kolumny; ++k) {
            int pole = getPole(w, k);
            char znak = pole == -1 ? '*' : (pole == 0 ? '.' : char('0' + pole));
            if (!kolorRamki.empty()) {
                wyjscie << (pole == -1 ? "\033[38;2;180;40;80m" : kolory[pole]);
            }
            wyjscie << ' ' << znak << ' ';
            if (!kolorRamki.empty()) wyjscie << "\033[0m" << kolorRamki;
            wyjscie << '|';
        }
        wyjscie << '\n';
        ramka();
    }
}

void Plansza::zapisz(const std::string& plik) const {
    std::ofstream wyjscie(plik);
    wyjscie << wiersze << ' ' << kolumny << ' ' << miny << '\n';
    for (int w = 0; w < wiersze; ++w) {
        for (int k = 0; k < kolumny; ++k) {
            int pole = getPole(w, k);
            wyjscie << (pole == -1 ? '*' : char('0' + pole));
        }
        wyjscie << '\n';
    }
    wyjscie.close();
    if (!wyjscie) throw std::runtime_error("Nie udalo sie zapisac ostatniej planszy.");
}

void Plansza::wczytaj(const std::string& plik) {
    if (!std::filesystem::exists(plik)) return;
    std::ifstream wejscie(plik);
    int w, k, m;
    if (!(wejscie >> w >> k >> m)) throw std::runtime_error("Brak wymiarow zapisanej planszy.");
    Plansza odczytana(w, k, m);
    int znalezione = 0;
    std::vector<int> zapisane;
    for (int i = 0; i < w; ++i) {
        std::string rzad;
        if (!(wejscie >> rzad) || rzad.size() != static_cast<std::size_t>(k))
            throw std::runtime_error("Uszkodzony wiersz zapisanej planszy.");
        for (char znak : rzad) {
            if (znak != '*' && (znak < '0' || znak > '8'))
                throw std::runtime_error("Nieprawidlowe pole w zapisanej planszy.");
            int pole = znak == '*' ? -1 : znak - '0';
            odczytana.pola[zapisane.size()] = pole;
            zapisane.push_back(pole);
            if (pole == -1) ++znalezione;
        }
    }
    std::string nadmiar;
    if (znalezione != m || (wejscie >> nadmiar))
        throw std::runtime_error("Nieprawidlowa liczba min lub dodatkowe dane planszy.");
    odczytana.policzSasiadow();
    if (odczytana.pola != zapisane)
        throw std::runtime_error("Bledne liczby obok min w zapisanej planszy.");
    *this = odczytana;
}
