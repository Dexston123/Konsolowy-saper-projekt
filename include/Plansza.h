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
    std::vector<bool> odkryte;
    std::vector<bool> flagi;
    bool rozpoczeta = false;
    int trafiona = -1;

    int indeks(int wiersz, int kolumna) const;
    void policzSasiadow();
    void rozmiescMiny(std::mt19937& generator, int bezpiecznyW = -1, int bezpiecznyK = -1);
    int odkryjObszar(int wiersz, int kolumna);
    void sprawdzCzyMoznaGrac() const;

public:
    Plansza() = default;
    Plansza(int wiersze, int kolumny, int miny);
    void losuj(std::mt19937& generator);
    int getWiersze() const;
    int getKolumny() const;
    int getMiny() const;
    int getPole(int wiersz, int kolumna) const;
    int liczbaOdkrytych() const;
    int liczbaFlag() const;
    bool czyOdkryte(int wiersz, int kolumna) const;
    bool czyFlaga(int wiersz, int kolumna) const;
    bool czyPusta() const;
    bool czyRozpoczeta() const;
    bool czyWygrana() const;
    bool czyPrzegrana() const;
    int odkryj(int wiersz, int kolumna, std::mt19937& generator);
    int odkryjSasiadow(int wiersz, int kolumna);
    void przelaczFlage(int wiersz, int kolumna);
    void wypisz(std::ostream& wyjscie, const std::string& kolorRamki = "") const;
    void wczytaj(std::istream& wejscie);
    void zapisz(std::ostream& wyjscie) const;
    void wczytaj(const std::string& plik);
    void zapisz(const std::string& plik) const;
    friend std::ostream& operator<<(std::ostream& wyjscie, const Plansza& plansza);
};
