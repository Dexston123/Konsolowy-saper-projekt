#include "Plansza.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <stdexcept>

Plansza::Plansza(int wiersze, int kolumny, int miny)
    : wiersze(wiersze), kolumny(kolumny), miny(miny) {
    if (wiersze < 1 || wiersze > 30 || kolumny < 1 || kolumny > 30 ||
        miny < 1 || miny >= wiersze * kolumny)
        throw std::invalid_argument("Nieprawidlowe wymiary planszy lub liczba min.");
    pola.assign(wiersze * kolumny, 0);
    odkryte.assign(pola.size(), false);
    flagi.assign(pola.size(), false);
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
int Plansza::liczbaOdkrytych() const { return static_cast<int>(std::count(odkryte.begin(), odkryte.end(), true)); }
int Plansza::liczbaFlag() const { return static_cast<int>(std::count(flagi.begin(), flagi.end(), true)); }
bool Plansza::czyOdkryte(int wiersz, int kolumna) const { return odkryte[indeks(wiersz, kolumna)]; }
bool Plansza::czyFlaga(int wiersz, int kolumna) const { return flagi[indeks(wiersz, kolumna)]; }
bool Plansza::czyPusta() const { return pola.empty(); }
bool Plansza::czyRozpoczeta() const { return rozpoczeta; }
bool Plansza::czyPrzegrana() const { return trafiona >= 0; }
bool Plansza::czyWygrana() const {
    return !czyPusta() && !czyPrzegrana() && liczbaOdkrytych() == wiersze * kolumny - miny;
}

void Plansza::rozmiescMiny(std::mt19937& generator, int bezpiecznyW, int bezpiecznyK) {
    std::vector<int> pozycje;
    for (int w = 0; w < wiersze; ++w)
        for (int k = 0; k < kolumny; ++k)
            if (bezpiecznyW < 0 || std::abs(w - bezpiecznyW) > 1 || std::abs(k - bezpiecznyK) > 1)
                pozycje.push_back(indeks(w, k));
    // Przy bardzo gestej planszy chronimy przynajmniej samo klikniete pole.
    if (static_cast<int>(pozycje.size()) < miny) {
        pozycje.clear();
        for (int i = 0; i < wiersze * kolumny; ++i)
            if (i != indeks(bezpiecznyW, bezpiecznyK)) pozycje.push_back(i);
    }
    std::fill(pola.begin(), pola.end(), 0);
    std::shuffle(pozycje.begin(), pozycje.end(), generator);
    for (int i = 0; i < miny; ++i) pola[pozycje[i]] = -1;
    policzSasiadow();
}

void Plansza::losuj(std::mt19937& generator) {
    if (czyPusta()) throw std::logic_error("Najpierw ustaw wymiary planszy.");
    std::fill(odkryte.begin(), odkryte.end(), false);
    std::fill(flagi.begin(), flagi.end(), false);
    rozpoczeta = false;
    trafiona = -1;
    rozmiescMiny(generator);
}

void Plansza::policzSasiadow() {
    for (int w = 0; w < wiersze; ++w) {
        for (int k = 0; k < kolumny; ++k) {
            if (getPole(w, k) == -1) continue;
            int ile = 0;
            for (int dw = -1; dw <= 1; ++dw) {
                for (int dk = -1; dk <= 1; ++dk) {
                    int sw = w + dw, sk = k + dk;
                    if (sw >= 0 && sw < wiersze && sk >= 0 && sk < kolumny && getPole(sw, sk) == -1) ++ile;
                }
            }
            pola[indeks(w, k)] = ile;
        }
    }
}

void Plansza::sprawdzCzyMoznaGrac() const {
    if (czyPusta()) throw std::logic_error("Brak planszy.");
    if (czyPrzegrana() || czyWygrana()) throw std::logic_error("Ta plansza jest juz zakonczona.");
}

int Plansza::odkryjObszar(int wiersz, int kolumna) {
    int start = indeks(wiersz, kolumna);
    if (odkryte[start] || flagi[start]) return 0;
    odkryte[start] = true;
    if (pola[start] == -1) {
        trafiona = start;
        return -1;
    }
    int ile = 1;
    std::vector<int> kolejka{start};
    for (std::size_t i = 0; i < kolejka.size(); ++i) {
        int pole = kolejka[i];
        if (pola[pole] != 0) continue;
        int w = pole / kolumny, k = pole % kolumny;
        for (int dw = -1; dw <= 1; ++dw) {
            for (int dk = -1; dk <= 1; ++dk) {
                int sw = w + dw, sk = k + dk;
                if (sw < 0 || sw >= wiersze || sk < 0 || sk >= kolumny) continue;
                int sasiad = indeks(sw, sk);
                if (odkryte[sasiad] || flagi[sasiad] || pola[sasiad] == -1) continue;
                odkryte[sasiad] = true;
                ++ile;
                kolejka.push_back(sasiad);
            }
        }
    }
    return ile;
}

int Plansza::odkryj(int wiersz, int kolumna, std::mt19937& generator) {
    sprawdzCzyMoznaGrac();
    int pole = indeks(wiersz, kolumna);
    if (flagi[pole]) throw std::logic_error("Najpierw zdejmij flage z tego pola.");
    if (odkryte[pole]) return 0;
    if (!rozpoczeta) {
        rozmiescMiny(generator, wiersz, kolumna);
        rozpoczeta = true;
    }
    return odkryjObszar(wiersz, kolumna);
}

void Plansza::przelaczFlage(int wiersz, int kolumna) {
    sprawdzCzyMoznaGrac();
    int pole = indeks(wiersz, kolumna);
    if (odkryte[pole]) throw std::logic_error("Nie mozna oznaczyc odkrytego pola.");
    if (!flagi[pole] && liczbaFlag() >= miny)
        throw std::logic_error("Wykorzystano wszystkie flagi. Zdejmij ktoras z nich.");
    flagi[pole] = !flagi[pole];
}

int Plansza::odkryjSasiadow(int wiersz, int kolumna) {
    sprawdzCzyMoznaGrac();
    int pole = indeks(wiersz, kolumna);
    if (!odkryte[pole] || pola[pole] <= 0)
        throw std::logic_error("Wybierz odkryte pole z liczba.");
    int oznaczone = 0;
    for (int dw = -1; dw <= 1; ++dw)
        for (int dk = -1; dk <= 1; ++dk) {
            int w = wiersz + dw, k = kolumna + dk;
            if (w >= 0 && w < wiersze && k >= 0 && k < kolumny && czyFlaga(w, k)) ++oznaczone;
        }
    if (oznaczone != pola[pole])
        throw std::logic_error("Liczba flag obok tego pola musi odpowiadac cyfrze.");
    int ile = 0;
    for (int dw = -1; dw <= 1; ++dw)
        for (int dk = -1; dk <= 1; ++dk) {
            int w = wiersz + dw, k = kolumna + dk;
            if (w < 0 || w >= wiersze || k < 0 || k >= kolumny) continue;
            int wynik = odkryjObszar(w, k);
            if (wynik == -1) return -1;
            ile += wynik;
        }
    return ile;
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
    auto ramka = [&]() {
        wyjscie << "     +";
        for (int k = 0; k < kolumny; ++k) wyjscie << "---+";
        wyjscie << '\n';
    };
    wyjscie << "     ";
    for (int k = 1; k <= kolumny; ++k) wyjscie << std::setw(3) << k << ' ';
    wyjscie << '\n';
    ramka();
    bool koniec = czyPrzegrana() || czyWygrana();
    for (int w = 0; w < wiersze; ++w) {
        wyjscie << std::setw(3) << w + 1 << "  |";
        for (int k = 0; k < kolumny; ++k) {
            int i = indeks(w, k), pole = pola[i];
            char znak = '#';
            const char* kolor = "\033[38;2;150;150;150m";
            if (i == trafiona) znak = 'X';
            else if (koniec && flagi[i] && pole != -1) znak = '!';
            else if (koniec && pole == -1) znak = '*';
            else if (flagi[i]) znak = 'F';
            else if (odkryte[i]) znak = pole == 0 ? '.' : char('0' + pole);
            if (znak == '*' || znak == 'X') kolor = "\033[38;2;180;40;80m";
            else if (znak == 'F') kolor = "\033[38;2;255;190;60m";
            else if (znak == '!') kolor = "\033[38;2;255;70;70m";
            else if (odkryte[i]) kolor = kolory[pole];
            if (!kolorRamki.empty()) wyjscie << kolor;
            wyjscie << ' ' << znak << ' ';
            if (!kolorRamki.empty()) wyjscie << "\033[0m" << kolorRamki;
            wyjscie << '|';
        }
        wyjscie << '\n';
        ramka();
    }
}

void Plansza::zapisz(std::ostream& wyjscie) const {
    wyjscie << "PLANSZA_V2\n" << wiersze << ' ' << kolumny << ' ' << miny << ' '
            << rozpoczeta << ' ' << trafiona << '\n';
    for (int w = 0; w < wiersze; ++w) {
        for (int k = 0; k < kolumny; ++k) {
            int pole = getPole(w, k);
            wyjscie << (pole == -1 ? '*' : char('0' + pole));
        }
        wyjscie << '\n';
    }
    for (int w = 0; w < wiersze; ++w) {
        for (int k = 0; k < kolumny; ++k) {
            int i = indeks(w, k);
            wyjscie << (flagi[i] ? 'F' : (odkryte[i] ? 'o' : '#'));
        }
        wyjscie << '\n';
    }
}

void Plansza::wczytaj(std::istream& wejscie) {
    std::string wersja;
    int w, k, m, start = 0, mina = -1;
    if (!(wejscie >> wersja)) throw std::runtime_error("Brak zapisu planszy.");
    bool nowy = wersja == "PLANSZA_V2";
    if (nowy) {
        if (!(wejscie >> w >> k >> m >> start >> mina) || (start != 0 && start != 1))
            throw std::runtime_error("Nieprawidlowy naglowek planszy.");
    } else {
        std::istringstream pierwszy(wersja);
        std::string nadmiar;
        if (!(pierwszy >> w) || (pierwszy >> nadmiar) || !(wejscie >> k >> m))
            throw std::runtime_error("Brak wymiarow zapisanej planszy.");
    }
    Plansza odczytana(w, k, m);
    std::vector<int> zapisane;
    int znalezione = 0;
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
    if (znalezione != m) throw std::runtime_error("Nieprawidlowa liczba min.");
    odczytana.policzSasiadow();
    if (odczytana.pola != zapisane) throw std::runtime_error("Bledne liczby obok min.");
    odczytana.rozpoczeta = start == 1;
    odczytana.trafiona = mina;
    if (mina < -1 || mina >= w * k || (mina >= 0 && odczytana.pola[mina] != -1))
        throw std::runtime_error("Bledny zapis trafionej miny.");
    if (nowy) {
        for (int i = 0; i < w; ++i) {
            std::string rzad;
            if (!(wejscie >> rzad) || rzad.size() != static_cast<std::size_t>(k))
                throw std::runtime_error("Brak stanu odkrytych pol.");
            for (int j = 0; j < k; ++j) {
                char znak = rzad[j];
                if (znak != 'F' && znak != 'o' && znak != '#')
                    throw std::runtime_error("Bledny stan pola.");
                int iPole = odczytana.indeks(i, j);
                odczytana.flagi[iPole] = znak == 'F';
                odczytana.odkryte[iPole] = znak == 'o';
                if (znak == 'o' && odczytana.pola[iPole] == -1 && iPole != mina)
                    throw std::runtime_error("Odkryta mina w niezakonczonej planszy.");
            }
        }
    }
    if ((!start && (mina >= 0 || odczytana.liczbaOdkrytych() != 0)) ||
        (start && odczytana.liczbaOdkrytych() == 0) ||
        (mina >= 0 && !odczytana.odkryte[mina]) || odczytana.liczbaFlag() > m)
        throw std::runtime_error("Niespojny stan planszy.");
    *this = odczytana;
}

void Plansza::zapisz(const std::string& plik) const {
    std::ofstream wyjscie(plik);
    zapisz(wyjscie);
    wyjscie.close();
    if (!wyjscie) throw std::runtime_error("Nie udalo sie zapisac planszy.");
}

void Plansza::wczytaj(const std::string& plik) {
    if (!std::filesystem::exists(plik)) return;
    std::ifstream wejscie(plik);
    Plansza odczytana;
    odczytana.wczytaj(wejscie);
    std::string nadmiar;
    if (wejscie >> nadmiar) throw std::runtime_error("Dodatkowe dane w pliku planszy.");
    *this = odczytana;
}
