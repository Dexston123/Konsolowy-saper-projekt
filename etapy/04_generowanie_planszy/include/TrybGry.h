#pragma once

#include "Plansza.h"
#include "Ustawienia.h"
#include <string>

class TrybGry {
protected:
    Plansza plansza;
    int numerPlanszy = 0;

public:
    explicit TrybGry(const Ustawienia& ustawienia);
    virtual ~TrybGry() = default;
    virtual std::string nazwa() const = 0;
    virtual std::string opis() const = 0;
    virtual void przygotuj(std::mt19937& generator);
    const Plansza& getPlansza() const;
    int getNumerPlanszy() const;
};

class TrybKlasyczny : public TrybGry {
public:
    explicit TrybKlasyczny(const Ustawienia& ustawienia);
    std::string nazwa() const override;
    std::string opis() const override;
    void przygotuj(std::mt19937& generator) override;
};

class TrybNieskonczony : public TrybGry {
public:
    explicit TrybNieskonczony(const Ustawienia& ustawienia);
    std::string nazwa() const override;
    std::string opis() const override;
    void przygotuj(std::mt19937& generator) override;
};
