#pragma once

#include <string>

class Ustawienia {
private:
    int kolor = 1;
    int trudnosc = 2;
    bool animacja = true;

public:
    Ustawienia() = default;
    Ustawienia(int kolor, int trudnosc, bool animacja);

    int getKolor() const;
    int getTrudnosc() const;
    bool czyAnimacja() const;
    int bokPlanszy() const;
    int liczbaMin() const;
    std::string nazwaKoloru() const;
    std::string nazwaTrudnosci() const;
    void setKolor(int nowy);
    void setTrudnosc(int nowy);
    void setAnimacja(bool wlaczona);
    void wczytaj(const std::string& plik);
    void zapisz(const std::string& plik) const;
    bool operator==(const Ustawienia& inne) const;
};
