#include "Gra.h"
#include <exception>
#include <iostream>

int main() {
    try {
        Gra gra;
        return gra.uruchom();
    } catch (const std::exception& blad) {
        std::cerr << "Blad uruchomienia: " << blad.what() << '\n';
        return 1;
    }
}
