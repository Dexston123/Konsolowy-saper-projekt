#include "Plansza.h"
#include "TrybGry.h"
#include "Wynik.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <vector>

static void oczekujBledu(const std::function<void()>& operacja) {
    bool byl = false;
    try { operacja(); } catch (const std::exception&) { byl = true; }
    assert(byl);
}

static Plansza znanaPlansza(int bok, const std::vector<int>& miny, const std::vector<int>& otwarte) {
    std::ostringstream tekst;
    tekst << "PLANSZA_V2\n" << bok << ' ' << bok << ' ' << miny.size() << " 1 -1\n";
    for (int w = 0; w < bok; ++w) {
        for (int k = 0; k < bok; ++k) {
            int i = w * bok + k;
            if (std::find(miny.begin(), miny.end(), i) != miny.end()) tekst << '*';
            else {
                int sasiedzi = 0;
                for (int mina : miny)
                    if (std::abs(mina / bok - w) <= 1 && std::abs(mina % bok - k) <= 1) ++sasiedzi;
                tekst << sasiedzi;
            }
        }
        tekst << '\n';
    }
    for (int i = 0; i < bok * bok; ++i) {
        tekst << (std::find(otwarte.begin(), otwarte.end(), i) != otwarte.end() ? 'o' : '#');
        if (i % bok == bok - 1) tekst << '\n';
    }
    Plansza plansza;
    std::istringstream wejscie(tekst.str());
    plansza.wczytaj(wejscie);
    return plansza;
}

static void sprawdzUklad(const Plansza& p) {
    int znalezione = 0;
    for (int w = 0; w < p.getWiersze(); ++w) {
        for (int k = 0; k < p.getKolumny(); ++k) {
            if (p.getPole(w, k) == -1) { ++znalezione; continue; }
            int sasiedzi = 0;
            for (int sw = std::max(0, w - 1); sw <= std::min(p.getWiersze() - 1, w + 1); ++sw)
                for (int sk = std::max(0, k - 1); sk <= std::min(p.getKolumny() - 1, k + 1); ++sk)
                    if (p.getPole(sw, sk) == -1) ++sasiedzi;
            assert(p.getPole(w, k) == sasiedzi);
        }
    }
    assert(znalezione == p.getMiny());
}

static void wyczysc(TrybGry& tryb, std::mt19937& generator) {
    Plansza& p = tryb.getPlansza();
    if (!p.czyRozpoczeta()) p.odkryj(0, 0, generator);
    for (int w = 0; w < p.getWiersze() && !p.czyWygrana(); ++w)
        for (int k = 0; k < p.getKolumny() && !p.czyWygrana(); ++k)
            if (p.getPole(w, k) != -1 && !p.czyOdkryte(w, k)) p.odkryj(w, k, generator);
    assert(p.czyWygrana());
}

