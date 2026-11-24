#include "Wynik.h"

#include <algorithm>
#include <ostream>
#include <sstream>
#include <stdexcept>

Wynik::Wynik(const std::string& gracz, long long punkty, const std::string& tryb,
             int poziom, int plansze, int sekundy, const std::string& status)
    : gracz(gracz), punkty(punkty), tryb(tryb), poziom(poziom), plansze(plansze), sekundy(sekundy), status(status) {
    if (!poprawnyNick(gracz) || punkty < 0 || punkty > 1000000000000LL ||
        (tryb != "KLASYCZNY" && tryb != "NIESKONCZONY") || poziom < 1 || poziom > 5 ||
        plansze < 0 || plansze > 1000000 || sekundy < 0 || sekundy > 1000000000 ||
        (status != "WYGRANA" && status != "PRZEGRANA" && status != "ARCHIWUM"))
        throw std::invalid_argument("Nieprawidlowy rekord wyniku.");
}

const std::string& Wynik::getGracz() const { return gracz; }
long long Wynik::getPunkty() const { return punkty; }
const std::string& Wynik::getTryb() const { return tryb; }
int Wynik::getPoziom() const { return poziom; }
int Wynik::getPlansze() const { return plansze; }
int Wynik::getSekundy() const { return sekundy; }
const std::string& Wynik::getStatus() const { return status; }

bool Wynik::operator<(const Wynik& inny) const {
    if (punkty != inny.punkty) return punkty > inny.punkty;
    if (plansze != inny.plansze) return plansze > inny.plansze;
    if (sekundy != inny.sekundy) return sekundy < inny.sekundy;
    return gracz < inny.gracz;
}

bool Wynik::poprawnyNick(const std::string& nick) {
    return !nick.empty() && nick.size() <= 16 && std::all_of(nick.begin(), nick.end(), [](char znak) {
        return (znak >= 'a' && znak <= 'z') || (znak >= 'A' && znak <= 'Z') ||
               (znak >= '0' && znak <= '9') || znak == '_' || znak == '-';
    });
}

Wynik Wynik::zLinii(const std::string& linia) {
    std::istringstream wejscie(linia);
    std::string nick, tryb, status, nadmiar;
    long long punkty;
    if (!(wejscie >> nick >> punkty >> tryb)) throw std::runtime_error("Bledny wiersz tablicy wynikow.");
    if (tryb == "klasyczny") tryb = "KLASYCZNY";
    if (tryb == "nieskonczony") tryb = "NIESKONCZONY";
    wejscie >> std::ws;
    if (wejscie.eof()) return Wynik(nick, punkty, tryb, 1, 0, 0, "ARCHIWUM");
    int poziom, plansze, sekundy;
    if (!(wejscie >> poziom >> plansze >> sekundy >> status) || (wejscie >> nadmiar))
        throw std::runtime_error("Bledny wiersz tablicy wynikow.");
    return Wynik(nick, punkty, tryb, poziom, plansze, sekundy, status);
}

std::ostream& operator<<(std::ostream& wyjscie, const Wynik& wynik) {
    return wyjscie << wynik.gracz << ' ' << wynik.punkty << ' ' << wynik.tryb << ' '
                   << wynik.poziom << ' ' << wynik.plansze << ' ' << wynik.sekundy << ' ' << wynik.status;
}
