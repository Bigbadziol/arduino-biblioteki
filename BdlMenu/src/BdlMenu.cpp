
#include "BdlMenu.h"


/*--------------------------------------------------------------------------------------
 *  Konstruktor naszej klasy. Ustawienie istotnych danych dla nowo tworzonego obiektu
 * -------------------------------------------------------------------------------------
 */
cMenu::cMenu() {
  Serial.println("MENU v 1.0.126");
  byte i;
  kursor.pozycja = 0;
  kursor.ps = 0;
  _kursorZapetlaj = false;
  _kursorSciezka = true;
  for (i = 0; i < sizeof(kursor.sciezka); i++) kursor.sciezka[i] = 0;
  _rozStrukt = sizeof(pozycja);
  _rozTab = sizeof(menuDane);

  for (i = 0; i < MENU_DANE_ROZMIAR; i++) {
    menuDane[i] = {"", 0, 0, NULL};
    aktywneMenu[i] = {"", 0, 0, NULL};
  }
  menuDane[0] = {"ROOT", 0, 0, NULL};
  _ilePozAktywneMenu = 0;
};
/*-----------------------------------------------------------------------------------
   Dodawanie nowej pozycji menu/submenu
   Idea jest taka , ze urzytkownik tworzy unikalny identyfikator skladajacy sie z
   rodzica i wlasnego id
   wezel startowy to ROOT[0][0],do ktorego urzytkownik nie powinien miec dostepu
   Przykladowo pozycje Pozycja_submenu_1[10][97] i Pozycja_submenu_2 [10][156] beda
   nalezaly do menu/submenu o identyfikatorze [10]
   MenuABC[0][10] jest glownym menu bo nalezy do ROOT,czyli wezla startowego
 ------------------------------------------------------------------------------------  
*/
void cMenu::Dodaj(String nazwa, int r_id, int m_id, void (*akcja)()) {
  int ostPoz; //ostatnia pozycja;
  ostPoz = ZliczPozycje();
  menuDane[ostPoz + 1].nazwa = nazwa;
  menuDane[ostPoz + 1].r_id = r_id;
  menuDane[ostPoz + 1].m_id = m_id;
  menuDane[ostPoz + 1].akcja = akcja;
};
/*-----------------------------------------------------------------------
   Uzytkownik ma mozliwosc dodawania pozycji w dowolnej kolejnosci
   Aby wszystko mialo rece i nogi sortujemy tablice najpierw po r_id, m_id.
   Pamietamy!! r_id w nastepnej instancji staje sie m_id;!!
   -----------------------------------------------------------------------
*/
void cMenu::Sortuj() {
  bool zamiana;
  int p = 1;
  int i;
  int j;
  int poz;
  sMenuPoz tmpMenuPoz;
  //Najpierw sortujemy po rodzicach
  poz = ZliczPozycje();
  for (j = 0; j < poz - 1; j++)
  {
    for ( i = 0; i < poz; i++)
    {
      if ((menuDane[i].r_id) > (menuDane[i + 1].r_id))
      {
        tmpMenuPoz = menuDane[i + 1];
        menuDane[i + 1] = menuDane[i];
        menuDane[i] = tmpMenuPoz;
      };
    };
  };
  //moje id - pozycji
  for (j = 0; j < poz - 1; j++)
  {
    for ( i = 0; i < poz; i++)
    {
      if ((menuDane[i].r_id) == (menuDane[i + 1].r_id))
      {
        if ((menuDane[i].m_id) > (menuDane[i + 1].m_id))
        {
          tmpMenuPoz = menuDane[i + 1];
          menuDane[i + 1] = menuDane[i];
          menuDane[i] = tmpMenuPoz;
        };//moje id
      };//rodzicid
    };//i
  };//j
};
/* -----------------------------------------------------------------------
    Do testow i kontroli co gdzie i jakie wartosci siedza
   ------------------------------------------------------------------------
*/
void cMenu::ZrzutDane() {
  int i;
  _info = " --> Rozmiary struktura/tablica/pozycje :" + String(_rozStrukt) + " , " +
          String(_rozTab) + " , " + String(MENU_DANE_ROZMIAR);
  Serial.println(_info);
  Serial.println("--> ZAWARTOSC TABELI menuDANE");
  for (i = 0; i < MENU_DANE_ROZMIAR ; i++) {
    _info = "DANE(" + String(i) + ")->" + menuDane[i].nazwa +
            "   R.ID:" + String(menuDane[i].r_id) +
            "   M.ID :" + String(menuDane[i].m_id) +
            "  AKCJA: ";//+String(menuDane[i].akcja*);
    Serial.println(_info);
  };
  _info = "Faktycznie pozycji w menu:" + String(ZliczPozycje());
  Serial.println(_info);
  Serial.println();
};

