#include "TrybGry.h"
#include "Wynik.h"

#include <algorithm>
#include <istream>
#include <ostream>
#include <stdexcept>

TrybGry::TrybGry(const Ustawienia& ustawienia, const std::string& gracz)
    : plansza(ustawienia.bokPlanszy(), ustawienia.bokPlanszy(), ustawienia.liczbaMin()),
      poziom(ustawienia.getTrudnosc()), gracz(gracz) {
    if (!Wynik::poprawnyNick(gracz)) throw std::invalid_argument("Nieprawidlowy nick.");
}

void TrybGry::przygotuj(std::mt19937& generator) { plansza.losuj(generator); }
const Plansza& TrybGry::getPlansza() const { return plansza; }
Plansza& TrybGry::getPlansza() { return plansza; }
int TrybGry::getNumerPlanszy() const { return numerPlanszy; }
int TrybGry::getPoziom() const { return poziom; }
int TrybGry::getWygranePlansze() const { return wygranePlansze; }
int TrybGry::getSekundy() const { return static_cast<int>(czasMs / 1000); }
const std::string& TrybGry::getGracz() const { return gracz; }

long long TrybGry::getPunkty() const {
    int odkryte = plansza.liczbaOdkrytych() - (plansza.czyPrzegrana() ? 1 : 0);
    return punktyZakonczonych + (wygranePlansze < numerPlanszy ? odkryte * 10LL * poziom : 0);
}

void TrybGry::dodajCzas(long long milisekundy) {
    if (milisekundy < 0) throw std::invalid_argument("Nieprawidlowy czas.");
    czasMs += std::min(milisekundy, 1000000000000LL - czasMs);
}

void TrybGry::zaliczPlansze() {
    if (!plansza.czyWygrana() || wygranePlansze != numerPlanszy - 1)
        throw std::logic_error("Nie mozna ponownie zaliczyc tej planszy.");
    punktyZakonczonych = getPunkty() + 500LL * poziom;
    ++wygranePlansze;
}

void TrybGry::zapisz(std::ostream& wyjscie) const {
    wyjscie << "GRA_V1\n" << (czyKolejnaPlansza() ? 2 : 1) << ' ' << poziom << ' '
            << numerPlanszy << ' ' << wygranePlansze << ' ' << punktyZakonczonych << ' '
            << czasMs << ' ' << gracz << '\n';
    plansza.zapisz(wyjscie);
}

std::unique_ptr<TrybGry> TrybGry::wczytaj(std::istream& wejscie) {
    std::string wersja, nick;
    int tryb, poziom, numer, wygrane;
    long long punkty, czas;
    if (!(wejscie >> wersja >> tryb >> poziom >> numer >> wygrane >> punkty >> czas >> nick) ||
        wersja != "GRA_V1" || (tryb != 1 && tryb != 2) || numer < 1 || numer > 1000000 ||
        wygrane < 0 || wygrane > numer || czas < 0 || czas > 1000000000000LL)
        throw std::runtime_error("Nieprawidlowy zapis gry.");
    Ustawienia parametry(1, poziom, true);
    std::unique_ptr<TrybGry> gra;
    if (tryb == 1) gra = std::make_unique<TrybKlasyczny>(parametry, nick);
    else gra = std::make_unique<TrybNieskonczony>(parametry, nick);
    gra->plansza.wczytaj(wejscie);
    const Plansza& p = gra->plansza;
    long long zaPlansze = ((parametry.bokPlanszy() * parametry.bokPlanszy() - parametry.liczbaMin()) * 10LL + 500) * poziom;
    if (p.getWiersze() != parametry.bokPlanszy() || p.getKolumny() != parametry.bokPlanszy() ||
        p.getMiny() != parametry.liczbaMin() || p.czyPrzegrana() ||
        (tryb == 1 && (numer != 1 || wygrane != 0 || p.czyWygrana())) ||
        (p.czyWygrana() ? wygrane != numer : wygrane != numer - 1) || punkty != wygrane * zaPlansze)
        throw std::runtime_error("Niespojny stan zapisanej gry.");
    gra->numerPlanszy = numer;
    gra->wygranePlansze = wygrane;
    gra->punktyZakonczonych = punkty;
    gra->czasMs = czas;
    std::string nadmiar;
    if (wejscie >> nadmiar) throw std::runtime_error("Dodatkowe dane w zapisie gry.");
    return gra;
}

TrybKlasyczny::TrybKlasyczny(const Ustawienia& ustawienia, const std::string& gracz) : TrybGry(ustawienia, gracz) {}
std::string TrybKlasyczny::nazwa() const { return "KLASYCZNY"; }
std::string TrybKlasyczny::opis() const { return "Jedna plansza. Odkryj wszystkie pola bez min."; }
bool TrybKlasyczny::czyKolejnaPlansza() const { return false; }
void TrybKlasyczny::przygotuj(std::mt19937& generator) {
    if (numerPlanszy != 0) throw std::logic_error("Rozpocznij nowa partie klasyczna z menu.");
    TrybGry::przygotuj(generator);
    numerPlanszy = 1;
}

TrybNieskonczony::TrybNieskonczony(const Ustawienia& ustawienia, const std::string& gracz) : TrybGry(ustawienia, gracz) {}
std::string TrybNieskonczony::nazwa() const { return "NIESKONCZONY"; }
std::string TrybNieskonczony::opis() const { return "Czysc kolejne plansze. Seria trwa do trafienia na mine."; }
bool TrybNieskonczony::czyKolejnaPlansza() const { return true; }
void TrybNieskonczony::przygotuj(std::mt19937& generator) {
    if (numerPlanszy > 0 && (!plansza.czyWygrana() || wygranePlansze != numerPlanszy))
        throw std::logic_error("Najpierw ukoncz obecna plansze.");
    TrybGry::przygotuj(generator);
    ++numerPlanszy;
}
