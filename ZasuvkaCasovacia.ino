// Tento program ma za ulohu vypnut pozadovane zariadenie po nastavenom case (casovacia/spinacia zasuvka)
// Zariadenie sa vypne za pomoci rele, ktore rozpoji obvod v zasuvke a tym odstavi privod elektrickej energie do zariadenia
// Po zapnuti arduina sa pomocou rotacneho encodera nastavy pozadovany cas, ktory sa zobrazuje na 7 segmentovom display.
// Cas sa potvrdi stlacenim tlacitka v rotacnom encodery, cim sa zacne odpocet tohto casu, ktory sa priebezne zobrazuje na display.
// Po skonceni odpoctu sa vysle signal do rele (nastavy sa napatie na riadacom pine na hodnotu 5V) a to nasledne rozopne kontakty,
// na co sa pripojene zariadenie vypne. Po uplynuti dalsich 2 sekund sa vymaze obsah displayu.
// Pre opatovne spustenie zariadenia je potrebne restartovat arduino.

// Pripojenie kniznic
#include <Arduino.h>
// Tieto 2 je potrebne stiahnut z Library Managera
#include <TM1637Display.h>  // Kniznica pre jednoduchsiu pracu so 7 segmentovym displayom
#include <SimpleEncoder.h>  // Kniznica pre jednoduchsiu pracu s rotacnym encoderom


// Definovanie pinov
// Definovanie cisla pinov, na ktorych je pripojeny display
const int CLK = 7;
const int DIO = 8;
// Definovanie cisla pinov, na ktorych je pripojeny encoder
const int BTN = 6;      // Button
const int encA = 10;
const int encB = 11;
// Definovanie cisla pinu, na ktorom je pripojene rele
const int relayPin = 9;


// Vytvorenie objektu pre Display, 
// ako parameter sa jej predavaju cisla pinov na ktorych je pripojeny display
TM1637Display display(CLK, DIO);

// Vytvorenie objektu pre Encoder, 
// ako parameter sa mu predavaju cisla pinov na ktorych je pripojeny encoder
SimpleEncoder encoder(BTN, encA, encB);


// Definovanie premennych
int time = 5;   // Premenna, v ktorej je ulozeny cas, po ktorom sa ma pripojene zariadenie vypnut
bool start = false;   // Pomocna premenna, ked sa nastavy na "true", zacne sa odpocet


// Funkcia "setup", vykona sa po zapnuti zariadenia len jeden krat
// nastavia sa v nej potrebne veci
void setup() 
{
    display.setBrightness(7);     // Nastavy jas displayu na 7 (rozsah 0-10)
    pinMode(relayPin, OUTPUT);    // Nastavy pin "relayPin" ako vystupny, je nan pripojene rele
    digitalWrite(relayPin, LOW);  // Nastavy vystupne napatie pinu, na ktorom je pripojene rele na hodnotu 0V
}


// Funkcia "loop", vykonava sa po skonceni funkcie "setup" v nekonecnej slucke az do vypnutia arduina
void loop()
{

  if (!start)   // Vykonava sa ak este nebolo stlacene tlacitko
  {
    if (encoder.CLOCKWISE)  // Ak sa otaca s encoderom v smere hodinovych ruciciek
      ++time; // Pripocita k casu 1
    if (encoder.COUNTERCLOCKWISE) // Ak sa otaca s encoderom v protismere hodinovych ruciciek
      --time; // Odpocita z casu 1
    if (encoder.BUTTON_PRESSED)  // Ak je stlacene tlacitko
      start = true;   // Nastavy hodnotu premennej "start" na true -> zacne sa odpocet
   
    // Vypise na display hodnotu casu, po ktorom sa ma vypnut zariadenie.
    // Parameter s hodnotou "(0x80 >> 1)" znamena, ze sa rozsvieti bodka (dvojbodka) 
    // patriaca k 2. cislici na display ("1" je tam preto, lebo cislice su znacene od nuly).
    // Parameter s hodnotou "true" znamena, ze nepouzite cisla na display budu mat hodnotu "0"
    display.showNumberDecEx(time, (0x80 >> 1), true);  // Expect: 0001
  }

  else  // Vykonava sa ak sa ma zacat odpocet, tzn ak premenna "start" ma hodnotu true
  {
    if (time) // Vykona sa ak je hodnota casu v premennej "time" kladnej hodnoty
    {
      --time;   // Odpocita 1 od aktualnej hodnoty casu

      // Vypise aktualnu hodnotu casu, ktory zostava do vypnutia zariadenia na display.
      display.showNumberDecEx(time, (0x80 >> 1), true);  

      delay(1000);  // Pocka 1000 milisekund (1000ms = 1s)

      if (!time)  // Vykona sa ak je Cas rovny nule
      {
        digitalWrite(relayPin, HIGH);   // Vypne rele -> nastavy vystupne napatie pinu, na ktorom je pripojene rele na hodnotu napajacieho napatia tj. 5V
        delay(2000);                    // Pocka 2 sekundy
        display.clear();                // Vymaze obsah displayu
      }  
    }  
  }
}