/*------------------------------------------------------------------------
  Funkcja pomocnicza przy rysowaniu menu, dolepia przed nazwa odpowiednia
  ilosc znakow;
 -----------------------------------------------------------------------
*/
String cMenu::Prefix(String prefix, int ile) {
  String wynik;
  for (int i = 0; i < ile; i++) wynik = wynik + prefix;
  return wynik;
};
/*------------------------------------------------------------------------
Funkcja pomocnicza przy rysowaniu menu, pobiera dane mieszczace sie w 
aktywnym menu i aktualnej pozycji kursora
  -----------------------------------------------------------------------
*/
String cMenu::KursorWypisz(int poz) {
  return "[" + String(aktywneMenu[poz].r_id) +
         "][" + String(aktywneMenu[poz].m_id) + "]   " + aktywneMenu[poz].nazwa;
};
/*-----------------------------------------------------------------------
    Odrysowanie strukture menu w czytelny sposob na monitorze USB,
    zdefiniowane prez urzytkownika
   ----------------------------------------------------------------------
*/
void cMenu::Rysuj() {
  int wypisano = 0;
  int glebokosc = 0;
  byte ile = 0;
  bool ruchPrzod;
  bool ruchDol;
  bool ruchCofnij;
  bool wypisac;
  bool tmpKursorSciezka;
  bool tmpKursorZapetlaj;
  String pre = "---";
  ile = AktywujMenu(0);
  tmpKursorSciezka = _kursorSciezka;
  tmpKursorZapetlaj = _kursorZapetlaj;
  _kursorSciezka = true; //zmienna prywatna;
  _kursorZapetlaj = false;

  if (ile > 0 ) { // czy menu glowne puste?
     Serial.println("Struktura menu posiada : "+String(ZliczPozycje())+ " pozycji");
     
    //przypadek szczegolny wypisanie pierwszej pozycji.
    _info = Prefix(pre, glebokosc) + KursorWypisz(kursor.pozycja);
    Serial.println(_info);
    wypisano++;
    while (wypisano < ZliczPozycje()) { //Powtazaj dopoki nie wypsze sie wszystkich pozycji menu
      wypisac = false;
      //probojemy zaglebiac sie w menu, isc do porzodu;
      ruchPrzod = KursorPrzod();
      if (ruchPrzod == true) { //probojemy isc do przodu
        glebokosc++;
        wypisac = true;
      }
      else { //nie mozemy do przodu , probojemy w dol;
        ruchDol = KursorDol();
        if (ruchDol == true) {

          wypisac = true;
        } else { //nie mozemy isc w dol , zatem trzeba sie cofnac i przesunac w dol
          glebokosc--;
          ruchCofnij = KursorCofnij();
          ruchDol = KursorDol();
          wypisac = true;
           //WARUNEK DODny na pal;
            if (ruchDol == false) {// jestesmy niejako na ostatniej pozycji danego submenu , wiec sie cofamy o poziom 
                    glebokosc--;
                    KursorCofnij(); 
                    KursorDol();
            };
        };//cofam kursor i przesuwam sie w dol
      };//nie mozemy do przodu , probojemy w dol.
      
      if (wypisac == true) {
        _info = Prefix(pre, glebokosc) + KursorWypisz(kursor.pozycja);
        Serial.println(_info);
        wypisano++;
      };//wypisanie kursora
    };// petla glowna wypisywania wszystkich pozycji
  } else Serial.println("UWAGA, menu puste!");
  _kursorZapetlaj = tmpKursorZapetlaj;
  _kursorSciezka = tmpKursorSciezka;
  kursor.sciezka[0]=0;
  kursor.pozycja=0;
};
/*------------------------------------------------------------------------
   Zwraca ilosc faktycznych danych na ktorych bedziemy operowac.
   Ignorujemy pozycje[0][0]ROOT oraz wszystkie puste pozycje w tabeli menuDane
   ------------------------------------------------------------------------
*/
int cMenu::ZliczPozycje() {
  int i;
  int p = 0;
  for (i = 0; i < MENU_DANE_ROZMIAR; i++) {
    if (menuDane[i].nazwa != "ROOT" && menuDane[i].nazwa != "") p++;
  };
  return p;
};
/*------------------------------------------------------------------------
   Generalnie sprawdzamy czy nadrzedne menu jest ROOTEM czy nie
   - 0 to root , 1- zwykle menu /submenu
   ------------------------------------------------------------------------
*/
int cMenu::IleNadMenu(byte kur) {
  byte wynik;
  if ((aktywneMenu[kur].r_id == 0)  && (aktywneMenu[kur].m_id==0)) wynik = 0;
  else wynik = 1;
  /*
    _info="KURSOR :"+String(kursor)+"   "+aktywneMenu[kursor].nazwa+"   NAD MENU:"+String(wynik);
    Serial.println(_info);
  */
  return wynik;
} ;
/*-------------------------------------------------------------------------
   Czy dana pozycja menu/ sub-menu zawiera jeszcze jakies podmenu,
   jesli tak zwraca ilosc pozycji jaka zawiera
   ------------------------------------------------------------------------
*/
int cMenu::IlePodMenu(byte kur) {
  int i;
  int wynik = 0;
  int p = aktywneMenu[kur].m_id;
  for (i = 0; i < ZliczPozycje(); i++) {
    if (menuDane[i].r_id == p) wynik++;
  };
  /*
    _info="KURSOR : "+String(kursor)+"   "+aktywneMenu[kursor].nazwa+ "  POD MENU:"+String(wynik);
    Serial.println(_info);
  */
  return wynik;
};
/* ------------------------------------------------------------------------------
    Ustawienie parametrow kursora
   zapetlanie - true (po osiagnieciu ostatniej pozycji przeskakujemy na 1)
   sciezka (true) po powrocie do menu nadrzednego ustawiamy sie w ostatniej pozycji.
  -------------------------------------------------------------------------------
*/
void cMenu::KursorUstaw(bool zapetl , bool sciezka) {
  _kursorZapetlaj = zapetl;
  _kursorSciezka = sciezka;
};
/* ---------------------------------------------------------------------
   Podstwowe informacje o kursorze do dziala jak Zrzut() , zrzuca dane o kursorze
   ------------------------------------------------------------------------
*/
void cMenu::ZrzutKursor() {
  Serial.println();
  Serial.println("---> INFORMACJA O KURSORZE");
  _info = "->Pozycja w aktywnym menu : " + String(kursor.pozycja);
  Serial.println(_info);
  _info = "->Pozycja w sciezce : " + String(kursor.ps);
  Serial.println(_info);
  _info = "->Zawartosc sciezki : ";
  Serial.println(_info);
  _info = "";
  for (int i = 0; i < sizeof(kursor.sciezka); i++) {
    if (i == kursor.ps) _info = _info + "[x]";
    else _info = _info + "[ ] ";
  }
  Serial.println(_info);
  _info = "";
  for (int i = 0; i < sizeof(kursor.sciezka); i++) {
    _info = _info + "[" + String(kursor.sciezka[i]) + "] ";
  }
  Serial.println(_info);

  _info = "-->NAZWA : " + aktywneMenu[kursor.pozycja].nazwa +
          "-->NAD MENU :" + String(IleNadMenu(kursor.pozycja)) +
          "-->POD MENU :" + String(IlePodMenu(kursor.pozycja));
  Serial.println(_info);
};
/*-------------------------------------------------------------------------
   Metod tworzy aktywne menu/submenu kopiujac dane do tabeli aktywneMenu
   Zwraca ilosc skopiowanych pozycji
   ------------------------------------------------------------------------
*/
int cMenu::AktywujMenu(int id) {
  int i;
  int j = 0;

  for (i = 0; i < MENU_DANE_ROZMIAR; i++) {
    aktywneMenu[i] = {"", 0, 0, NULL};
  };

  for (i = 0; i <= ZliczPozycje()  ; i++)
  {
    if (menuDane[i].r_id == id) {
      if (menuDane[i].nazwa != "ROOT" && menuDane[i].nazwa != "" && menuDane[i].m_id > 0  ) {
        aktywneMenu[j] = menuDane[i];
        j++;
      };//wykluczenie roota
    }; //znaleziono pozycje
  };//for
  _ilePozAktywneMenu = j;
  kursor.pozycja = 0;
  return j;
};
/* ------------------------------------------------------------------------
   Zrzut zawarosci tabeli aktywneMenu;
  ------------------------------------------------------------------------
*/
void cMenu::ZrzutAktywneMenu() {
  Serial.println();
  _info = "--->AKTYWNE MENU";
  Serial.println(_info);
  Serial.println("[LP][RID][MID][NAZWA]");
  for (int i = 0; i < _ilePozAktywneMenu; i++) {
    _info = "lp." + String(i) + "   [" + String(aktywneMenu[i].r_id) + "][" +
            String(aktywneMenu[i].m_id) + "]   " + aktywneMenu[i].nazwa;
    Serial.println(_info);
  };
};
/* ------------------------------------------------------------------------
   Obsluga przesuniecia kursora w dol w tablicy aktywneMenu
   -----------------------------------------------------------------------
*/
bool cMenu::KursorDol() {
  byte aktualnaPozycja = kursor.pozycja;
  bool zmieniono = false;
  if (_ilePozAktywneMenu > 0 ) // czy menu ma pozycje?
  {
    switch (_kursorZapetlaj) {
      case true:
        if ((aktualnaPozycja + 1) > _ilePozAktywneMenu) {
          kursor.pozycja = 0;
        }
        else {
          kursor.pozycja++;
        }
        zmieniono = true;
        break;
      case false:
        if ((aktualnaPozycja + 1) < _ilePozAktywneMenu) {
          kursor.pozycja++;
          zmieniono = true;
        };
        break;
    };
    kursor.sciezka[kursor.ps] = kursor.pozycja;
  };
  return zmieniono;
};
/*------------------------------------------------------------------------
   Obsluga przesuniecia kursora w gore w tablicy aktywneMenu
   ------------------------------------------------------------------------
*/
bool cMenu::KursorGora() {
  byte aktualnaPozycja = kursor.pozycja;
  bool zmieniono = false;
  if (_ilePozAktywneMenu > 0 ) // czy menu ma pozycje?
  {
    switch (_kursorZapetlaj) {
      case true:
        if ((aktualnaPozycja - 1) < 0)
          kursor.pozycja = _ilePozAktywneMenu;
        else kursor.pozycja--;
        zmieniono = true;
        break;
      case false:
        if ((aktualnaPozycja - 1) >= 0) {
          kursor.pozycja--;
          zmieniono = true;
        };
        break;
    };
    kursor.sciezka[kursor.ps] = kursor.pozycja;
  };
  return zmieniono;
};
/*-------------------------------------------------------------------------
 *  Obsluga przesuniecia kursora w gore w tablicy aktywneMenu
 *  ------------------------------------------------------------------------
*/
bool cMenu::KursorPrzod() {
  bool zmieniono = false;
  if (IlePodMenu(kursor.pozycja) > 0)
  {
    AktywujMenu(aktywneMenu[kursor.pozycja].m_id);
    kursor.ps++;
    kursor.sciezka[kursor.ps] = 0;
    zmieniono = true;
  };
  return zmieniono;
};
/*--------------------------------------------------------------------------
 *   Funkcja szuka id nadrzednego menu dla aktualnie aktywnego
 *--------------------------------------------------------------------------
 */
