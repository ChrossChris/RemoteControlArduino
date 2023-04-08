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

// Poti-Limit
#define POTI_HOEHE_MIN     -335   // Ausschlag unten
#define POTI_HOEHE_MAX      335   // Ausschlag oben
#define POTI_SEITE_MIN     -335   // Ausschlag links
#define POTI_SEITE_MAX      310   // Ausschlag rechts
#define POTI_QUER_MIN      -325   // Ausschlag links
#define POTI_QUER_MAX       330   // Ausschlag rechts
#define POTI_MOTOR_MIN        0   // Ausschlag unten
#define POTI_MOTOR_MAX      680   // Ausschlag oben
  
// Steuerungseinstellungen
#define CONTROL_LIMIT       1000
#define CENTER_POS          512
#define DEADZONE            128
#define LIMIT_FLAPS         100   // Max. Wert 255
#define LIMIT_TRIMM         200   // Max. Wert 255
#define LIMIT_QS_MISCHER    150   // Max. Wert 255
#define LIMIT_QF_MISCHER    100   // Max. Wert 255
#define LIMIT_FH_MISCHER    100   // Max. Wert 255
#define LIMIT_MH_MISCHER    150   // Max. Wert 255

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
#define SIGNUM_TRIMM_HOEHE            0
#define SIGNUM_TRIMM_QUER             1
#define SIGNUM_QUER_SEITE_MISCHER     2
#define SIGNUM_QUER_FLAPS_MISCHER     3
#define SIGNUM_FLAPS                  4
#define SIGNUM_FLAPS_HOEHE_MISCHER    5

#define POTI_VALUE   									true
#define MODEL_VALUE										false
#define EEPROM_ADDRESS_MODEL					0
#define EEPROM_ADDRESS_ACCU  					1

// Akku-Kapazitäten
// Stromkurve entspr. Gasstellung x
// * Throttle <= 20%
//     i = ACCU_a0/2 + ACCU_a0/2 * x/20
// * Throttle >  20%, Throttle <= 70%
//     i = ACCU_a0 + ACCU_a1 * (  x-20%) + ACCU_a2 * (  x-20%)^2
// * Throttle >  70%
//     i = ACCU_a0 + ACCU_a1 * (70%-20%) + ACCU_a2 * (70%-20%)^2
#define AMOUNT_OF_ACCUS										 4
#define AMOUNT_OF_MODELS									 4
#define ACCU_4s_5500mAh                    0
#define ACCU_4s_2400mAh                    1
#define ACCU_3s_2200mAh                    2
#define ACCU_3s_1800mAh                    3
//#define ACCU_a0_3s                         237
//#define ACCU_a0_4s                         330
//#define ACCU_a1_3s                         5.925
//#define ACCU_a1_4s                         8.250
//#define ACCU_a2_3s                         8.0595
//#define ACCU_a2_4s                        12.8339
#define ACCU_a0_3s                         237000L
#define ACCU_a0_4s                         330000L
#define ACCU_a1_3s                         5925L
#define ACCU_a1_4s                         8250L
#define ACCU_a2_3s                         8060L
#define ACCU_a2_4s                        12834L
#define ACCU_THROTTLE_MIN                  20L
#define ACCU_THROTTLE_MAX                  70L
#define ACCU_CAPACITY_TOTAL_4s_2400mAh     2400
#define ACCU_CAPACITY_TOTAL_3s_2200mAh     2200
#define ACCU_CAPACITY_TOTAL_3s_1800mAh     1800
#define ACCU_CAPACITY_TOTAL_4s_5500mAh     5500

#endif