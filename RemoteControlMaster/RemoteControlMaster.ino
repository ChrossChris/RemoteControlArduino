// Relativpfad funktioniert leider nicht zum Einbinden
#include "C:/Dokumente/Elektronik & Modellbau/Fernsteuerung/Arduino/inc/definitions.h"
#include "C:/Dokumente/Elektronik & Modellbau/Fernsteuerung/Arduino/inc/multiplex_heron.h"
#include "C:/Dokumente/Elektronik & Modellbau/Fernsteuerung/Arduino/inc/dpower_streamline.h"
#include "C:/Dokumente/Elektronik & Modellbau/Fernsteuerung/Arduino/inc/multiplex_easyglider.h"
#include "C:/Dokumente/Elektronik & Modellbau/Fernsteuerung/Arduino/inc/graupner_amigo4.h"

#include <Bounce2.h>
#include <EEPROM.h>


// Current state of execution
#define __INIT__            0
#define __TEST__            1
#define __RUNNING__         2
#define __TRIMMING__        3
#define __STOP_TRIMMING__   4
int     state = __INIT__;
Bounce  button;

// Globale Variable zur Konfiguration der Joystick, der Trimmlage und ggf.
// Start des Testprogramms
int   joysticks[CHANNELS];
int   centerPos[CHANNELS];
int   joystickLimits[CHANNELS][MAX_POTI+1];
byte  statusScreen        = 0;
unsigned long  startTime  = 0;


// Kalibrierungs- und Zusatzstellfunktionen
int  flaps                = 0;
int  trimmHoehe           = 0;
int  trimmQuer            = 0;
int  querSeiteMischer     = 0;
int  flapHoeheMischer     = 0;
int  motrHoeheMischer     = 0;
int  motrHoeheTotzone     = 0;
int  querFlapsMischer     = 0;
int  expoQuer             = 0;
int  expoHoehe            = 0;
int  expoSeite            = 0;
int  butterflyMixer       = 0;
bool butterfly            = false;
byte remoteControlSetting = 0;
byte model                = 0;

// Schalterstellungen zur Richtungsumkehr (Schalterstellungen unterscheiden sich aufgrund Einbaurichtung)
// (Normalstellung ist aber, wenn alle Schalter nach unten weisen!)
int  flipHoehe            = 1;
int  flipSeite            = 1;
int  flipQuerLinks        = 1;
int  flipQuerRechts       = 1;
int  flipFlapLinks        = 1;
int  flipFlapRechts       = 1;

// Ladezustand Akku
byte accu                = 0;  // Akku
long accuDischargeTotal  = 0;  // Entlademenge Akku [mAs] (Milliampersekunden)
long throttleTotal       = 0;  // Aufkummulierte Gasstellung [%ms] (Prozent*Millisekunden)

// Ausgangssignale an den Sender
// Container für PPM-Signale für jeden einzelnen Servo
int  ppm[PPM_CHANNELS]   = {PPM_CENTER_VALUE};

