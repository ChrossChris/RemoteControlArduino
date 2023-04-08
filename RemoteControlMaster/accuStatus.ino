// Akku-Kapazitäten
// Stromkurve entspr. Gasstellung x
// * Throttle <= 20%
//     i = ACCU_a0/2 + ACCU_a0/2 * x/20
// * Throttle >  20%, Throttle <= 70%
//     i = ACCU_a0 + ACCU_a1 * (  x-20%) + ACCU_a2 * (  x-20%)^2
// * Throttle >  70%
//     i = ACCU_a0 + ACCU_a1 * (70%-20%) + ACCU_a2 * (70%-20%)^2

// -----------------------------------------------------------------------------------------
// Hilfsfunktion zur Berechnung des Ladezustands des Akkus  
void updatePowerStateAccu(int joystickMotor)
{
  // Zeitpunkte, um die Integrationsschrittweite zu berechnen
  static unsigned long tOld     = micros();
  unsigned long        tNow     = micros();

  // Nach ca. 70 min erfolgt ein Überlauf (kann prinzipiell auftreten, daher in diesem Fall 
  // Integrationsschritt einmalig überspringen)
  if (tNow <= tOld)
  {
    tOld = tNow;
    return;
  }

  // Maximalströme in der Sättigung
  constexpr long iMaxAccu3s = ACCU_a0_3s + ACCU_a1_3s*(ACCU_THROTTLE_MAX-ACCU_THROTTLE_MIN) + ACCU_a2_3s*(ACCU_THROTTLE_MAX-ACCU_THROTTLE_MIN)*(ACCU_THROTTLE_MAX-ACCU_THROTTLE_MIN);
  constexpr long iMaxAccu4s = ACCU_a0_4s + ACCU_a1_4s*(ACCU_THROTTLE_MAX-ACCU_THROTTLE_MIN) + ACCU_a2_4s*(ACCU_THROTTLE_MAX-ACCU_THROTTLE_MIN)*(ACCU_THROTTLE_MAX-ACCU_THROTTLE_MIN);

  long x = (100L*joystickMotor) / CONTROL_LIMIT;  // Umrechnung Gasstellung in Prozent
  long i = 0;                                     // Entladestrom

  if ( (accu == ACCU_3s_1800mAh) || (accu == ACCU_3s_2200mAh) )
  {
    if      (x < ACCU_THROTTLE_MIN)  i = (ACCU_a0_3s  + (ACCU_a0_3s*x) / ACCU_THROTTLE_MIN) / 2L;  // Klammerung um Fehler durch Ganzzahldivision zu minimieren
    else if (x < ACCU_THROTTLE_MAX)  i =  ACCU_a0_3s  +  ACCU_a1_3s * (x - ACCU_THROTTLE_MIN)    +  ACCU_a2_3s * (x - ACCU_THROTTLE_MIN)*(x - ACCU_THROTTLE_MIN);
    else                             i =  iMaxAccu3s;
  }
  else if ( (accu == ACCU_4s_2400mAh) || (accu == ACCU_4s_5500mAh) )
  {
    if      (x < ACCU_THROTTLE_MIN)  i = (ACCU_a0_4s  + (ACCU_a0_4s*x) / ACCU_THROTTLE_MIN) / 2L;  // Klammerung um Fehler durch Ganzzahldivision zu minimieren
    else if (x < ACCU_THROTTLE_MAX)  i =  ACCU_a0_4s  +  ACCU_a1_4s * (x - ACCU_THROTTLE_MIN)    +  ACCU_a2_4s * (x - ACCU_THROTTLE_MIN)*(x - ACCU_THROTTLE_MIN);
    else                             i =  iMaxAccu4s;
  }

  // Aufgrund der Integerdarstellung sind die Koeffizienten erhöht worden und Strom wird in Mikroampere berechnet, daher
  // Umrechnung in Milliampere (vorab nötig, sonst Überlauf):
  i = i / 1000L;

  // Integration des Stroms, um die bereits verbrauchte Energie zu berechnen und des Motorknüppels
  const long dt       = tNow - tOld;         // Integrationsschritt in Mikrosekunden
  accuDischargeTotal += (i * dt) / 1000000L; // Umrechung mAs (Division 100000: Mikrosekunden -> Sekunden)
  throttleTotal      += (x * dt) / 1000L;    // Aufintegration der Gasstellung in Prozent*Millisekunden (Division 1000: Mikrosekunden -> Millisekunden)

  // Für den nächsten Zyklus wird der alte Zeitpunkt gespeichert
  tOld = tNow;  
}
