// -----------------------------------------------------------------------------------------
// Modellkonfiguration und Grenzen werden neu gesetzt
void setModel(const int modelSelection)
{
  // Auswahl: Multiplex Heron
  if   (modelSelection < 684)
  {
    // Rücksetzen der Limits, falls das Modell gewechselt wurde
    if (model != MULTIPLEX_HERON)
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
      querFlapsMischer = MULTIPLEX_HERON_QUER_FLAPS_MISCHER;
      expoQuer         = MULTIPLEX_HERON_EXPO_QUER;
      expoHoehe        = MULTIPLEX_HERON_EXPO_HOEHE;
      expoSeite        = MULTIPLEX_HERON_EXPO_SEITE;

      // Anlenkungsrichtung der Ruder entsprechend des Startmodells
      flipHoehe = MULTIPLEX_HERON_FLIP_HOEHE;
      flipSeite = MULTIPLEX_HERON_FLIP_SEITE;
      flipQuer  = MULTIPLEX_HERON_FLIP_QUER;
    }
    model = MULTIPLEX_HERON;
  }
  
  // Auswahl: Graupner Amigo 4
  else 
  {
    // Rücksetzen der Limits, falls das Modell gewechselt wurde
    if (model != GRAUPNER_AMIGO4)
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
      querFlapsMischer = GRAUPNER_AMIGO4_QUER_FLAPS_MISCHER;
      expoQuer         = GRAUPNER_AMIGO4_EXPO_QUER;
      expoHoehe        = GRAUPNER_AMIGO4_EXPO_HOEHE;
      expoSeite        = GRAUPNER_AMIGO4_EXPO_SEITE;
           
      // Anlenkungsrichtung der Ruder entsprechend des Startmodells
      flipHoehe = GRAUPNER_AMIGO4_FLIP_HOEHE;
      flipSeite = GRAUPNER_AMIGO4_FLIP_SEITE;
      flipQuer  = GRAUPNER_AMIGO4_FLIP_QUER;
    }
    model = GRAUPNER_AMIGO4;
  }
}
// -----------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------
// Akku und entsprechende Entladekurven werden gewählt
void setAccu(const int accuSelection)
{
  // Akku-Auswahl
  if      (accuSelection < 341) accu = ACCU_4s_2400mAh;
  else if (accuSelection < 682) accu = ACCU_3s_2200mAh;
  else                          accu = ACCU_3s_1800mAh;
}
// -----------------------------------------------------------------------------------------
