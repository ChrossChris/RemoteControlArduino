// -----------------------------------------------------------------------------------------
void setPPM(const byte ruder, int value, int trimmValue)
{ 
  // Berechnung des PPM-Werts, aber noch nicht ins Array schreiben, damit bei Aufruf der ISR (asynchron) kein
  // unvollständiger Wert im Array enthalten ist.
  int ppmValue = PPM_CENTER_VALUE;

  // Sonderfall: Anderer Servo, Signal ist invers (Vermutlich die Ursache, anders nicht erklärbar)
  // Daher wird das PPM-Signal noch einmal überschrieben
  if ((model == MULTIPLEX_HERON) && (ruder == PPM_QUER_LINKS))
  {
    value      = -value;
    trimmValue = -trimmValue;
  }


  if (ruder != PPM_MOTOR)
  {
    if (value < 0) ppmValue  = map(value, -CONTROL_LIMIT, 0, PPM_CENTER_VALUE-PPM_LEVEL,  PPM_CENTER_VALUE+trimmValue);
    else           ppmValue  = map(value,  0, CONTROL_LIMIT, PPM_CENTER_VALUE+trimmValue, PPM_CENTER_VALUE+PPM_LEVEL);
  }
  else             ppmValue = PPM_CENTER_VALUE - PPM_LEVEL + ((long) (2*PPM_LEVEL * value) / CONTROL_LIMIT);

  // Übernahme des gewünschten Servostellung und Absicherung der maximalen Pegelgrenzen
  // (Entsprechend Logik im Code können die Grenzen nicht überschritten werden, aber sicher ist
  // sicher, falls ggf. noch Änderungen im Code erfolgen.)
  ppm[ruder] = constrain(ppmValue, PPM_CENTER_VALUE-PPM_LEVEL, PPM_CENTER_VALUE+PPM_LEVEL);
}
// -----------------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------------
// Interrupt-Service-Routine für PPM-Erzeugung
ISR(TIMER1_COMPA_vect)
{  
  static byte         channel   = 0;    // PPM-Kanal (Servo-Signale werden als Sequenz nacheinander ausgegeben)
  static unsigned int totalTime = 0;    // Gesamtdauer der PPM-Sequenz in Mikrosekunden
  static boolean      pulse     = true; 

  // Überflüssig, da CTC-Mode?
  // TCNT1 = 0;
  
  // Puls oder Pause?
  if (pulse) 
  { 
    digitalWrite(PPM_PORT, HIGH);         // Puls wird gesetzt
    OCR1A = PULSE_LENGTH * MIKRO_SECONDS; // Pulsbreite -> Nachfolgende Interruptauslösung zum Start der Pause des PPM-Signals
    pulse = false;                        // Pause folgt auf Puls im nächsten Aufruf der ISR
  } 
  
  // Pause folgt auf den Puls: Gesamtdauer Puls+Pausenzeit codiert das Stellsignal für den aktuellen PPM-Kanal channel
  else
  { 
    digitalWrite(PPM_PORT, LOW);    // Puls wird zurückgesetzt, Pause im PPM-Signal (Servoauslenkung codiert durch die Länge der Pause)
    pulse = true;                   // Puls folgt auf Pause (im nächsten Aufruf der ISR)

    if (channel >= PPM_CHANNELS)
    {
      channel    = 0;               // Zurücksetzen auf Kanal 0 (Sequenz startet von vorn), vorhergehender Puls somit Ende des PPM-Frames
      totalTime += PULSE_LENGTH;    // Gesamtdauer der PPM-Sequenz plus Dauer des letzen Pulses, der das PPM-Signal abschließt
      OCR1A      = (FRAME_LENGTH - totalTime) * MIKRO_SECONDS; // Pausenzeit bis zum Start der neuen PPM-Sequenz, festes Raster durch FRAME_LENGTH gegeben.
      totalTime  = 0;               // Zurücksetzen der Gesamtdauer für die neue PPM-Sequenz
    }
    else
    {
      OCR1A      = (ppm[channel] - PULSE_LENGTH) * MIKRO_SECONDS; // Festlegen der Pulspause für den aktuellen PPM-Kanal, Pulslänge zählt dabei in der Codierung mit
      totalTime += ppm[channel];    // Sukzessive Aufsummierung der Zeitlänge der gesamten PPM-Sequenz mit jedem Kanal, Puls zu Beginn wird dabei mitgezählt
      channel++;                    // Hochzählen des Kanals für nächsten Servo in der PPM-Sequenz -> Vorbereitung für den nächsten Aufruf der ISR
    }     
  }

}
// -----------------------------------------------------------------------------------------
