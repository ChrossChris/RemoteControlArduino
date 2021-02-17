#ifndef DEFINITIONS
#define DEFINITIONS

// Input-Ports                  // Kabelfarbe   |  Potikontakt
#define JOYST_QUER          A0  // orange       |  Stecker IV
#define JOYST_HOEHE         A1  // gelb         |  Stecker III
#define JOYST_SEITE         A2  // grün         |  Stecker II
#define JOYST_MOTOR         A3  // blau         |  Stecker I   (gedreht)
#define POTI_LINKS_1        A4  // rot          |  grün
#define POTI_LINKS_2        A5  // orange       |  gelb
#define POTI_RECHTS_1       A6  // gelb         |  weiß
#define POTI_RECHTS_2       A7  // grün         |  schwarz
#define BUTTON               7  // blau         |  schwarz
#define SWITCH_RECHTS_OBEN   8  // lila         |  grün/blau
#define SWITCH_RECHTS_UNTEN  9  // grau         |  rot
#define SWITCH_LINKS_OBEN   10  // weiß         |  weiß
#define SWITCH_LINKS_UNTEN  11  // lila (Joyst) |  Stecker Schalter (nachträglich eingebaut und über freies Joystickkabel)
#define ON                  true
#define OFF                 false

// Steuerungseinstellungen
#define CONTROL_LIMIT       1000
#define CENTER_POS          512
#define DEADZONE            128
#define LIMIT_FLAPS         100   // Max. Wert 255
#define LIMIT_TRIMM         250   // Max. Wert 255
#define LIMIT_QS_MISCHER    150   // Max. Wert 255
#define LIMIT_QF_MISCHER    100   // Max. Wert 255
#define LIMIT_FH_MISCHER    100   // Max. Wert 255
#define LIMIT_MH_MISCHER    100   // Max. Wert 255

// Indizes für die einzelnen Ruder und Array-Zugriffe
#define HOEHE               0
#define SEITE               1
#define QUER                2
#define MOTOR               3
#define MIN                 0
#define MAX                 1
#define MIN_POTI            2
#define MAX_POTI            3
#define CHANNELS            4

// Serielle Schnittstelle
#define BAUD_RATE           57600
#define SERIAL_START        255
#define SERIAL_END          128
#define UPDATE_PERIOD       100

// LCD-Display und Sound
#define DISPLAY_RIGHT       0x25
#define DISPLAY_LEFT        0x27
#define DISPLAY_WIDTH       20
#define DISPLAY_HEIGHT      4
#define BUZZER_PORT         11
#define SEQUENCE_LENGTH     7      // Max. Anzahl an Tonfolgen

// Generierung PPM-Signal
#define PPM_PORT            12     // Output-Pin
#define FRAME_LENGTH        23000  // 23ms  Paketlänge
#define PULSE_LENGTH        420    // 420µs Pulslänge (200µs..500µs) (gemessen an FM6014)
#define PRESCALER           CS11   // Prescaler 8, 0.5µs-Takt des Timers
#define MIKRO_SECONDS       2      // Korrektur des Output-Compare-Registers durch Faktor 2 nötig entspr. Prescaler
#define PPM_CENTER_VALUE    1500   // 1,5ms Mittelstellung 
#define PPM_LEVEL           500    // 0,5ms Signalhub plus/minus
#define PPM_CHANNELS        8      // Anzahl der PPM-Kanäle in einem Paket
#define PPM_MOTOR           0
#define PPM_HOEHE           1
#define PPM_SEITE           2
#define PPM_QUER_LINKS      3
#define PPM_QUER_RECHTS     4
#define PPM_FLAP_LINKS      5
#define PPM_FLAP_RECHTS     6

// Codierindizes für kompaktes Format
#define TRIMM_EXPO_HOEHE              0
#define TRIMM_EXPO_QUER               1
#define QUER_SEITE_FLAPS_MISCHER      2
#define BUTTERFLY                     3
#define FLIP_HOEHE                    4
#define FLIP_SEITE                    5
#define FLIP_QUER                     6
#define SIGNUM_TRIMM_HOEHE            0
#define SIGNUM_TRIMM_QUER             1
#define SIGNUM_QUER_SEITE_MISCHER     2
#define SIGNUM_QUER_FLAPS_MISCHER     3
#define SIGNUM_FLAPS                  4
#define SIGNUM_FLAPS_HOEHE_MISCHER    5

// Akku-Kapazitäten
// Stromkurve entspr. Motorstellung
// Accu 3S: i = 0.2A , x < 15%                                               Ruhestrom bis 15% Gasstellung
//              0.2A + (23A-0.2A) / (90%-15%) * (x-15%), 15% <= x <= 100%    Linearer Verlauf bis 90%, dann Maximum
// Accu 4S: i = 0.2A , x < 15%                                               Ruhestrom bis 15% Gasstellung
//              0.2A + (26A-0.2A) / (90%-15%) * (x-15%), 15% <= x <= 100%    Linearer Verlauf bis 90%, dann Maximum
#define ACCU_4s_2400mAh                    1
#define ACCU_3s_2200mAh                    2
#define ACCU_3s_1800mAh                    3
#define ACCU_I0                            0.2
#define ACCU_GAS0                          0.15
#define ACCU_GAS1                          0.90
#define ACCU_IMAX_3S                       20.0
#define ACCU_IMAX_4S                       26.0
#define ACCU_THROTTLE_TOTALMAX_4s_2400mAh  24000   // 100% -> 26700
#define ACCU_THROTTLE_TOTALMAX_3s_2200mAh  29300   // 100% -> 32600
#define ACCU_THROTTLE_TOTALMAX_3s_1800mAh  22800   // 100% -> 25300

#endif