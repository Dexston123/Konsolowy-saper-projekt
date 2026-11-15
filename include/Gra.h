#pragma once

#include "TrybGry.h"
#include <random>
#include <string>
#include <vector>

class Gra {
private:
    Ustawienia ustawienia;
    Plansza ostatniaPlansza;
    std::mt19937 generator;
    int klasyczne = 0;
    int nieskonczone = 0;
    std::vector<std::string> wyniki;
    std::vector<std::string> komunikaty;
    bool koloryDostepne = false;

    void wczytajDane();
    bool zapiszDane() const;
    void wyczyscEkran() const;
    std::string kodKoloru() const;
    void naglowek(const std::string& tytul, bool pokazLogo = true) const;
    void zastosujKolor() const;
    void ladowanie() const;
    int wybor(int od, int doIlu) const;
    void pauza() const;
    void menuGraj();
    int wybierzTrudnosc(const std::string& tytul) const;
    void podglad(TrybGry& tryb, bool klasyczny);
    void menuUstawien();
    void tablicaWynikow() const;

public:
    Gra();
    int uruchom();
};
