Zadanie 1 - Core Gunplay

Jak uruchomic:
  repo:
zainstalowany git lfs w celu obslugi duzych plikow
  projekt:
Wersja silnika 5.6.1
po otworzeniu projektu nalezy skompilowac pliki c++

Sterowanie:
WASD lub strzalki - poruszanie sie postaci
1 - wyciagniecie/schowanie pistoletu
2 - wyciagniecie/schowanie karabinu
LPM - strzal
PPM - zmiana trybu strzelania
R - przeladowanie

Projekt zostal przygotowany jako podstawowy i mozliwy do rozszerzenia system broni w silniku unreal engine 5.6.
Zalozenia:
Gra pierwszoosobowa
Każda bron uzywana przez gracza bazuje na klasie WeaponBase
wszystkie parametry broni sa w pelni edytowalne, bronie moga miec wiele trybow strzelania
Bronie posiadaja systemy rozrzutu oraz prostego odrzutu
Pociski sa osobnymi aktorami, dziedzicza parametry takie jak obrazenia czy szybkosc z broni
Projekt posiada proste ui oraz debug log ktory pozwala sprawdzic, w co dokladnie trafil pocisk, obecny stan amunicji oraz obecny tryb strzelania

Ograniczenia:
Projekt nie posiada systemu ekwipunku, bronie sa dodane recznie
brak animacji broni
system obrazen jest prosty

Podział C++/Blueprint:
C++:
bazowa logika broni
systemy strzelania, rozrzutu, odrzutu oraz przeladowania
logika pociskow
obsluga trafien oraz obrazen

Blueprint:
konfiguracje konkretnych broni
spawnowanie broni gracza
przelaczanie miedzy broniami
obsluge wejsc przez Enchanced Input
prosty hud
