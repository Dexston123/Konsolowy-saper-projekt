#pragma once

#include "TrybGry.h"
#include "Wynik.h"
#include <memory>
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
    std::string nick = "Gracz";
    std::vector<Wynik> wyniki;
    std::vector<std::string> komunikaty;
    std::unique_ptr<TrybGry> aktywnaGra;
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
    void pokazPlansze(const TrybGry& tryb, const std::string& informacja = "") const;
    void rozegraj();
    void zakonczGre(bool wygrana);
    void nastepnaPlansza();
    void pomoc() const;
    void menuUstawien();
    void zmienNick();
    void tablicaWynikow() const;

public:
    Gra();
    int uruchom();
};
