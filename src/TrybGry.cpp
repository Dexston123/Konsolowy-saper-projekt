#include "TrybGry.h"

TrybGry::TrybGry(const Ustawienia& ustawienia)
    : plansza(ustawienia.bokPlanszy(), ustawienia.bokPlanszy(), ustawienia.liczbaMin()) {}

void TrybGry::przygotuj(std::mt19937& generator) {
    plansza.losuj(generator);
}

const Plansza& TrybGry::getPlansza() const { return plansza; }
int TrybGry::getNumerPlanszy() const { return numerPlanszy; }

TrybKlasyczny::TrybKlasyczny(const Ustawienia& ustawienia) : TrybGry(ustawienia) {}
std::string TrybKlasyczny::nazwa() const { return "KLASYCZNY"; }
std::string TrybKlasyczny::opis() const { return "Jedna gra na punkty."; }
void TrybKlasyczny::przygotuj(std::mt19937& generator) {
    TrybGry::przygotuj(generator);
    numerPlanszy = 1;
}

TrybNieskonczony::TrybNieskonczony(const Ustawienia& ustawienia) : TrybGry(ustawienia) {}
std::string TrybNieskonczony::nazwa() const { return "NIESKONCZONY"; }
std::string TrybNieskonczony::opis() const { return "Kolejne plansze az do trafienia na mine."; }
void TrybNieskonczony::przygotuj(std::mt19937& generator) {
    TrybGry::przygotuj(generator);
    ++numerPlanszy;
}
