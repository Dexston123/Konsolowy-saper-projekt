#include "Ustawienia.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

Ustawienia::Ustawienia(int kolor, int trudnosc, bool animacja) : animacja(animacja) {
    setKolor(kolor);
    setTrudnosc(trudnosc);
}

int Ustawienia::getKolor() const { return kolor; }
int Ustawienia::getTrudnosc() const { return trudnosc; }
bool Ustawienia::czyAnimacja() const { return animacja; }
int Ustawienia::bokPlanszy() const {
    const int boki[] = {6, 9, 12, 16, 18};
    return boki[trudnosc - 1];
}

int Ustawienia::liczbaMin() const {
    const int miny[] = {5, 10, 24, 40, 65};
    return miny[trudnosc - 1];
}

std::string Ustawienia::nazwaTrudnosci() const {
    const char* nazwy[] = {"Bardzo latwy", "Latwy", "Sredni", "Trudny", "Ekspert"};
    return nazwy[trudnosc - 1];
}

std::string Ustawienia::nazwaKoloru() const {
    const char* nazwy[] = {"Bez kolorow", "Zielona", "Blekitna", "Bursztynowa"};
    return nazwy[kolor];
}

void Ustawienia::setKolor(int nowy) {
    if (nowy < 0 || nowy > 3)
        throw std::invalid_argument("Nieprawidlowy numer kolorystyki.");
    kolor = nowy;
}

void Ustawienia::setTrudnosc(int nowy) {
    if (nowy < 1 || nowy > 5)
        throw std::invalid_argument("Nieprawidlowy poziom trudnosci.");
    trudnosc = nowy;
}

void Ustawienia::setAnimacja(bool wlaczona) { animacja = wlaczona; }

bool Ustawienia::operator==(const Ustawienia& inne) const {
    return kolor == inne.kolor && trudnosc == inne.trudnosc && animacja == inne.animacja;
}

void Ustawienia::wczytaj(const std::string& plik) {
    if (!std::filesystem::exists(plik)) return;
    std::ifstream wejscie(plik);
    int k, t, a;
    std::string wersja;
    std::string nadmiar;
    if (!(wejscie >> wersja)) throw std::runtime_error("Nie mozna odczytac ustawien.");
    if (wersja == "USTAWIENIA_V2") {
        if (!(wejscie >> k >> t >> a))
            throw std::runtime_error("Nie mozna odczytac ustawien.");
    } else {
        // Starszy zapis mial tylko dwa rozmiary planszy.
        std::istringstream pierwszy(wersja);
        int rozmiar;
        if (!(pierwszy >> k) || (pierwszy >> nadmiar) ||
            !(wejscie >> rozmiar >> a) || (rozmiar != 1 && rozmiar != 2))
            throw std::runtime_error("Nie mozna odczytac ustawien.");
        t = rozmiar == 1 ? 2 : 4;
    }
    if ((wejscie >> nadmiar) || (a != 0 && a != 1))
        throw std::runtime_error("Nie mozna odczytac ustawien.");
    *this = Ustawienia(k, t, a == 1);
}

void Ustawienia::zapisz(const std::string& plik) const {
    std::ofstream wyjscie(plik);
    wyjscie << "USTAWIENIA_V2\n" << kolor << ' ' << trudnosc << ' ' << animacja << '\n';
    wyjscie.close();
    if (!wyjscie) throw std::runtime_error("Nie udalo sie zapisac ustawien.");
}
