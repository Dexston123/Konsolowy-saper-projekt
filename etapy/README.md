# Etapy aplikacji

Kazdy folder zawiera samodzielna wersje programu, z wlasnym kodem,
CMakeLists.txt i instrukcja kompilacji. Nie lacz plikow z roznych wersji
w jednym poleceniu kompilacji.

Foldery powstaly przez podzial istniejacej aplikacji na kolejne dzialajace
zakresy. Najnowsza wersja znajduje sie tez w glownym katalogu repozytorium.

- [Menu i ładowanie](01_menu_i_ladowanie/README.md): Menu, zasady gry, sprawdzanie wejscia i siedmiosekundowe ladowanie z miganiem napisu.

## Uruchomienie wybranego etapu

Przejdz do jego folderu, a nastepnie wykonaj polecenie podane w README.
Kazdy etap korzysta z danych w katalogu roboczym, z ktorego go uruchomiono.
Wersje z ustawieniami najlepiej uruchamiac z ich wlasnych folderow,
aby nie wspoldzielily plikow danych.
