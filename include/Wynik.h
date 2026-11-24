#pragma once

#include <iosfwd>
#include <string>

class Wynik {
private:
    std::string gracz;
    long long punkty;
    std::string tryb;
    int poziom;
    int plansze;
    int sekundy;
    std::string status;

public:
    Wynik(const std::string& gracz, long long punkty, const std::string& tryb,
          int poziom, int plansze, int sekundy, const std::string& status);
    const std::string& getGracz() const;
    long long getPunkty() const;
    const std::string& getTryb() const;
    int getPoziom() const;
    int getPlansze() const;
    int getSekundy() const;
    const std::string& getStatus() const;
    bool operator<(const Wynik& inny) const;
    static bool poprawnyNick(const std::string& nick);
    static Wynik zLinii(const std::string& linia);
    friend std::ostream& operator<<(std::ostream& wyjscie, const Wynik& wynik);
};
