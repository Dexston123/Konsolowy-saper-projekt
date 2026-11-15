#pragma once

#include <string>

class Gra {
private:
    bool koloryDostepne = false;
    std::string kodKoloru() const;
    void zastosujKolor() const;
    void wyczyscEkran() const;
    void naglowek(const std::string& tytul, bool pokazLogo = true) const;
    void ladowanie() const;
    int wybor(int od, int doIlu) const;
    void pauza() const;
    void oGrze() const;

public:
    Gra();
    int uruchom();
};
