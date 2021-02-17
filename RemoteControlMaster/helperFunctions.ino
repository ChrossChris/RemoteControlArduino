// -----------------------------------------------------------------------------------------
void readJoysticks()
{
  // Knüppelpositionen werden aus den Potis eingelesen und skaliert
  for (int i = 0; i < CHANNELS; i++)   
  {
    // Begrenzung der Poti-Werte (Grenzen können je nach Joystickstellung minimal überschritten werden) unter
    // Berücksichtigung der aktuellen Trimmung/Nulllage
    joysticks[i]  = constrain(joysticks[i] - centerPos[i], joystickLimits[i][MIN_POTI], joystickLimits[i][MAX_POTI]);

    // Skalieren auf maximalen Auflösungsbereich und Berücksichtigung der eingestellten Richtung
    if ((i == QUER) && (state != __TRIMMING__))
    {
      if (joysticks[i] < 0)  joysticks[i] = map(joysticks[i],  joystickLimits[i][MIN_POTI], 0, -joystickLimits[i][MAX], 0);
      else                   joysticks[i] = map(joysticks[i],  0, joystickLimits[i][MAX_POTI],  0, joystickLimits[i][MAX]);      
    }
    else
    {
      if (joysticks[i] < 0)  joysticks[i] = map(joysticks[i],  joystickLimits[i][MIN_POTI], 0,   joystickLimits[i][MIN],  0);
      else                   joysticks[i] = map(joysticks[i],  0, joystickLimits[i][MAX_POTI],   0,  joystickLimits[i][MAX]);
    }
  }
}
// -----------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------
int applyExpoCurve(const byte channel, const int expo)
{
    // Falls expo zu klein (quasi abgeschaltet), kann
    // die Berechnung abgekürzt und der Joystickwert direkt
    // zurückgegeben werden.
    if (expo < 10) return joysticks[channel];
    
    const float p = map(expo, 0, 1023, 100, 400) / 100.0F;
    float x       = (float) joysticks[channel];
    float selectedLimit;
    if (joysticks[channel] < 0) 
    {
      if (channel == QUER)  selectedLimit = (float) -joystickLimits[channel][MAX];
      else                  selectedLimit = (float)  joystickLimits[channel][MIN];
    }
    else selectedLimit = (float) joystickLimits[channel][MAX];

    // Berechnung der Potenzfunktion entsprechend:
    //   f: x |-> x^p  x = 0..1
    // x wird normiert entsprechend dem aktuell eingestellten maximalen Wert, 
    // die Potenzialfunktion ausgeführt und anschließend entnormiert durch 
    // Rückskalierung auf die eingestellten Limits.
    // (Betragsmäßig, für negative Auslenkungen egalisieren sich die Vorzeichen,
    // daher nur Unterscheidung nötig für Intervallgrenzen.)
    x /= selectedLimit;  // Normierung
    x  = pow(x, p);      // Kalibrierungskurve
    x *= selectedLimit;  // Entnormierung (Vorzeichen wird wieder hergestellt)
    
    return (int) (x);
}
// -----------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------
// Füge Totzone um die Mittelstellung ein, damit Sensitiviät in der Nulllage nicht so hoch
// ist. Der Wert der Totzone "deadzone" wird symmetrisch um die Mittelposition "centerValue"
// gelegt.
// (Ggf. bei Umstrukturierung Deklaration mit Default-Values für maxIn und minIn)
int mapWithDeadzone(const int value, const int minOut, const int maxOut, const int centerValue, const int deadzone)
{
  const int upperDeadzone = centerValue + deadzone/2;
  const int lowerDeadzone = centerValue - deadzone/2;
  if      (value > upperDeadzone)  return map(value, upperDeadzone, 1023,   0,      maxOut);
  else if (value < lowerDeadzone)  return map(value, 0,    lowerDeadzone,   minOut, 0);
  else                             return 0;
}
// -----------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------
// Mischt Ruderausschläge aufeinander, bspw. Querruder auf Seitenruder oder Flaps auf Höhenruder
// In der Realisierung verschiebt der Mixanteil die Nullage des Servos, sodass die Joystickauslenkung
// in beide Richtungen einen unterschiedlich langen Stellweg am Servo zur Folge haben. Ansonsten würde
// bspw. bereits der Anschlag bei halber Joystickauslenkung erreicht werden. Somit ergibt sich ein 
// besseres und nachvollziehbareres Steuergefühl.
// trg:   Zielkanal, auf den gemixt wird
// src:   Ausgangskanal, der den Mix steuert (in maximal Ruderausschlag CONTROL_LIMIT)
// value: Mixanteil in Prozent: ruderkanal += value [%] * source
int mixer(const byte trg, const int src, const int value)
{
  int mix = (long) value * src / 100; // Skalierung entsprechend Prozentanteil von value
  mix     = constrain(mix, joystickLimits[trg][MIN], joystickLimits[trg][MAX]);  // Mixanteil darf die maximalen Ruderausschläge nicht überschreiten
  if  (joysticks[trg] < 0)  mix = map(joysticks[trg], joystickLimits[trg][MIN], 0, joystickLimits[trg][MIN], mix);  // Mixer verschiebt die Nulllage, somit verbleiben 100%
  else                      mix = map(joysticks[trg], 0, joystickLimits[trg][MAX], mix, joystickLimits[trg][MAX]);  // Auslenkung für die Joysticks in beide Richtungen (mit jeweils 
                                                                                                                    // unterschiedlich lang resultierendem Stellweg am Servo)
  return mix;
}
// -----------------------------------------------------------------------------------------
