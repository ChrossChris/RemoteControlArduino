#include <Wire.h>  // I2C-Bus
#include <LiquidCrystal_I2C.h> 

// Relativpfad funktioniert leider nicht zum Einbinden
#include "C:/Dokumente/Elektronik & Modellbau/Fernsteuerung/Arduino/inc/definitions.h"
#include "C:/Dokumente/Elektronik & Modellbau/Fernsteuerung/Arduino/inc/multiplex_heron.h"
#include "C:/Dokumente/Elektronik & Modellbau/Fernsteuerung/Arduino/inc/dpower_streamline.h"
#include "C:/Dokumente/Elektronik & Modellbau/Fernsteuerung/Arduino/inc/multiplex_easyglider.h"
#include "C:/Dokumente/Elektronik & Modellbau/Fernsteuerung/Arduino/inc/graupner_amigo4.h"

LiquidCrystal_I2C lcdRight(DISPLAY_RIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT);  
LiquidCrystal_I2C lcdLeft (DISPLAY_LEFT,  DISPLAY_WIDTH, DISPLAY_HEIGHT);  

// -----------------------------------------------------------------------------------------------
// Tonausgabe
struct ToneSequence
{
  ToneSequence(int _freq, int _duration, int _pause)
  : frequency(_freq)
  , duration(_duration)
  , pause(_pause)
  {};
 
  int frequency = 0;
  int duration  = 0;
  int pause     = 0;
};
typedef ToneSequence TS;

unsigned long   timeBaseSound[SEQUENCE_LENGTH];
int             frequency[SEQUENCE_LENGTH];
int             tonDuration[SEQUENCE_LENGTH];
int             pulseIndex = 0;
// -----------------------------------------------------------------------------------------------

// Globale Variable zur Speicherung der anzuzeigenden Werte
int  joysticks[CHANNELS]              = {150};
int  joystickLimits[CHANNELS][MAX+1]  = {{0,0}};
int  expoQuer                = 0;
int  expoHoehe               = 0;
int  expoSeite               = 0;
int  flaps                   = 0;
int  querSeiteMischer        = 0;
int  querFlapsMischer        = 0;
int  flapHoeheMischer        = 0;
int  motrHoeheMischer        = 0;
int  trimmHoehe              = 0;
int  trimmQuer               = 0;
int  expoQuerFilter          = 0;
int  expoHoeheFilter         = 0;
int  expoSeiteFilter         = 0;
int  querSeiteMischerFilter  = 0;
int  querFlapsMischerFilter  = 0;
int  flapHoeheMischerFilter  = 0;
int  motrHoeheMischerFilter  = 0;
int  flapsFilter             = 0;
int  trimmHoeheFilter        = 0;
int  trimmQuerFilter         = 0;
byte remoteControlSetting    = 0;
byte statusScreen            = 0;
byte statusScreenOld         = 0;
byte accu                    = 0;
byte model                   = 0;
int  accuCells               = 0;
int  accuCapacity            = 0;
int  accuChargeLevel         = 0;
int  accuChargeLevelFilter   = 0;
int  accuDischargeTotal_mAh  = 0;
bool activeTone              = false;
unsigned int  runningTimeSec = 0;
unsigned int  throttleTotal  = 0;


void setup()
{
  // LCD-Display starten
  lcdRight.init(); 
  lcdRight.backlight(); 
  lcdLeft.init(); 
  lcdLeft.backlight();   
  
  // Serielle Schnittstelle zu Master-Arduino
  Serial.begin(BAUD_RATE);
  Serial.flush();
}


