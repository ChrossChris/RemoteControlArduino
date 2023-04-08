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
    // Nichtlineare Kennlinie für die Motorstellung (in Zukunft als separate Funktion wie der Mischer):
    // -  0..  5% Geberstellung gehen auf       0% maximaler Gasstellung
    // -  5.. 10% Geberstellung gehen auf  0.. 30% maximaler Gasstellung
    // - 10.. 90% Geberstellung gehen auf 30.. 70% maximaler Gasstellung
    // - 90..100% Geberstellung gehen auf 70..100% maximaler Gasstellung
    // (Erste Implementierung, Kennlinie sollte eigentlich parametrierbar sein, ähnlich wie Expo und Mischer, allerdings schwieriger in der Eingabe)
    else if (i == MOTOR)
    {
      const int x1 = (5*joystickLimits[i][MAX_POTI])/100; //   5%
      const int x2 = (1*joystickLimits[i][MAX_POTI])/10;  //  10%
      const int x3 = (9*joystickLimits[i][MAX_POTI])/10;  //  90%
      const int x4 =    joystickLimits[i][MAX_POTI];      // 100%
      const int y1 =  0;                                  //   0%
      const int y2 = (3*joystickLimits[i][MAX])/10;       //  30%
      const int y3 = (7*joystickLimits[i][MAX])/10;       //  70%
      const int y4 =    joystickLimits[i][MAX];           // 100% 
      
      if      (joysticks[i] < x1) joysticks[i] = y1;
      else if (joysticks[i] < x2) joysticks[i] = map(joysticks[i],  x1, x2, y1, y2);
      else if (joysticks[i] < x3) joysticks[i] = map(joysticks[i],  x2, x3, y2, y3);
      else                        joysticks[i] = map(joysticks[i],  x3, x4, y3, y4);
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
// targetChannel:     Zielkanal, dem zugemischt werden soll
// mixIn:             Eingangswert, der den Mischer steuert (in maximal Ruderausschlag CONTROL_LIMIT)
// gain:              Verstärkungswert des Mischeranteil in Prozent: ruderkanal += gain [%] * source
// inputDeadzone:     Totzone im Eingangskanal, erst bei Überschreiten dieses Werts wird der Mischer aktiv
// inputMaximu:       Maximalwert des Eingangskanals (entsprechend der eingestellten Begrenzung nicht bei CONTROL_LIMIT)
int mixer(const byte targetChannel, const int mixIn, const int gain, const int inputDeadzone, const int inputMaximum)
{
  int mix = 0;
  if (abs(mixIn) > inputDeadzone)
  {
    const int sign = mixIn > 0 ? 1 : -1;
    mix = (long) gain * CONTROL_LIMIT * sign*(abs(mixIn)-inputDeadzone) / (100L*(inputMaximum - inputDeadzone));  // Skalierung entsprechend Prozentanteil von gain
    mix = constrain(mix, joystickLimits[targetChannel][MIN], joystickLimits[targetChannel][MAX]);                 // Mixanteil darf die maximalen Ruderausschläge nicht überschreiten
  }
  
  if  (joysticks[targetChannel] < 0)  mix = map(joysticks[targetChannel], joystickLimits[targetChannel][MIN], 0, joystickLimits[targetChannel][MIN], mix);  // Mixer verschiebt die Nulllage, somit verbleiben 100%
  else                                mix = map(joysticks[targetChannel], 0, joystickLimits[targetChannel][MAX], mix, joystickLimits[targetChannel][MAX]);  // Auslenkung für die Joysticks in beide Richtungen (mit jeweils 
                                                                                                                                                            // unterschiedlich lang resultierendem Stellweg am Servo)
  return mix;
}
// -----------------------------------------------------------------------------------------
