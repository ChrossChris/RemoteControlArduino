// -----------------------------------------------------------------------------------------------------------------------------
void setJoystickLimits(const bool first_call)
{
  char string[150];
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
      if (  (  actuationLevel > minPositiveActuationLevel                  ) 
         || ( (actuationLevel < minNegativeActuationLevel) && (i != MOTOR) )  ) 
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
    else if (actuationLevel < minNegativeActuationLevel)
    {
      int minJoystick = ((long) CONTROL_LIMIT * actuationLevel) / -joystickLimits[i][MIN_POTI]; 
      if (minJoystick < joystickLimits[i][MIN])   joystickLimits[i][MIN] = constrain(minJoystick, -CONTROL_LIMIT, 0);
    }
    
    // Maximalen Ausschlag bestimmen, Begrenzung auf 100% (Integergrenzen werden im 
    // Zwischenergebnis überschritten, daher Erweiterung auf Long-Integer)
    // (else if kein Ausschluss zu minimalem Ausschlag, da der Joystick immer nur in
    // die eine oder andere Richtung weisen kann, aber nie beide gleichzeitig auslenken
    // kann.)
    else if (actuationLevel > minPositiveActuationLevel)
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

// -----------------------------------------------------------------------------------------
// Modellkonfiguration und Grenzen werden neu gesetzt
void setModel(int selectedModel)
{
  // Auswahl Multiplex Heron (und Modellauswahl wurde geändert)
  if (selectedModel == MULTIPLEX_HERON)
  {
    // Joystickgrenzen / Ruderausschläge
    joystickLimits[HOEHE][MIN] = MULTIPLEX_HERON_HOEHE_MIN * (CONTROL_LIMIT / 100);
    joystickLimits[HOEHE][MAX] = MULTIPLEX_HERON_HOEHE_MAX * (CONTROL_LIMIT / 100);
    joystickLimits[SEITE][MIN] = MULTIPLEX_HERON_SEITE_MIN * (CONTROL_LIMIT / 100);
    joystickLimits[SEITE][MAX] = MULTIPLEX_HERON_SEITE_MAX * (CONTROL_LIMIT / 100);
    joystickLimits[QUER][MIN]  = MULTIPLEX_HERON_QUER_MIN  * (CONTROL_LIMIT / 100);
    joystickLimits[QUER][MAX]  = MULTIPLEX_HERON_QUER_MAX  * (CONTROL_LIMIT / 100);
    joystickLimits[MOTOR][MIN] = MULTIPLEX_HERON_MOTOR_MIN * (CONTROL_LIMIT / 100);
    joystickLimits[MOTOR][MAX] = MULTIPLEX_HERON_MOTOR_MAX * (CONTROL_LIMIT / 100);
    
    // Konfiguration: Expo- und Mischanteile
    butterflyMixer   = MULTIPLEX_HERON_BUTTERFLY;
    querSeiteMischer = MULTIPLEX_HERON_QUER_SEITE_MISCHER;
    flapHoeheMischer = MULTIPLEX_HERON_FLAP_HOEHE_MISCHER;
    motrHoeheMischer = MULTIPLEX_HERON_MOTR_HOEHE_MISCHER;
    motrHoeheTotzone = MULTIPLEX_HERON_MOTR_HOEHE_TOTZONE;  
    querFlapsMischer = MULTIPLEX_HERON_QUER_FLAPS_MISCHER;
    expoQuer         = MULTIPLEX_HERON_EXPO_QUER;
    expoHoehe        = MULTIPLEX_HERON_EXPO_HOEHE;
    expoSeite        = MULTIPLEX_HERON_EXPO_SEITE;

    // Anlenkungsrichtung der Ruder entsprechend des Startmodells
    flipHoehe        = MULTIPLEX_HERON_FLIP_HOEHE;
    flipSeite        = MULTIPLEX_HERON_FLIP_SEITE;
    flipQuerLinks    = MULTIPLEX_HERON_FLIP_QUER_LINKS;
    flipQuerRechts   = MULTIPLEX_HERON_FLIP_QUER_RECHTS;
    flipFlapLinks    = MULTIPLEX_HERON_FLIP_FLAP_LINKS;
    flipFlapRechts   = MULTIPLEX_HERON_FLIP_FLAP_RECHTS;
  }
  
  // Auswahl: D-Power Streamline 270X (und Modellauswahl wurde geändert), dann Rücksetzen der Limits
  else if (selectedModel == DPOWER_STREAMLINE)
  {
    // Joystickgrenzen / Ruderausschläge
    joystickLimits[HOEHE][MIN] = DPOWER_STREAMLINE_HOEHE_MIN * (CONTROL_LIMIT / 100);
    joystickLimits[HOEHE][MAX] = DPOWER_STREAMLINE_HOEHE_MAX * (CONTROL_LIMIT / 100);
    joystickLimits[SEITE][MIN] = DPOWER_STREAMLINE_SEITE_MIN * (CONTROL_LIMIT / 100);
    joystickLimits[SEITE][MAX] = DPOWER_STREAMLINE_SEITE_MAX * (CONTROL_LIMIT / 100);
    joystickLimits[QUER][MIN]  = DPOWER_STREAMLINE_QUER_MIN  * (CONTROL_LIMIT / 100);
    joystickLimits[QUER][MAX]  = DPOWER_STREAMLINE_QUER_MAX  * (CONTROL_LIMIT / 100);
    joystickLimits[MOTOR][MIN] = DPOWER_STREAMLINE_MOTOR_MIN * (CONTROL_LIMIT / 100);
    joystickLimits[MOTOR][MAX] = DPOWER_STREAMLINE_MOTOR_MAX * (CONTROL_LIMIT / 100);
    
    // Konfiguration: Expo- und Mischanteile
    butterflyMixer   = DPOWER_STREAMLINE_BUTTERFLY;
    querSeiteMischer = DPOWER_STREAMLINE_QUER_SEITE_MISCHER;
    flapHoeheMischer = DPOWER_STREAMLINE_FLAP_HOEHE_MISCHER;
    motrHoeheMischer = DPOWER_STREAMLINE_MOTR_HOEHE_MISCHER;
    motrHoeheTotzone = DPOWER_STREAMLINE_MOTR_HOEHE_TOTZONE;  
    querFlapsMischer = DPOWER_STREAMLINE_QUER_FLAPS_MISCHER;
    expoQuer         = DPOWER_STREAMLINE_EXPO_QUER;
    expoHoehe        = DPOWER_STREAMLINE_EXPO_HOEHE;
    expoSeite        = DPOWER_STREAMLINE_EXPO_SEITE;

    // Anlenkungsrichtung der Ruder entsprechend des Startmodells
    flipHoehe        = DPOWER_STREAMLINE_FLIP_HOEHE;
    flipSeite        = DPOWER_STREAMLINE_FLIP_SEITE;
    flipQuerLinks    = DPOWER_STREAMLINE_FLIP_QUER_LINKS;
    flipQuerRechts   = DPOWER_STREAMLINE_FLIP_QUER_RECHTS;
    flipFlapLinks    = DPOWER_STREAMLINE_FLIP_FLAP_LINKS;
    flipFlapRechts   = DPOWER_STREAMLINE_FLIP_FLAP_RECHTS;
  }

  // Auswahl: Multiplex EasyGlider (und Modellauswahl wurde geändert), dann Rücksetzen der Limits
  else if (selectedModel == MULTIPLEX_EASYGLIDER)
  {
    // Joystickgrenzen / Ruderausschläge
    joystickLimits[HOEHE][MIN] = MULTIPLEX_EASYGLIDER_HOEHE_MIN * (CONTROL_LIMIT / 100);
    joystickLimits[HOEHE][MAX] = MULTIPLEX_EASYGLIDER_HOEHE_MAX * (CONTROL_LIMIT / 100);
    joystickLimits[SEITE][MIN] = MULTIPLEX_EASYGLIDER_SEITE_MIN * (CONTROL_LIMIT / 100);
    joystickLimits[SEITE][MAX] = MULTIPLEX_EASYGLIDER_SEITE_MAX * (CONTROL_LIMIT / 100);
    joystickLimits[QUER][MIN]  = MULTIPLEX_EASYGLIDER_QUER_MIN  * (CONTROL_LIMIT / 100);
    joystickLimits[QUER][MAX]  = MULTIPLEX_EASYGLIDER_QUER_MAX  * (CONTROL_LIMIT / 100);
    joystickLimits[MOTOR][MIN] = MULTIPLEX_EASYGLIDER_MOTOR_MIN * (CONTROL_LIMIT / 100);
    joystickLimits[MOTOR][MAX] = MULTIPLEX_EASYGLIDER_MOTOR_MAX * (CONTROL_LIMIT / 100);
    
    // Konfiguration: Expo- und Mischanteile
    butterflyMixer   = MULTIPLEX_EASYGLIDER_BUTTERFLY;
    querSeiteMischer = MULTIPLEX_EASYGLIDER_QUER_SEITE_MISCHER;
    flapHoeheMischer = MULTIPLEX_EASYGLIDER_FLAP_HOEHE_MISCHER;
    motrHoeheMischer = MULTIPLEX_EASYGLIDER_MOTR_HOEHE_MISCHER;
    motrHoeheTotzone = MULTIPLEX_EASYGLIDER_MOTR_HOEHE_TOTZONE;  
    querFlapsMischer = MULTIPLEX_EASYGLIDER_QUER_FLAPS_MISCHER;
    expoQuer         = MULTIPLEX_EASYGLIDER_EXPO_QUER;
    expoHoehe        = MULTIPLEX_EASYGLIDER_EXPO_HOEHE;
    expoSeite        = MULTIPLEX_EASYGLIDER_EXPO_SEITE;

    // Anlenkungsrichtung der Ruder entsprechend des Startmodells
    flipHoehe        = MULTIPLEX_EASYGLIDER_FLIP_HOEHE;
    flipSeite        = MULTIPLEX_EASYGLIDER_FLIP_SEITE;
    flipQuerLinks    = MULTIPLEX_EASYGLIDER_FLIP_QUER_LINKS;
    flipQuerRechts   = MULTIPLEX_EASYGLIDER_FLIP_QUER_RECHTS;
    flipFlapLinks    = MULTIPLEX_EASYGLIDER_FLIP_FLAP_LINKS;
    flipFlapRechts   = MULTIPLEX_EASYGLIDER_FLIP_FLAP_RECHTS;
  }

  // Auswahl: Graupner Amigo 4 (und Modellauswahl wurde geändert), dann Rücksetzen der Limits
  else if (selectedModel == GRAUPNER_AMIGO4)
  {
    // Joystickgrenzen / Ruderausschläge
    joystickLimits[HOEHE][MIN] = GRAUPNER_AMIGO4_HOEHE_MIN * (CONTROL_LIMIT / 100);
    joystickLimits[HOEHE][MAX] = GRAUPNER_AMIGO4_HOEHE_MAX * (CONTROL_LIMIT / 100);
    joystickLimits[SEITE][MIN] = GRAUPNER_AMIGO4_SEITE_MIN * (CONTROL_LIMIT / 100);
    joystickLimits[SEITE][MAX] = GRAUPNER_AMIGO4_SEITE_MAX * (CONTROL_LIMIT / 100);
    joystickLimits[QUER][MIN]  = GRAUPNER_AMIGO4_QUER_MIN  * (CONTROL_LIMIT / 100);
    joystickLimits[QUER][MAX]  = GRAUPNER_AMIGO4_QUER_MAX  * (CONTROL_LIMIT / 100);
    joystickLimits[MOTOR][MIN] = GRAUPNER_AMIGO4_MOTOR_MIN * (CONTROL_LIMIT / 100);
    joystickLimits[MOTOR][MAX] = GRAUPNER_AMIGO4_MOTOR_MAX * (CONTROL_LIMIT / 100);

    // Konfiguration: Expo- und Mischanteile
    butterflyMixer   = GRAUPNER_AMIGO4_BUTTERFLY;
    querSeiteMischer = GRAUPNER_AMIGO4_QUER_SEITE_MISCHER;
    flapHoeheMischer = GRAUPNER_AMIGO4_FLAP_HOEHE_MISCHER;
    motrHoeheMischer = GRAUPNER_AMIGO4_MOTR_HOEHE_MISCHER;
    motrHoeheTotzone = GRAUPNER_AMIGO4_MOTR_HOEHE_TOTZONE;  
    querFlapsMischer = GRAUPNER_AMIGO4_QUER_FLAPS_MISCHER;
    expoQuer         = GRAUPNER_AMIGO4_EXPO_QUER;
    expoHoehe        = GRAUPNER_AMIGO4_EXPO_HOEHE;
    expoSeite        = GRAUPNER_AMIGO4_EXPO_SEITE;
         
    // Anlenkungsrichtung der Ruder entsprechend des Startmodells
    flipHoehe       = GRAUPNER_AMIGO4_FLIP_HOEHE;
    flipSeite       = GRAUPNER_AMIGO4_FLIP_SEITE;
    flipQuerLinks   = GRAUPNER_AMIGO4_FLIP_QUER_LINKS;
    flipQuerRechts  = GRAUPNER_AMIGO4_FLIP_QUER_RECHTS;
    flipFlapLinks   = GRAUPNER_AMIGO4_FLIP_FLAP_LINKS;
    flipFlapRechts  = GRAUPNER_AMIGO4_FLIP_FLAP_RECHTS;
  }
}
// -----------------------------------------------------------------------------------------
