#pragma once

#include <string>

#include "TrybGry.h"

#include "Ustawienia.h"
#include <vector>

class Gra {
private:
    bool koloryDostepne = false;
    Ustawienia ustawienia;
    std::vector<std::string> komunikaty;
    void wczytajDane();
    bool zapiszDane() const;
    void menuUstawien();
    int wybierzTrudnosc(const std::string& tytul) const;
    std::string kodKoloru() const;
    void zastosujKolor() const;
    void wyczyscEkran() const;
    void naglowek(const std::string& tytul, bool pokazLogo = true) const;
    void ladowanie() const;
    int wybor(int od, int doIlu) const;
    void pauza() const;
    void oGrze() const;
    void menuGraj();
    void pokazTryb(const TrybGry& tryb) const;

public:
    Gra();
    int uruchom();
};
