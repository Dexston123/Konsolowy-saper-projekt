#pragma once

#include "Plansza.h"
#include "Ustawienia.h"
#include <memory>
#include <string>

class TrybGry {
protected:
    Plansza plansza;
    int numerPlanszy = 0;
    int poziom = 2;
    int wygranePlansze = 0;
    long long punktyZakonczonych = 0;
    long long czasMs = 0;
    std::string gracz;

public:
    TrybGry(const Ustawienia& ustawienia, const std::string& gracz);
    virtual ~TrybGry() = default;
    virtual std::string nazwa() const = 0;
    virtual std::string opis() const = 0;
    virtual bool czyKolejnaPlansza() const = 0;
    virtual void przygotuj(std::mt19937& generator);
    const Plansza& getPlansza() const;
    Plansza& getPlansza();
    int getNumerPlanszy() const;
    int getPoziom() const;
    int getWygranePlansze() const;
    int getSekundy() const;
    const std::string& getGracz() const;
    long long getPunkty() const;
    void dodajCzas(long long milisekundy);
    void zaliczPlansze();
    void zapisz(std::ostream& wyjscie) const;
    static std::unique_ptr<TrybGry> wczytaj(std::istream& wejscie);
};

class TrybKlasyczny : public TrybGry {
public:
    explicit TrybKlasyczny(const Ustawienia& ustawienia, const std::string& gracz = "Gracz");
    std::string nazwa() const override;
    std::string opis() const override;
    bool czyKolejnaPlansza() const override;
    void przygotuj(std::mt19937& generator) override;
};

class TrybNieskonczony : public TrybGry {
public:
    explicit TrybNieskonczony(const Ustawienia& ustawienia, const std::string& gracz = "Gracz");
    std::string nazwa() const override;
    std::string opis() const override;
    bool czyKolejnaPlansza() const override;
    void przygotuj(std::mt19937& generator) override;
};
