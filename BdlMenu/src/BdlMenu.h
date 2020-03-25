/* BdlMenu 
   Najwazniejsze informacje:
   - Klasa nie obsluguje wielu wyjatkow
   - Nie ruszac pozycji [obiekt].menuDane[0] , zarezerwowana dla ROOTA
   - Nie tworzyc menu o nazwie ROOT i identyfikatorach r_id=0 ,m_id=0 , zarezerwowane dla...
   - m_id musi byc wartoscia unikalna , taki klucz
   - Nazwa nie moze byc pusta
   - Pozycje menu mozna dodawac w dowolnej kolejnosci , lecz na koncu trzeba wywolac metode sortuj.
   - Zalecane dodawanie przez metode Dodaj(String nazwa, int r_id, int m_id, void (*akcja)()).
     Jesli dana pozycja menu nie ma nic robic void (*akcja)() ustawiamy na NULL w przeciwnym wypadku
     metoda zdefiniowana przez uzytkownika;
   - Przy tworzeniu menu glownego 1-szego poziomu r_id ustawiamy na 0 bo sa one niejako pochodne ROOTA...,
     dalej wg. fantazji
   Metody najczesciej urzywane:
      * Dodaj(...)- dodanie pozycji menu;
      * Sortuj() - posortowanie dodanych pozycji
      * Rysuj()- odrysowanie menu w monitorze USB zdefiniowanego przez urzytkownika
      * ZrzutDane() , ZrzutAktywneMenu() , ZrzutKursor() - co gdzie siedzi
      * AktywujMenu (id) - aktywacja menu o okreslonym id
      * KursorUstaw ( bool zapetl , bool sciezka) , czy zapetlac pozycje po osiagnieciu skrajnej w menu (np. przeskok z ostatniej 
          na pierwsza czy nie . Czy pamietac sciezke jak w rozwijanych oknach ? Domyslnie FALSE i TRUE
      * KursorGora(), KursorDol(), KursorPrzod(), KursorCofnij(),KursorAkcja() - typu bool zwracaja true jak sie udalo
          same aktywuja odpowiednie menu jak trzeba;    
*/
#ifndef BdlMenu_h
#define BdlMenu_h
#include "Arduino.h"

class cMenu {
    static const int MENU_DANE_ROZMIAR = 20;
    //enum RUCH {GORA, DOL, PRZOD, COFNIJ, AKCJA};
    struct sMenuPoz {
      String nazwa; //nazwa pozycji menu
      int r_id; // id rodzica/nadmenu, dla menu pierwszego poziomu rodzicem zawsze jest Root [0]
      int m_id; // identyfikator danej pozycji menu / submenu
      void (*akcja)(); //do wywolania metody zdefiniowanej przez urzytkownika
    };
    struct sKursor {
      byte pozycja; //aktualna pozycja w aktywnym menu
      byte sciezka[5]; //chcemy by kursor cofajac sie znajdowal sie w poprzednim miejscu jak w rozwijanych oknach;
      byte ps; //pozycja w sciezce, 0-menu glowne , 1-submenu poziomu pierwszego , 2-submenu poziomu drugiego,itd;
    };
  public:
    sKursor kursor; // pozycja w aktywnym menu;
    cMenu();//konstruktor
    sMenuPoz pozycja; // tu skladowane sa dane aktywnej pozycji menu na ktorej znajduje sie kursor
    sMenuPoz menuDane[MENU_DANE_ROZMIAR]; //zawiera definicje wszystkich sub-menu
    sMenuPoz aktywneMenu[MENU_DANE_ROZMIAR];// zawiera pozycje aktywnego menu
    void Dodaj(String nazwa, int r_id, int m_id, void (*akcja)()); // opiane szczegolowo w naglowku.
    int ZliczPozycje(); // zlicza faktyczna ilosc dodanych pozycji do tabeli "menuDane"
    void Sortuj();//sortowanie tabeli menuDane,czyli wszystkich pozycji po rodzicu(nadmenu),nastepnie po id pozycji
    void ZrzutDane(); //zrzut istotniejszych danych przez USB danych do monitora.
    void ZrzutAktywneMenu(); //zrzut zawartosci tabeli aktywne menu;
    void Rysuj();// odrysowanie menu na monitorze zdefiniowanego menu
    int IleNadMenu(byte kur); // zwraca 0 jesli poziom wyzej  ROOT , 1 - menu entego poziomu
    int IlePodMenu(byte kur); // zwraca liczbe submenu dla kursora w aktywnym menu .
    int AktywujMenu(int id); //aktywuje menu o okreslonym kluczu

    void KursorUstaw ( bool zapetl , bool sciezka); //kursor zapetla sie w menu czy zostaje na skrajnej poz. , Kursor po cofnieciu wraca sciezka czy londuje na 1 poz.
    // domslnie zapetlanie FALSE , SCIEZKA true);
    bool KursorGora(); //przesun kursor w aktywnym menu w gore, TRUE jak sie uda
    bool KursorDol(); // j.w.
    bool KursorPrzod(); //j.w.
    bool KursorCofnij();//j.w
    bool KursorAkcja();//obsluga metody zdefiniowanej przez urzytkownika.
    void ZrzutKursor(); // zwraca informacje testowe do seriala.

  private:
    String _info; //wysylanie komunikatow na monitor(USB)
    int _rozStrukt; //zm. wew. przechowuje ile pamieci zajmuje strukt. menuPoz;
    int _rozTab;    //zm. wew. przechowuje ile zajmuje tablica opisujaca dane pozycji menu
    int _ilePozAktywneMenu; //zm. wew. ile faktycznie pozycji jest w aktywnym menu;
    bool _kursorZapetlaj; // Czy po osiagniciu maksymalnej pozycji w menu wrocic do pierwszej czy nie.
    bool _kursorSciezka;// TRUE , pamieta ostatnia pozycje jak w okienkach , FALSE , po powrocie wraca zawsze na pierwsza
    // Czy po osiagnieciu minimalnej pozycji przejsc do maxymalnej.
    int MenuNadrzedneId (int idm); // Wyszukaj id menu nadrzednego w stosunku do aktywnego menu
    String Prefix(String pref , int ile); //funkcja pomocnicza do funkcji rysuj ,mnozy ciag znakow "ile" razy;
    String KursorWypisz(int poz); // funkcja pomocnicza do funkcji rysuj; na podst. pozycji w aktualnym menu , wypisuje zawartosc;
};
#endif