int main() {
    std::mt19937 generator(12345);
    for (int poziom = 1; poziom <= 5; ++poziom) {
        Ustawienia u(1, poziom, false);
        for (int proba = 0; proba < 100; ++proba) {
            Plansza p(u.bokPlanszy(), u.bokPlanszy(), u.liczbaMin());
            p.losuj(generator);
            sprawdzUklad(p);
            std::ostringstream ukryta;
            ukryta << p;
            assert(ukryta.str().find('*') == std::string::npos);
            assert(!p.czyRozpoczeta() && p.liczbaOdkrytych() == 0);
            int w = proba % u.bokPlanszy(), k = (proba * 7) % u.bokPlanszy();
            p.odkryj(w, k, generator);
            assert(p.getPole(w, k) == 0 && !p.czyPrzegrana() && p.czyOdkryte(w, k));
            sprawdzUklad(p);
            std::ostringstream zapis;
            p.zapisz(zapis);
            Plansza odczyt;
            std::istringstream wejscie(zapis.str());
            odczyt.wczytaj(wejscie);
            std::ostringstream ponownie;
            odczyt.zapisz(ponownie);
            assert(zapis.str() == ponownie.str());
        }
    }
    Plansza gesta(2, 2, 3);
    gesta.losuj(generator);
    assert(gesta.odkryj(0, 0, generator) == 1 && gesta.czyWygrana());
    Plansza chord = znanaPlansza(3, {0}, {4});
    oczekujBledu([&] { chord.odkryjSasiadow(1, 1); });
    chord.przelaczFlage(0, 0);
    oczekujBledu([&] { chord.odkryj(0, 0, generator); });
    oczekujBledu([&] { chord.przelaczFlage(0, 1); });
    assert(chord.odkryjSasiadow(1, 1) == 7 && chord.czyWygrana());
    std::ostringstream wygrana;
    chord.wypisz(wygrana);
    std::string widokWygranej = wygrana.str();
    assert(std::count(widokWygranej.begin(), widokWygranej.end(), '*') == chord.getMiny());
    assert(widokWygranej.find('F') == std::string::npos);
    oczekujBledu([&] { chord.przelaczFlage(0, 0); });

    Plansza blad = znanaPlansza(3, {0}, {4});
    blad.przelaczFlage(0, 1);
    assert(blad.odkryjSasiadow(1, 1) == -1 && blad.czyPrzegrana());
    std::ostringstream koniec;
    blad.wypisz(koniec, "\033[0;92m");
    assert(koniec.str().find('X') != std::string::npos && koniec.str().find('!') != std::string::npos);
    assert(koniec.str().find("\033[48;") == std::string::npos);
    std::ostringstream zapisPrzegranej;
    blad.zapisz(zapisPrzegranej);
    Plansza odczytPrzegranej;
    std::istringstream przegrana(zapisPrzegranej.str());
    odczytPrzegranej.wczytaj(przegrana);
    assert(odczytPrzegranej.czyPrzegrana());

    Plansza zalewanie = znanaPlansza(3, {0}, {8});
    zalewanie.przelaczFlage(1, 1);
    zalewanie.odkryj(2, 1, generator);
    assert(!zalewanie.czyWygrana() && !zalewanie.czyOdkryte(1, 1));
    zalewanie.przelaczFlage(1, 1);
    zalewanie.odkryj(1, 1, generator);
    assert(zalewanie.czyWygrana());
    Plansza duza = znanaPlansza(30, {0}, {899});
    duza.odkryj(29, 28, generator);
    assert(duza.czyWygrana() && duza.liczbaOdkrytych() == 899);
    oczekujBledu([&] { duza.getPole(-1, 0); });
    oczekujBledu([&] { duza.getPole(std::numeric_limits<int>::max(), 0); });

    Ustawienia u(1, 1, false);
    TrybKlasyczny klasyczny(u, "Tester");
    klasyczny.przygotuj(generator);
    wyczysc(klasyczny, generator);
    assert(!klasyczny.czyKolejnaPlansza() && klasyczny.getPunkty() == 310);
    klasyczny.zaliczPlansze();
    assert(klasyczny.getPunkty() == 810 && klasyczny.getWygranePlansze() == 1);
    oczekujBledu([&] { klasyczny.zaliczPlansze(); });
    oczekujBledu([&] { klasyczny.przygotuj(generator); });
    TrybNieskonczony seria(u, "Tester");
    seria.przygotuj(generator);
    oczekujBledu([&] { seria.przygotuj(generator); });
    wyczysc(seria, generator);
    seria.zaliczPlansze();
    seria.dodajCzas(12345);
    std::ostringstream zapisSerii;
    seria.zapisz(zapisSerii);
    std::istringstream wznowienie(zapisSerii.str());
    auto wznowiona = TrybGry::wczytaj(wznowienie);
    assert(wznowiona->getPunkty() == 810 && wznowiona->getSekundy() == 12);
    assert(wznowiona->getPlansza().czyWygrana());
    wznowiona->przygotuj(generator);
    assert(wznowiona->getNumerPlanszy() == 2 && wznowiona->getPunkty() == 810);
    wyczysc(*wznowiona, generator);
    wznowiona->zaliczPlansze();
    assert(wznowiona->getPunkty() == 1620 && wznowiona->getWygranePlansze() == 2);
    wznowiona->dodajCzas(std::numeric_limits<long long>::max());
    assert(wznowiona->getSekundy() == 1000000000);

    TrybKlasyczny przerwana(u, "Tester");
    przerwana.przygotuj(generator);
    przerwana.getPlansza().przelaczFlage(1, 1);
    std::ostringstream zapisPrzedRuchem;
    przerwana.zapisz(zapisPrzedRuchem);
    std::istringstream stan(zapisPrzedRuchem.str());
    auto start = TrybGry::wczytaj(stan);
    assert(!start->getPlansza().czyRozpoczeta() && start->getPlansza().czyFlaga(1, 1));
    std::istringstream stare("3 3 1\n*10\n110\n000\n");
    Plansza archiwum;
    archiwum.wczytaj(stare);
    assert(!archiwum.czyRozpoczeta());
    for (const std::string zapis : {"PLANSZA_V2\n999 2 1 0 -1\n", "PLANSZA_V2\n3 3 1 1 -1\n*10\n110\n000\no##\n###\n###\n"}) {
        oczekujBledu([&] { std::istringstream plik(zapis); Plansza p; p.wczytaj(plik); });
    }
    oczekujBledu([&] { std::istringstream plik(zapisSerii.str() + "smieci"); TrybGry::wczytaj(plik); });
    std::string falszywe = zapisSerii.str();
    auto miejsce = falszywe.find("810");
    falszywe.replace(miejsce, 3, "999");
    oczekujBledu([&] { std::istringstream plik(falszywe); TrybGry::wczytaj(plik); });
    Wynik stary = Wynik::zLinii("Adam 100 klasyczny");
    assert(stary.getStatus() == "ARCHIWUM");
    std::vector<Wynik> wyniki{Wynik("Adam", 100, "KLASYCZNY", 1, 0, 20, "PRZEGRANA"),
        Wynik("Ola", 200, "KLASYCZNY", 1, 0, 10, "PRZEGRANA"),
        Wynik("Jan", 100, "KLASYCZNY", 1, 0, 10, "PRZEGRANA")};
    std::sort(wyniki.begin(), wyniki.end());
    assert(wyniki[0].getGracz() == "Ola" && wyniki[1].getGracz() == "Jan");
    std::ostringstream rekord;
    rekord << wyniki[0];
    assert(Wynik::zLinii(rekord.str()).getPunkty() == 200);
    assert(!Wynik::poprawnyNick("A B") && !Wynik::poprawnyNick("") && Wynik::poprawnyNick("Gracz_1"));
    oczekujBledu([&] { Wynik::zLinii("Adam 100 KLASYCZNY 1 zle"); });
    std::cout << "OK: 500 losowan, bezpieczny start, flagi, sasiedzi, zalewanie, wygrana/przegrana, punkty, seria, zapis i ranking.\n";
}
