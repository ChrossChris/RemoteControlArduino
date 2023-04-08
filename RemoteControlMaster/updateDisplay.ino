// -----------------------------------------------------------------------------------------
// Hilfsfunktionen zur Anzeige des Status auf dem Display mithilfe des zweiten Arduino
void updateDisplay()
{ 
  // Nächste Aktualisierungszeitpunkt
  static unsigned long updateTime = millis() + UPDATE_PERIOD;

  // Aktualisierung des Displays nur alle UPDATE_PERIOD, ansonsten
  // funktioniert die Kommunikation über die Schnittstelle nicht
  if (millis() > updateTime)
  {
    byte i = 0;
    byte  values[60];
    values[i++] = model                 // Bit 0,1:   Auswahl Modell, max. 4,
                | (accu << 2)           // Bit 2,3,4: Auswahl Akku, max. 8,
                | (statusScreen << 5);  // Bit 5,6,7: Auswahl Status-Screens, max. 8
    values[i++] = remoteControlSetting;
    const unsigned int throttleTotal_s = throttleTotal / 1000; // Umrechnung der Gesamtgasstellung in %s -> Teiler 1000
    values[i++] = highByte((unsigned int)  throttleTotal_s);
    values[i++] = lowByte( (unsigned int)  throttleTotal_s);
    const int accuDischargeTotal_mAh = accuDischargeTotal / 3600; // Umrechnung der Entlademenge in mAh -> Teiler 3600
    values[i++] = highByte((int) accuDischargeTotal_mAh);
    values[i++] = lowByte( (int) accuDischargeTotal_mAh);

    byte signs = 0;
    if (trimmHoehe       < 0) bitSet(signs,SIGNUM_TRIMM_HOEHE);
    if (trimmQuer        < 0) bitSet(signs,SIGNUM_TRIMM_QUER);
    if (querSeiteMischer < 0) bitSet(signs,SIGNUM_QUER_SEITE_MISCHER);
    if (querFlapsMischer < 0) bitSet(signs,SIGNUM_QUER_FLAPS_MISCHER);
    if (flaps            < 0) bitSet(signs,SIGNUM_FLAPS);
    if (flapHoeheMischer < 0) bitSet(signs,SIGNUM_FLAPS_HOEHE_MISCHER);
    values[i++] = signs;
    values[i++] = lowByte(abs(trimmHoehe)); 
    values[i++] = lowByte(abs((25*expoHoehe)/255)); // Skalierung bis maximalen Integergrenze 25*1024 < 2^16-1
    values[i++] = lowByte(abs(trimmQuer));
    values[i++] = lowByte(abs((25*expoQuer)/255));  // Skalierung bis maximalen Integergrenze 25*1024 < 2^16-1
    values[i++] = lowByte(abs(querSeiteMischer));
    values[i++] = lowByte(abs((25*expoSeite)/255)); // Skalierung bis maximalen Integergrenze 25*1024 < 2^16-1
    values[i++] = lowByte(abs(querFlapsMischer));
    values[i++] = lowByte(abs(flaps));
    values[i++] = lowByte(abs(flapHoeheMischer));
    values[i++] = lowByte(abs(motrHoeheMischer));
    
    for (int idx = 0; idx < CHANNELS; idx++)
    {
      values[i++] = lowByte(joysticks[idx]/10 + 100);           // Umwandeln in Prozent und ohne Vorzeichen
      values[i++] = lowByte(joystickLimits[idx][MIN]/10 + 100); // Umwandeln in Prozent und ohne Vorzeichen
      values[i++] = lowByte(joystickLimits[idx][MAX]/10 + 100); // Umwandeln in Prozent und ohne Vorzeichen
    }
    unsigned int runningTime = ((millis() - startTime) / 1000L); // Time since start in seconds
    values[i++] = highByte(runningTime);
    values[i++] = lowByte(runningTime);

    // Senden des Datenblocks
    Serial.write(SERIAL_START);
    Serial.write(SERIAL_START);
    Serial.write(i + 2); 
    for (int idx = 0; idx < i; idx++)  Serial.write(values[idx]);
    Serial.write(SERIAL_END);
    Serial.write(SERIAL_END);

    // Nächste Aktualisierung des Displays bestimmen
    updateTime = millis() + UPDATE_PERIOD;
  }
}
// -----------------------------------------------------------------------------------------


void updateDisplayDebug()
{

  const bool switchLinksOben   = !digitalRead(SWITCH_LINKS_OBEN);  // Korrektur nötig, da Schalter links invers verbaut ist
  const bool switchLinksUnten  =  digitalRead(SWITCH_LINKS_UNTEN); 
  const bool switchRechtsOben  =  digitalRead(SWITCH_RECHTS_OBEN);
  const bool switchRechtsUnten =  digitalRead(SWITCH_RECHTS_UNTEN);

  const int potiLinks1   = analogRead(POTI_LINKS_1);
  const int potiLinks2   = analogRead(POTI_LINKS_2);
  const int potiRechts1  = analogRead(POTI_RECHTS_1);
  const int potiRechts2  = analogRead(POTI_RECHTS_2);

  char out[160];

  sprintf(out,"SW lo: %d   SW lu: %d   SW ro: %d   SW ru: %d   Poti l1: %4d   Poti l2: %4d   Poti r1: %4d   Poti r2: %4d\n",
    switchLinksOben, switchLinksUnten, switchRechtsOben, switchRechtsUnten,
    potiLinks1,      potiLinks2,       potiRechts1,      potiRechts2);

  
  Serial.print(out);
}
// -----------------------------------------------------------------------------------------
