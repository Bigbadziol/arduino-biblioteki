#include "BdlMenu.h"
//=========================================================================
void akcjaJeden() {
  Serial.println("Fajna akcja 1");
};
void akcjaDwa(){
 Serial.println("Fajna akcja 2");
}
void akcjaTrzy(){
  Serial.println("Fajna akcja 3");
}
//-------------------------------------------------------------------------
void setup() {

  Serial.begin(9600);
  cMenu Menu;
  Menu.KursorUstaw(false,true); //nie zapetlaj, pamietaj sciezke
  Menu.Dodaj("Pod menu 1-1F.3?", 108, 5, NULL);
  Menu.menuDane[2] = {"POZIOM 1-1", 0, 10, akcjaJeden};
  Menu.menuDane[3] = {"POZIOM 1-3", 0, 30, NULL};
  Menu.menuDane[4] = {"POZIOM 1-2", 0, 20, NULL};
  Menu.Dodaj("Pod menu 1-1A.2", 101, 1012, NULL);
  Menu.Dodaj("Kol. podmenu",103,1031,NULL);
  Menu.Dodaj("Pod menu 1-1A.3",101,1013,NULL);
  Menu.Dodaj("PODMENU 1-1 F", 10, 108, NULL);
  Menu.Dodaj("PODMENU 1-1 A", 10, 101, NULL);
  Menu.Dodaj("POZIOM xx",108,1086,NULL);
  Menu.Dodaj("PODMENU 1-1 C", 10, 103, NULL);
  Menu.Dodaj("Pod menu 1-1F.1", 108, 1081, NULL);
  Menu.Dodaj("Pod menu 1-1F.5", 108, 1085, NULL);
  Menu.Dodaj("Pod menu 1-1A.1", 101, 1011, akcjaDwa);
  Menu.Dodaj("POZIOM 1-2.5", 0, 25, akcjaTrzy);
  Menu.Dodaj("POD POZIOM 1-2.5", 25, 156, NULL);
  Menu.Sortuj();
  Menu.ZrzutDane();
  Menu.Rysuj();
  Menu.ZrzutAktywneMenu();
  Menu.ZrzutKursor();
  
  Menu.KursorAkcja();
  Menu.KursorPrzod();
  Menu.KursorPrzod();
  Menu.KursorAkcja();
  Menu.KursorCofnij();
  Menu.KursorCofnij();
  Menu.KursorDol();
  Menu.KursorDol();
  Menu.KursorAkcja();
}
//-----------------------------------------------------------------------
void loop() {


};
