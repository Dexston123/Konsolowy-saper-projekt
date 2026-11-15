#pragma once

#include <string>

class TrybGry {
public:
    virtual ~TrybGry() = default;
    virtual std::string nazwa() const = 0;
    virtual std::string opis() const = 0;
};

class TrybKlasyczny : public TrybGry {
public:
    std::string nazwa() const override;
    std::string opis() const override;
};

class TrybNieskonczony : public TrybGry {
public:
    std::string nazwa() const override;
    std::string opis() const override;
};
