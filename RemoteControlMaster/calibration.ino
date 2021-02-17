// -----------------------------------------------------------------------------------------------------------------------------
void setJoystickLimits(bool first_call)
{
  static bool initialReset[CHANNELS];

  // Obere und untere Begrenzung übernehmen, wenn sie überschritten wurden  
  for (int i = 0; i < CHANNELS; i++)
  {
    // Flankenerkennung, damit nur die Richtungen kalibriert werden, in denen der Joystick
    // auch aus der Ruhelage bewegt wurde
    if (first_call) 
    {
      centerPos[i]    = joysticks[i];
      initialReset[i] = true;
    }

    // Joystick kalibrieren? -> Initial einmal aus der Ruhelage bewegen,
    // um minimale und maximale Grenzen zurückzusetzen. Schwelle ist aktuell
    // auf 5% festgelegt.
    const int actuationLevel            = joysticks[i] - centerPos[i];
    const int minNegativeActuationLevel = (joystickLimits[i][MIN_POTI] * 5) / 100;
    const int minPositiveActuationLevel = (joystickLimits[i][MAX_POTI] * 5) / 100;
    if ( initialReset[i] == true ) 
    {
      if (   ( actuationLevel > minPositiveActuationLevel )
          || ( actuationLevel < minNegativeActuationLevel )  ) 
      {
        joystickLimits[i][MIN] = 0;
        joystickLimits[i][MAX] = 0;
        initialReset[i]        = false;
      }
    }

    // Minimalen Ausschlag bestimmen, Begrenzung auf 100% (Integergrenzen werden im 
    // Zwischenergebnis überschritten, daher Erweiterung auf Long-Integer)
    // (else if kein Ausschluss zu maximalem Ausschlag, da der Joystick immer nur in
    // die eine oder andere Richtung weisen kann, aber nie beide gleichzeitig auslenken
    // kann.)
    else if ( actuationLevel < minNegativeActuationLevel)
    {
      int minJoystick = ((long) CONTROL_LIMIT * actuationLevel) / -joystickLimits[i][MIN_POTI]; 
      if (minJoystick < joystickLimits[i][MIN])   joystickLimits[i][MIN] = constrain(minJoystick, -CONTROL_LIMIT, 0);
    }
    
    // Maximalen Ausschlag bestimmen, Begrenzung auf 100% (Integergrenzen werden im 
    // Zwischenergebnis überschritten, daher Erweiterung auf Long-Integer)
    // (else if kein Ausschluss zu minimalem Ausschlag, da der Joystick immer nur in
    // die eine oder andere Richtung weisen kann, aber nie beide gleichzeitig auslenken
    // kann.)
    else if ( actuationLevel > minPositiveActuationLevel)
    {
      int maxJoystick = ((long) CONTROL_LIMIT * actuationLevel) /  joystickLimits[i][MAX_POTI]; 
      if (maxJoystick > joystickLimits[i][MAX])   joystickLimits[i][MAX] = constrain(maxJoystick, 0, CONTROL_LIMIT);
    }

    else
    {
      // Nothing to do... Sensitivitätsgrenzen wurden nicht überschritten, um neue Intervallgrenzen zu übernehmen
    }
  } // end for loop über alle vier Steuerrichtungen
}
// -----------------------------------------------------------------------------------------------------------------------------



// -----------------------------------------------------------------------------------------------------------------------------
void runTestprogram()
{
  const  int actuationPeriod = 5000;  // Periode für Aktuierung einer Achse
  static int activeJoystick  = 0;     // Aktuell aktuierte Achse, wechselt zyklisch
  static int value           = 0;     // Argument für sinusförmige Aktuierung der Ruder

  // Standardwerte: Alle Joysticks in Nulllage
  joysticks[HOEHE] = 0;
  joysticks[SEITE] = 0;
  joysticks[QUER]  = 0;
  joysticks[MOTOR] = 0;
  
  // Sinusförmiger Ausschlag des aktuell aktiven Ruders
  // Auswahl des Ruders: Die Ruder werden der Reihe nach angesteuert
  joysticks[activeJoystick] = (int) (joystickLimits[activeJoystick][MAX] * sin((float)value * 2*M_PI / (float) actuationPeriod));
  
  // Inkrement für nächsten Aufruf und ggf. Wechsel zum nächsten Ruder,
  // falls einmal vollständiger Ausschlag erfolgt ist.
  value++;
  if (value > actuationPeriod)
  {
    value = 0;
    (++activeJoystick) %= 3;
  }  
}
// -----------------------------------------------------------------------------------------------------------------------------