int cMenu::MenuNadrzedneId (int idm) {
  int wynik = 0;
  for (int i = 0; i < MENU_DANE_ROZMIAR; i++) {
    if (menuDane[i].m_id == idm) wynik = menuDane[i].r_id;
  };
 // _info = "---> F.Menu nadrzedne -> Dostalem id : " + String(idm) + " Zwracam nadrzedne id : " + String(wynik);
 // Serial.println(_info);
  return wynik;
};
/*--------------------------------------------------------------------------
 * Przesuniecie kursora do menu nadrzednego
 *--------------------------------------------------------------------------
 */
bool cMenu::KursorCofnij() {
  bool zmieniono = false;
  int nad_id; //nadrzedne id;
  int inm;
  //inm = IleNadMenu(kursor.pozycja);
  //Serial.println("Ile NAD Menu :"+String(inm));
  
  if (IleNadMenu(kursor.pozycja) > 0) { // czy nie jestesmy ROOTEM;
    nad_id  = MenuNadrzedneId(aktywneMenu[kursor.pozycja].r_id);
    kursor.sciezka[kursor.ps] = 0;
    kursor.ps--;
    
    AktywujMenu(nad_id);
    if (_kursorSciezka == true) kursor.pozycja = kursor.sciezka[kursor.ps];
    else kursor.pozycja = 0;
    zmieniono = true;
  }
  return zmieniono;
};
/*-------------------------------------------------------------------------
 * Wywolanie metody zdefiniowanej przez urzytkownika
 * ------------------------------------------------------------------------
 */
bool cMenu::KursorAkcja() {
    if(aktywneMenu[kursor.pozycja].akcja != NULL)
    {
      (*(aktywneMenu[kursor.pozycja].akcja))();
    };
};

