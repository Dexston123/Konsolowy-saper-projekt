#pragma once

#include <iosfwd>
#include <random>
#include <string>
#include <vector>

class Plansza {
private:
    int wiersze = 0;
    int kolumny = 0;
    int miny = 0;
    std::vector<int> pola;

    int indeks(int wiersz, int kolumna) const;
    void policzSasiadow();

public:
    Plansza() = default;
    Plansza(int wiersze, int kolumny, int miny);
    void losuj(std::mt19937& generator);
    int getWiersze() const;
    int getKolumny() const;
    int getMiny() const;
    int getPole(int wiersz, int kolumna) const;
    bool czyPusta() const;
    void wypisz(std::ostream& wyjscie, const std::string& kolorRamki = "") const;
    void wczytaj(const std::string& plik);
    void zapisz(const std::string& plik) const;
    friend std::ostream& operator<<(std::ostream& wyjscie, const Plansza& plansza);
};
