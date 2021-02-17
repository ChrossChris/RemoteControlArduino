// Stromkurve entspr. Motorstellung
// Accu 3S: i = 0.2A , x < 15%                                               Ruhestrom bis 15% Gasstellung
//              0.2A + (23A-0.2A) / (90%-15%) * (x-15%), 15% <= x <= 100%    Linearer Verlauf bis 90%, dann Maximum
// Accu 4S: i = 0.2A , x < 15%                                               Ruhestrom bis 15% Gasstellung
//              0.2A + (26A-0.2A) / (90%-15%) * (x-15%), 15% <= x <= 100%    Linearer Verlauf bis 90%, dann Maximum



// -----------------------------------------------------------------------------------------
// Hilfsfunktion zur Berechnung des Ladezustands des Akkus  
void updatePowerStateAccu(int joystickMotor)
{
  // Zeitpunkte, um die Integrationsschrittweite zu berechnen
  static unsigned long tOld   = micros();
  unsigned long        tNow   = micros();

  // Gradienten für die Stromkurven, aktuell linearer Verlauf angenommen, da nichts besseres bekannt
  constexpr float gradAccu3s = (ACCU_IMAX_3S - ACCU_I0) / (ACCU_GAS1 - ACCU_GAS0);
  constexpr float gradAccu4s = (ACCU_IMAX_4S - ACCU_I0) / (ACCU_GAS1 - ACCU_GAS0);

  // Begrenzung des maximalen Strom (bei ca. 90%), Erhöhung der Gasstellung hat keinen weiteren Einfluss
  float x = min(ACCU_GAS1, ((float) joystickMotor) / CONTROL_LIMIT);
  float i = 0.0F;

  if      (x < ACCU_GAS0)             i = ACCU_I0;                                   // Ruhestrom
  else if (accu == ACCU_3s_1800mAh)   i = ACCU_I0 + gradAccu3s * (x - ACCU_GAS0);    // Stromkurve für Akku mit 3 Zellen
  else if (accu == ACCU_3s_2200mAh)   i = ACCU_I0 + gradAccu3s * (x - ACCU_GAS0);    // Stromkurve für Akku mit 3 Zellen
  else if (accu == ACCU_4s_2400mAh)   i = ACCU_I0 + gradAccu4s * (x - ACCU_GAS0);    // Stromkurve für Akku mit 4 Zellen
  else                                i = 0.0F;                                      // Weitere Akkus bisher nicht in Verwendung

  // Integration des Stroms, um die bereits verbrauchte Energie zu berechnen
  powerStateAccu += i * (tNow - tOld) / (1E3*3600.0F); // Umrechnung in mAh

  // Integration des Motorknüppels
  throttleTotal  += (tNow - tOld) * x / 10000.0F;
  
  // Für den nächsten Zyklus wird der alte Zeitpunkt gespeichert
  tOld = tNow;  
}