void loop()
{
  // Signaltöne für die unterschiedlichen akustischen Ausgaben
  const ToneSequence seq100[4]  = { TS( 700, 70, 30), TS( 800, 70, 30), TS( 900, 70, 30), TS(1000, 70, 30)};
  const ToneSequence seq600[4]  = { TS(1000, 70, 30), TS( 900, 70, 30), TS( 800, 70, 30), TS( 700, 70, 30)};
  const ToneSequence seq500[3]  = { TS(2500, 50,100), TS( 500,250,100), TS(2000, 50, 50)};
  const ToneSequence seq1000[3] = { TS( 750,450,150), TS(2000, 50,100), TS(2500, 50, 50)};
  const ToneSequence seq0000[7] = { TS( 750,450,150), TS(2000, 50,100), TS(2500, 50, 50), TS( 750,450,150), TS( 750,450,150), TS( 750,450,150), TS( 750,450,150)};

  // ----------------------------------------------------------------------------------
  // Datenstrom entsprechend Protokoll über die serielle Schnittstelle einlesen
  byte rxMsg[64] = {0};
  byte amount    = 0;

  byte rxByte = 0;
  while (1)
  {
    if (Serial.available() > 2)
    {
      rxByte = Serial.read();
      if ((rxByte == SERIAL_START) && (Serial.peek() == SERIAL_START)) 
      {
        Serial.read(); // Zweite Start-Byte verwerfen
        amount = Serial.read();
        break;
      }
    }
  }
  Serial.readBytes(rxMsg, amount);

  // ----------------------------------------------------------------------------------
  // Empfangene Daten verarbeiten und interpretieren
  byte idxMsg = 0;

  model                 = (rxMsg[idxMsg]   & B00000011) >> 0;
  accu                  = (rxMsg[idxMsg]   & B00011100) >> 2;
  statusScreen          = (rxMsg[idxMsg++] & B11100000) >> 5;  
  remoteControlSetting  = rxMsg[idxMsg++];

  throttleTotal           = (unsigned int)  word(rxMsg[idxMsg+0],rxMsg[idxMsg+1]);  
  accuDischargeTotal_mAh  = (int)           word(rxMsg[idxMsg+2],rxMsg[idxMsg+3]);
  idxMsg                 += 4;
  
  byte signs       = rxMsg[idxMsg+ 0];
  trimmHoehe       = rxMsg[idxMsg+ 1];
  expoHoehe        = rxMsg[idxMsg+ 2];
  trimmQuer        = rxMsg[idxMsg+ 3];
  expoQuer         = rxMsg[idxMsg+ 4];
  querSeiteMischer = rxMsg[idxMsg+ 5];
  expoSeite        = rxMsg[idxMsg+ 6];
  querFlapsMischer = rxMsg[idxMsg+ 7];
  flaps            = rxMsg[idxMsg+ 8];
  flapHoeheMischer = rxMsg[idxMsg+ 9];
  motrHoeheMischer = rxMsg[idxMsg+10];
  if (bitRead(signs,SIGNUM_TRIMM_HOEHE)         == 1)  trimmHoehe       = -trimmHoehe;
  if (bitRead(signs,SIGNUM_TRIMM_QUER)          == 1)  trimmQuer        = -trimmQuer;
  if (bitRead(signs,SIGNUM_QUER_SEITE_MISCHER)  == 1)  querSeiteMischer = -querSeiteMischer;
  if (bitRead(signs,SIGNUM_QUER_FLAPS_MISCHER)  == 1)  querFlapsMischer = -querFlapsMischer;
  if (bitRead(signs,SIGNUM_FLAPS)               == 1)  flaps            = -flaps;
  if (bitRead(signs,SIGNUM_FLAPS_HOEHE_MISCHER) == 1)  flapHoeheMischer = -flapHoeheMischer;
  idxMsg += 11;
 
  for (int idx = 0; idx < CHANNELS; idx++)  
  {
    joysticks[idx]           = (int) rxMsg[idxMsg+0] - 100;
    joystickLimits[idx][MIN] = (int) rxMsg[idxMsg+1] - 100;
    joystickLimits[idx][MAX] = (int) rxMsg[idxMsg+2] - 100;
    idxMsg += 3;
  }
  runningTimeSec  = (unsigned int) word(rxMsg[idxMsg+0],rxMsg[idxMsg+1]);

  // ----------------------------------------------------------------------------------
  // Ladezustand Akku
  if (accu == ACCU_4s_5500mAh) 
  {
    accuCells       = 4;
    accuCapacity    = ACCU_CAPACITY_TOTAL_4s_5500mAh;
    accuChargeLevel = 100L - ((accuDischargeTotal_mAh * 100L) / (9L*accuCapacity/10L)); // 90% Akkukapazität verfügbar bis entladen
    accuChargeLevel = max(accuChargeLevel, 0);
  }
  else if (accu == ACCU_4s_2400mAh) 
  {
    accuCells       = 4;
    accuCapacity    = ACCU_CAPACITY_TOTAL_4s_2400mAh;
    accuChargeLevel = 100L - ((accuDischargeTotal_mAh * 100L) / (9L*accuCapacity/10L)); // 90% Akkukapazität verfügbar bis entladen
    accuChargeLevel = max(accuChargeLevel, 0);
  }
  else if (accu == ACCU_3s_2200mAh)  
  {
    accuCells       = 3;
    accuCapacity    = ACCU_CAPACITY_TOTAL_3s_2200mAh;
    accuChargeLevel = 100L - ((accuDischargeTotal_mAh * 100L) / (9L*accuCapacity/10L)); // 90% Akkukapazität verfügbar bis entladen
    accuChargeLevel = max(accuChargeLevel, 0);
  }
  else if (accu == ACCU_3s_1800mAh)  
  {
    accuCells       = 3;
    accuCapacity    = ACCU_CAPACITY_TOTAL_3s_1800mAh;
    accuChargeLevel = 100L - ((accuDischargeTotal_mAh * 100L) / (9L*accuCapacity/10L)); // 90% Akkukapazität verfügbar bis entladen
    accuChargeLevel = max(accuChargeLevel, 0);
  }
  
  
  // ----------------------------------------------------------------------------------
  // LCD-Displays aktualisieren
  switch (statusScreen)
  {
    case 0:
      displayStatus(DISPLAY_LEFT);
      displayAddFunctionsGraphic(DISPLAY_RIGHT);
      break;
    case 1:
      displayAddFunctionsNumeric(DISPLAY_LEFT);
      displayAddFunctionsGraphic(DISPLAY_RIGHT);
      break;
    case 2:
      displayJoystickPosition(DISPLAY_LEFT);
      displayAddFunctionsGraphic(DISPLAY_RIGHT);
      break;
    case 3:
      displayJoystickLimits(DISPLAY_LEFT);
      displayAddFunctionsGraphic(DISPLAY_RIGHT);
      break;
    case 4:
      displayJoystickLimits(DISPLAY_LEFT);
      displayConfiguration(DISPLAY_RIGHT);
      activeTone = false;
      break;
  }
  statusScreenOld = statusScreen;
  
  // ----------------------------------------------------------------------------------
  // Soundwiedergabe für Änderung der Expo-Stellung
  expoHoeheFilter         = tickSound(expoHoehe,        expoHoeheFilter,         0,                100,              15, activeTone);
  expoSeiteFilter         = tickSound(expoSeite,        expoSeiteFilter,         0,                100,              15, activeTone);
  expoQuerFilter          = tickSound(expoQuer,         expoQuerFilter,          0,                100,              15, activeTone);
  querSeiteMischerFilter  = tickSound(querSeiteMischer, querSeiteMischerFilter, -LIMIT_QS_MISCHER, LIMIT_QS_MISCHER, 15, activeTone);
  querFlapsMischerFilter  = tickSound(querFlapsMischer, querFlapsMischerFilter, -LIMIT_QF_MISCHER, LIMIT_QF_MISCHER, 15, activeTone);
  flapHoeheMischerFilter  = tickSound(flapHoeheMischer, flapHoeheMischerFilter, -LIMIT_FH_MISCHER, LIMIT_FH_MISCHER, 15, activeTone);
  motrHoeheMischerFilter  = tickSound(motrHoeheMischer, motrHoeheMischerFilter,  0,                LIMIT_MH_MISCHER, 15, activeTone);
  flapsFilter             = tickSound(flaps,            flapsFilter,            -LIMIT_FLAPS,      LIMIT_FLAPS,      15, activeTone);
  trimmHoeheFilter        = tickSound(trimmHoehe,       trimmHoeheFilter,       -LIMIT_TRIMM,      LIMIT_TRIMM,      15, activeTone);
  trimmQuerFilter         = tickSound(trimmQuer,        trimmQuerFilter,        -LIMIT_TRIMM,      LIMIT_TRIMM,      15, activeTone);
  // nur im ersten Zyklus keine Töne für die Einstellungen wiedergeben, da alle Filter einen Durchlauf benötigen
  activeTone = true; 

  // ----------------------------------------------------------------------------------
  // Soundwiedergabe für Akkustand
  if (accuChargeLevel == 0)  
  {
    static unsigned long next = millis();
    unsigned long now         = millis();
    if (now >= next)
    {
      setSound(seq0000,7);
      next = now + 10000; // Ton alle 10 Sekunden wiederholen
    }
  }
  else if (accuChargeLevel != accuChargeLevelFilter)
  {
    // Ausführliche Auflistung der Soundsequenzen für entsprechenden Akkustand
    // (Soundprofil lässt sich leichter und individuell anpassen, im Gegensatz 
    // zu kompakterer Implementierung.)
    if      (accuChargeLevel ==100)  setSound(seq1000,3);
    else if (accuChargeLevel == 95)  setSound(seq100, 1);
    else if (accuChargeLevel == 90)  setSound(seq100, 2);
    else if (accuChargeLevel == 85)  setSound(seq100, 3);
    else if (accuChargeLevel == 80)  setSound(seq100, 4);
    else if (accuChargeLevel == 75)  setSound(seq500, 3);
    else if (accuChargeLevel == 70)  setSound(seq600, 1);
    else if (accuChargeLevel == 65)  setSound(seq600, 2);
    else if (accuChargeLevel == 60)  setSound(seq600, 3);
    else if (accuChargeLevel == 55)  setSound(seq600, 4);
    else if (accuChargeLevel == 50)  setSound(seq1000,3);
    else if (accuChargeLevel == 45)  setSound(seq100, 1);
    else if (accuChargeLevel == 40)  setSound(seq100, 2);
    else if (accuChargeLevel == 35)  setSound(seq100, 3);
    else if (accuChargeLevel == 30)  setSound(seq100, 4);
    else if (accuChargeLevel == 25)  setSound(seq500, 3);
    else if (accuChargeLevel == 22)  setSound(seq600, 1);
    else if (accuChargeLevel == 19)  setSound(seq600, 2);
    else if (accuChargeLevel == 16)  setSound(seq600, 3);
    else if (accuChargeLevel == 13)  setSound(seq600, 4);
    else if (accuChargeLevel == 10)  setSound(seq1000,3);
    else if (accuChargeLevel ==  9)  setSound(seq100, 1);
    else if (accuChargeLevel ==  8)  setSound(seq100, 2);
    else if (accuChargeLevel ==  7)  setSound(seq100, 3);
    else if (accuChargeLevel ==  6)  setSound(seq100, 4);
    else if (accuChargeLevel ==  5)  setSound(seq500, 3);
    else if (accuChargeLevel ==  4)  setSound(seq100, 1);
    else if (accuChargeLevel ==  3)  setSound(seq100, 2);
    else if (accuChargeLevel ==  2)  setSound(seq100, 3);
    else if (accuChargeLevel ==  1)  setSound(seq100, 4);
    accuChargeLevelFilter = accuChargeLevel;  
  }
 
  updateSound();
}
