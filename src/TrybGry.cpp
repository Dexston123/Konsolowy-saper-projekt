#include "TrybGry.h"

std::string TrybKlasyczny::nazwa() const { return "KLASYCZNY"; }
std::string TrybKlasyczny::opis() const { return "Jedna gra na punkty."; }
std::string TrybNieskonczony::nazwa() const { return "NIESKONCZONY"; }
std::string TrybNieskonczony::opis() const { return "Kolejne plansze az do trafienia na mine."; }