// -----------------------------------------------------------------------------------------------------------------------------  
void setup()
{
  // Eingänge für Schalter konfigurieren
  pinMode(SWITCH_RECHTS_OBEN,   INPUT);  // ON:  Schalter oben   OFF: Schalter unten
  pinMode(SWITCH_RECHTS_UNTEN,  INPUT);  // ON:  Schalter oben   OFF: Schalter unten
  pinMode(SWITCH_LINKS_OBEN,    INPUT);  // OFF: Schalter unten  ON:  Schalter oben (Schalter verkehrt herum eingebaut, wird invertiert)
  pinMode(SWITCH_LINKS_UNTEN,   INPUT);  // ON:  Schalter unten  OFF: Schalter oben
  
  // Nullpunkt und maximale Joystickausschläge: Höhenruder
  centerPos[HOEHE]                = analogRead(JOYST_HOEHE);
  joystickLimits[HOEHE][MIN_POTI] = POTI_HOEHE_MIN;
  joystickLimits[HOEHE][MAX_POTI] = POTI_HOEHE_MAX;
  
  // Nullpunkt und maximale Joystickausschläge: Seitenruder
  centerPos[SEITE]                = analogRead(JOYST_SEITE);
  joystickLimits[SEITE][MIN_POTI] = POTI_SEITE_MIN;
  joystickLimits[SEITE][MAX_POTI] = POTI_SEITE_MAX;
  
  // Nullpunkt und maximale Joystickausschläge: Querruder
  centerPos[QUER]                 = analogRead(JOYST_QUER);
  joystickLimits[QUER][MIN_POTI]  = POTI_QUER_MIN;
  joystickLimits[QUER][MAX_POTI]  = POTI_QUER_MAX;
  
  // Nullpunkt und maximale Joystickausschläge: Motor
  centerPos[MOTOR]                = analogRead(JOYST_MOTOR);
  joystickLimits[MOTOR][MIN_POTI] = POTI_MOTOR_MIN;
  joystickLimits[MOTOR][MAX_POTI] = POTI_MOTOR_MAX;

  // Auswahl und Konfiguration des Modells und des Akkus: Die vorherige Auswahl wird per Default
  // eingestellt.
  model = EEPROM.read(EEPROM_ADDRESS_MODEL);
  accu  = EEPROM.read(EEPROM_ADDRESS_ACCU);
  setModel(model);


  // PPM-Generierung und Timer für Interrupt-Erzeugung
  for (int i=0; i < PPM_CHANNELS; i++)  ppm[i] = PPM_CENTER_VALUE;
  pinMode(PPM_PORT, OUTPUT);
  digitalWrite(PPM_PORT, LOW);
  cli();
  TCCR1A  = 0;                // TCCR1 Register zurücksetzen
  TCCR1B  = 0;                // TCCR1 Register zurücksetzen
  OCR1A   = 100;              // Compare-Register zum Auslösen des Interrupts
  TCCR1B |= (1 << WGM12);     // CTC Mode (Clear Timer on Compare Match)
  TCCR1B |= (1 << PRESCALER); // Prescaler des Systemtakts für Inkrement von Timer1
  TIMSK1 |= (1 << OCIE1A);    // Aktivierung ISR bei Timer-Compare mir OCR1A-Register
  sei();

  // Serielle Schnittstelle starten
  Serial.begin(BAUD_RATE);
  Serial.flush();

  // Startzustand der State Machine und Entprellung der Tasters über Bounce-Bibliothek
  button.attach(BUTTON, INPUT);  // Input Port
  button.interval(25);           // Entprellzeit in ms
  state = __INIT__;              // Initial state
  
}
// -----------------------------------------------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------------------------------------------
void loop()
{
  // -----------------------------------------------------------------------------
  // STATE-MACHINE (START)
  // Check for control button to update the state machine
  button.update();
  int old_state = state;  
  switch (state)
  {
    case __INIT__:
      if      (button.read()     ==  LOW)       state = __RUNNING__;
      else if (button.duration() >  2000)       state = __TEST__;
      else                                      state = __INIT__;
      break;
      
    case __TEST__:
      if   (button.rose())                      state = __RUNNING__;
      else                                      state = __TEST__;
      break;
      
    case __RUNNING__:
      if      (    button.rose()             )  state = __RUNNING__;
      else if (   (button.read()     == HIGH)  
               && (button.duration() >  2000))  state = __TRIMMING__;
      else                                      state = __RUNNING__;
      break;

    case __TRIMMING__:
      if (button.read() == LOW)                 state = __STOP_TRIMMING__;
      else                                      state = __TRIMMING__;
      break;

    case __STOP_TRIMMING__:
      state = __RUNNING__;
  }
  // STATE-MACHINE (ENDE)
  // -----------------------------------------------------------------------------  

  
  // -----------------------------------------------------------------------------
  // Joystick-Positionen:  
  joysticks[HOEHE] = analogRead(JOYST_HOEHE);
  joysticks[SEITE] = analogRead(JOYST_SEITE);
  joysticks[QUER]  = analogRead(JOYST_QUER);
  joysticks[MOTOR] = analogRead(JOYST_MOTOR);

  // Schalterstellungen:
  // ON: Schalter oben   OFF: Schalter unten 
  const bool switchLinksOben   = !digitalRead(SWITCH_LINKS_OBEN);  // Korrektur nötig, da Schalter links invers verbaut ist
  const bool switchLinksUnten  =  digitalRead(SWITCH_LINKS_UNTEN); 
  const bool switchRechtsOben  =  digitalRead(SWITCH_RECHTS_OBEN);
  const bool switchRechtsUnten =  digitalRead(SWITCH_RECHTS_UNTEN);
  
  // Mischanteile und Aufspaltung der Querruderwerte für individuelle
  // Anlenkung (Querruder werden als Wölbklappen/Flaps verwendet)
  int querLinks  = 0;
  int querRechts = 0;
  int flapLinks  = 0;
  int flapRechts = 0;
 
  // --------------------------------------------------------------------
  // Startup
  if (state == __INIT__)
  {
    // Reset start time to current time 
    startTime     = millis();
    statusScreen  = 0;
  }
  
  else if (state == __TEST__)
  {
    // Testprogramm zur automatischen Ansteuerung der Ruder
    runTestprogram();
    querLinks    = joysticks[QUER];
    querRechts   = joysticks[QUER];
    flapLinks    = querLinks  / 2;
    flapRechts   = querRechts / 2;
    statusScreen = 2; // Joystickausschläge
  }

  else if (state == __TRIMMING__)
  { 
    // Auswahl von Modell und und eingesetztem Akku
    int newModel = 0;
    accu     = map(analogRead(POTI_LINKS_2), 0, 1024, 0, AMOUNT_OF_ACCUS);
    newModel = map(analogRead(POTI_LINKS_1), 0, 1024, 0, AMOUNT_OF_MODELS); 
    if (newModel != model)  setModel(newModel);
 
    // Maximale Auslenkungen der Knüppel für die jeweiligen Achsen festlegen
    const bool resetInitialLimits = (old_state != state) || (newModel != model);
    setJoystickLimits(resetInitialLimits);

    // Knüppelpositionen werden aus den Potis eingelesen und skaliert,
    // damit sie auch während der Trimmung eine Wirkung zeigen
    readJoysticks();

    // Zusätzliche Trimmsignale (die im normalen Mode __RUNNING__ nicht eingestellt werden können, da alle Analogeingänge bereits doppelt belegt in Verwendung sind
    static int bufferPotiRechts1;
    static int bufferPotiRechts2;
    if ((old_state != state) || (newModel != model))
    {
      bufferPotiRechts1 = analogRead(POTI_RECHTS_1);
      bufferPotiRechts2 = analogRead(POTI_RECHTS_2);
    }
    else 
    {
      if (abs(bufferPotiRechts1-analogRead(POTI_RECHTS_1)) > 50)
      {
        expoSeite         = analogRead(POTI_RECHTS_1);
        bufferPotiRechts1 = -51; // Sicherstellen, dass immer der aktuelle Wert übernommen wird und sich das Poti nicht zufällig wieder in Ausgangslage befindet
      }
      if (abs(bufferPotiRechts2-analogRead(POTI_RECHTS_2)) > 50)
      {
        motrHoeheMischer  = analogRead(POTI_RECHTS_2);
        motrHoeheMischer  = mapWithDeadzone(motrHoeheMischer, 0, LIMIT_MH_MISCHER, 0, DEADZONE);  // symmetrische Skalierung zwischen den Grenzen und einer Totzone
        bufferPotiRechts2 = -51; // Sicherstellen, dass immer der aktuelle Wert übernommen wird und sich das Poti nicht zufällig wieder in Ausgangslage befindet
      }
    }
        
    flapHoeheMischer = 512; // Flap-Höhe-Mixer in Mittelstellung (nicht verwendet), da nicht mehr genügend Analogeingänge vorhanden
    flapHoeheMischer = mapWithDeadzone(flapHoeheMischer, -LIMIT_FH_MISCHER, LIMIT_FH_MISCHER, CENTER_POS, DEADZONE);  // symmetrische Skalierung zwischen den Grenzen und einer Totzone
     
    querLinks         = joysticks[QUER];
    querRechts        = joysticks[QUER];
    flapLinks         = (long) querFlapsMischer * querLinks  / 100;
    flapRechts        = (long) querFlapsMischer * querRechts / 100;    
    statusScreen      = 4;
    model             = newModel;
  }

  else if (state == __STOP_TRIMMING__)
  {
    for (int i = 0; i < CHANNELS-1; i++)   
    {
      // Skalieren auf maximalen Auflösungsbereich
      if (joystickLimits[i][MIN] == 0)   joystickLimits[i][MIN] = -joystickLimits[i][MAX];
      if (joystickLimits[i][MAX] == 0)   joystickLimits[i][MAX] = -joystickLimits[i][MIN];
    }

    // Motor kann nicht negativ ausgelenkt werden, daher immer auf null setzen, egal ob
    // verändert wurde oder nicht
    joystickLimits[MOTOR][MIN] = 0;

    // Zurücksetzen des Akkuladezustands (kann zur Verzehrungen führen, daher nur bei vollem
    // Akku machen)
    accuDischargeTotal = 0;
    throttleTotal      = 0;
    
    // Speichere Modell- und Akkuauswahl im EEProm
    EEPROM.update(EEPROM_ADDRESS_MODEL, model);
    EEPROM.update(EEPROM_ADDRESS_ACCU,  accu);
  
    // Reset status screen and start time for normal running mode
    startTime    = millis();
    statusScreen = 0;
  }

  else if (state == __RUNNING__)
  {
    // Knüppelpositionen werden aus den Potis eingelesen und skaliert
    readJoysticks();
    
    // Kalibrierungseingänge
    // Abfragen der doppelt belegten Potis entsprechend der Schalterstellung.
    // Es fehlt noch die Berücksichtigung, dass der Wert nur übernommen wird,
    // wenn dieser geändert wird
    if (switchLinksOben == OFF)         
    {
      trimmHoehe = analogRead(POTI_LINKS_1);
      trimmHoehe = mapWithDeadzone(trimmHoehe, -LIMIT_TRIMM, LIMIT_TRIMM, CENTER_POS, DEADZONE);  // symmetrische Skalierung zwischen den Grenzen und einer Totzone
      bitClear(remoteControlSetting, TRIMM_EXPO_HOEHE);
    }
    else
    {
      expoHoehe = analogRead(POTI_LINKS_1);
      bitSet(remoteControlSetting, TRIMM_EXPO_HOEHE);
    }
    
    if (switchLinksUnten == OFF)         
    {
      trimmQuer = analogRead(POTI_LINKS_2);
      trimmQuer = mapWithDeadzone(trimmQuer, -LIMIT_TRIMM, LIMIT_TRIMM, CENTER_POS, DEADZONE);  // symmetrische Skalierung zwischen den Grenzen und einer Totzone
      bitClear(remoteControlSetting, TRIMM_EXPO_QUER);
    }
    else                      
    {
      expoQuer = analogRead(POTI_LINKS_2);
      bitSet(remoteControlSetting, TRIMM_EXPO_QUER);
    }

    if (switchRechtsOben == OFF) 
    {
      querSeiteMischer = analogRead(POTI_RECHTS_1);
      querSeiteMischer = mapWithDeadzone(querSeiteMischer, -LIMIT_QS_MISCHER, LIMIT_QS_MISCHER, CENTER_POS, DEADZONE);  // symmetrische Skalierung zwischen den Grenzen und einer Totzone
      bitClear(remoteControlSetting, QUER_SEITE_FLAPS_MISCHER);
    }
    else
    {
      // Stellung der Querruder wird durch POTI_RECHTS_1 in Abhängigkeit der Wölbklappen gestellt, querFlapMischer wird dafür zweckemfremdet
      querFlapsMischer = analogRead(POTI_RECHTS_1);
      querFlapsMischer = mapWithDeadzone(querFlapsMischer, -LIMIT_QF_MISCHER, LIMIT_QF_MISCHER, CENTER_POS, DEADZONE); // symmetrische Skalierung zwischen den Grenzen und einer Totzone
      bitSet(remoteControlSetting, QUER_SEITE_FLAPS_MISCHER);
    }

    if (switchRechtsUnten == OFF)   
    {
      butterfly = false;
      bitClear(remoteControlSetting, BUTTERFLY);
    }
    else                      
    {
      butterfly = true;
      bitSet(remoteControlSetting, BUTTERFLY);
    }
    // Stellung der Wölbklappen 
    flaps = analogRead(POTI_RECHTS_2);
    flaps = mapWithDeadzone(flaps, -LIMIT_FLAPS, LIMIT_FLAPS, CENTER_POS, DEADZONE); // Flaps sind sowohl für Butterfly als auch für den Speed-/Thermik-Flug einstellbar
    
    // Exponential-Mischer auf Ruderstellungen anwenden
    joysticks[HOEHE] = applyExpoCurve(HOEHE, expoHoehe);
    joysticks[SEITE] = applyExpoCurve(SEITE, expoSeite); 
    joysticks[QUER]  = applyExpoCurve(QUER,  expoQuer);

    // Mixer: Ruderausschläge zumischen
    joysticks[SEITE] = mixer(SEITE, joysticks[QUER],         querSeiteMischer,                0,  joystickLimits[QUER][MAX]);  // QS-Mischer: Querruder zu Seitenruder mischen
    joysticks[HOEHE] = mixer(HOEHE, flaps*CONTROL_LIMIT/100, flapHoeheMischer,                0,              CONTROL_LIMIT);  // FH-Mischer: Flaps zu Höhenruder mischen
    joysticks[HOEHE] = mixer(HOEHE, joysticks[MOTOR],       -motrHoeheMischer, motrHoeheTotzone, joystickLimits[MOTOR][MAX]);  // MH-Mischer: Motor zu Höhenruder mischen (automatisches Drücken)

    // 2-Klappenkonfiguration (ohne Wölbklappen wie Solius und Amigo), Querruder können als
    // Wölbklappen für den Landeanflug verstellt werden.
    if (butterflyMixer == 0)
    {
      // Flaps: Offset für Querruder-Nullage
      querLinks  =  ((long) (flaps * CONTROL_LIMIT) / LIMIT_FLAPS) + trimmQuer;
      querRechts = -((long) (flaps * CONTROL_LIMIT) / LIMIT_FLAPS) + trimmQuer;
      if  (joysticks[QUER] < 0)
      {
        querLinks  = map(joysticks[QUER], -CONTROL_LIMIT, 0, -CONTROL_LIMIT, querLinks);
        querRechts = map(joysticks[QUER], -CONTROL_LIMIT, 0, -CONTROL_LIMIT, querRechts);
      }
      else  
      {
        querLinks  = map(joysticks[QUER], 0, CONTROL_LIMIT, querLinks,  CONTROL_LIMIT);
        querRechts = map(joysticks[QUER], 0, CONTROL_LIMIT, querRechts, CONTROL_LIMIT);
      }
      querLinks  = constrain(querLinks,  -CONTROL_LIMIT, CONTROL_LIMIT);
      querRechts = constrain(querRechts, -CONTROL_LIMIT, CONTROL_LIMIT);
      flapLinks  = 0;
      flapRechts = 0;
    }

    // 4-Klappenkonfiguration ohne Butterfly (normal, kein Landeanflug), Stellung der Wölbklappen 
    // werden direkt eingestellt.
    else if (!butterfly)
    {
      if  (joysticks[QUER] < 0)
      {
        // beide Ausschläge negativ: Links maximal, rechts minimal
        const int limitQuerLinks  = max(-CONTROL_LIMIT, -joystickLimits[QUER][MAX]+trimmQuer);
        const int limitQuerRechts = max(-CONTROL_LIMIT,  joystickLimits[QUER][MIN]+trimmQuer);
        querLinks  = map(joysticks[QUER], -joystickLimits[QUER][MAX], 0, limitQuerLinks,  trimmQuer);
        querRechts = map(joysticks[QUER], -joystickLimits[QUER][MAX], 0, limitQuerRechts, trimmQuer);
      }
      else  
      {
        // beide Ausschläge prositiv: Links minimal, rechts maximal
        const int limitQuerLinks  = min(CONTROL_LIMIT, -joystickLimits[QUER][MIN]+trimmQuer);
        const int limitQuerRechts = min(CONTROL_LIMIT,  joystickLimits[QUER][MAX]+trimmQuer);
        querLinks  = map(joysticks[QUER], 0, joystickLimits[QUER][MAX], trimmQuer,  limitQuerLinks);
        querRechts = map(joysticks[QUER], 0, joystickLimits[QUER][MAX], trimmQuer,  limitQuerRechts);
      }
  
      // Flaps: Wölbklappen und Berücksichtigung von Querrudermitnahme
      flapLinks  = -((long) flaps * CONTROL_LIMIT / LIMIT_FLAPS);
      flapRechts =  ((long) flaps * CONTROL_LIMIT / LIMIT_FLAPS);

      if (querFlapsMischer < 0)
      {
        flapLinks  += ((long) (querLinks  - trimmQuer) * abs(querFlapsMischer)) / 100;
        flapRechts += ((long) (querRechts - trimmQuer) * abs(querFlapsMischer)) / 100;
      }
      else
      {
        flapLinks  += min( ((long) (querLinks  - trimmQuer) * abs(querFlapsMischer)) / 100, 0);  // Links:  Kein Wölbklappenausschlag nach unten bei negativem Wert
        flapRechts += max( ((long) (querRechts - trimmQuer) * abs(querFlapsMischer)) / 100, 0);  // Rechts: Kein Wölbklappenausschlag nach unten bei positivem Wert
      }
    }
    
    // Konfiguration Landeanflug mit Butterfly, Wölbklappenstellung kann direkt vorgegeben werden,
    // Querruder entsprechend ButterflyMixer-Faktor
    else
    {
      querLinks  =  ((long) butterflyMixer * flaps * CONTROL_LIMIT) / (100*100) + trimmQuer;
      querRechts = -((long) butterflyMixer * flaps * CONTROL_LIMIT) / (100*100) + trimmQuer;
      if  (joysticks[QUER] < 0)
      {
        // beide Ausschläge negativ: Links maximal, rechts minimal
        const int limitQuerLinks  = max(-CONTROL_LIMIT,       -joystickLimits[QUER][MAX]+querLinks  );
        const int limitQuerRechts = max(-CONTROL_LIMIT, min(0, joystickLimits[QUER][MIN]+querRechts)); // Ausschlag nach unten bis mindestens in Nullposition sicherstellen
        querLinks  = map(joysticks[QUER], -joystickLimits[QUER][MAX], 0, limitQuerLinks,  querLinks);
        querRechts = map(joysticks[QUER], -joystickLimits[QUER][MAX], 0, limitQuerRechts, querRechts);
      }
      else  
      {
        // beide Ausschläge prositiv: Links minimal, rechts maximal
        const int limitQuerLinks  = min(CONTROL_LIMIT, max(0, -joystickLimits[QUER][MIN]+querLinks)); // Ausschlag nach unten bis mindestens in Nullposition sicherstellen
        const int limitQuerRechts = min(CONTROL_LIMIT,         joystickLimits[QUER][MAX]+querRechts);
        querLinks  = map(joysticks[QUER], 0, joystickLimits[QUER][MAX], querLinks,  limitQuerLinks);
        querRechts = map(joysticks[QUER], 0, joystickLimits[QUER][MAX], querRechts, limitQuerRechts);
      }
      
      flapLinks  = -((long) flaps * CONTROL_LIMIT) / 100;
      flapRechts =  ((long) flaps * CONTROL_LIMIT) / 100;
    }
      
    // Wechsel des Status-Screens, falls der Taster kurz gedrückt wurde
    if (button.rose())  (++statusScreen) %= 4;
  }

  // PPM-Signale setzen, außer Querruder, die links und rechts getrennt ausgegeben werden.
  setPPM(PPM_MOTOR,                        joysticks[MOTOR], 0);
  setPPM(PPM_HOEHE,       flipHoehe      * joysticks[HOEHE], trimmHoehe);
  setPPM(PPM_SEITE,       flipSeite      * joysticks[SEITE], 0);
  setPPM(PPM_QUER_LINKS,  flipQuerLinks  * querLinks,        0);
  setPPM(PPM_QUER_RECHTS, flipQuerRechts * querRechts,       0);
  setPPM(PPM_FLAP_LINKS,  flipFlapLinks  * flapLinks,        0);
  setPPM(PPM_FLAP_RECHTS, flipFlapRechts * flapRechts,       0);
    
  // Berechnung des Akku-Ladezustands auf Basis des Motoraktuierung
  // und der zugrunde liegenden charakteristischen Stromkurve
  updatePowerStateAccu(joysticks[MOTOR]);

  // Display Kalibrierung und Trimmung -> via Serial-Interface: RemoteControlSlave
  updateDisplay(); 

}
// -----------------------------------------------------------------------------------------------------------------------------
