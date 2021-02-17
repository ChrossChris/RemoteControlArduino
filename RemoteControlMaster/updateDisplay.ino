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
    values[i++] = highByte((unsigned int)  throttleTotal);
    values[i++] = lowByte( (unsigned int)  throttleTotal);
    values[i++] = highByte((int) powerStateAccu);
    values[i++] = lowByte( (int) powerStateAccu);

    byte signs = 0;
    if (trimmHoehe       < 0) bitSet(signs,SIGNUM_TRIMM_HOEHE);
    if (trimmQuer        < 0) bitSet(signs,SIGNUM_TRIMM_QUER);
    if (querSeiteMischer < 0) bitSet(signs,SIGNUM_QUER_SEITE_MISCHER);
    if (querFlapsMischer < 0) bitSet(signs,SIGNUM_QUER_FLAPS_MISCHER);
    if (flaps            < 0) bitSet(signs,SIGNUM_FLAPS);
    if (flapHoeheMischer < 0) bitSet(signs,SIGNUM_FLAPS_HOEHE_MISCHER);
    values[i++] = signs;
    values[i++] = lowByte(abs(trimmHoehe)); 
    values[i++] = lowByte(abs((25*expoHoehe)/255)); 
    values[i++] = lowByte(abs(trimmQuer));
    values[i++] = lowByte(abs((25*expoQuer)/255));
    values[i++] = lowByte(abs(querSeiteMischer));
    values[i++] = lowByte(abs((25*expoSeite)/255));
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


// -----------------------------------------------------------------------------------------
// Hilfsfunktionen zur Anzeige des Status auf dem Display mithilfe des zweiten Arduino
//void updateDisplayInteger()
//{ 
//  // Nächste Aktualisierungszeitpunkt
//  static unsigned long updateTime = millis() + UPDATE_PERIOD;
//
//  // Aktualisierung des Displays nur alle UPDATE_PERIOD, ansonsten
//  // funktioniert die Kommunikation über die Schnittstelle nicht
//  if (millis() > updateTime)
//  {
//    byte i = 0;
//    int  values[60];
//    values[i++] = word(model, statusScreen);
//    values[i++] = word(accu, remoteControlSetting);
//    values[i++] = (unsigned int)  throttleTotal;
//    values[i++] = (int) powerStateAccu;
//
//    byte signs = 0;
//    if (trimmHoehe       < 0) bitSet(signs,SIGNUM_TRIMM_HOEHE);
//    if (trimmQuer        < 0) bitSet(signs,SIGNUM_TRIMM_QUER);
//    if (querSeiteMischer < 0) bitSet(signs,SIGNUM_QUER_SEITE_MISCHER);
//    if (querFlapsMischer < 0) bitSet(signs,SIGNUM_QUER_FLAPS_MISCHER);
//    if (flaps            < 0) bitSet(signs,SIGNUM_FLAPS);
//    if (flapHoeheMischer < 0) bitSet(signs,SIGNUM_FLAPS_HOEHE_MISCHER);
//
//    values[i++] = word(lowByte(abs(trimmHoehe)),        lowByte(abs((25*expoHoehe)/255))); 
//    values[i++] = word(lowByte(abs(trimmQuer)),         lowByte(abs((25*expoQuer )/255)));
//    values[i++] = word(lowByte(abs(querSeiteMischer)),  lowByte(abs((25*expoSeite)/255)));
//    values[i++] = word(lowByte(abs(querFlapsMischer)),  lowByte(abs(flaps)));
//    values[i++] = word(lowByte(abs(flapHoeheMischer)),  signs);
//    
//    for (int idx = 0; idx < CHANNELS; idx++)
//    {
//      values[i++] = joysticks[idx];
//      values[i++] = joystickLimits[idx][MIN];
//      values[i++] = joystickLimits[idx][MAX];
//    }
//    values[i++] = (unsigned int)  ((millis() - startTime) / 1000L); // Time since start in seconds
//    sendPackage(0, i, values);  
//
//    // Nächste Aktualisierung des Displays bestimmen
//    updateTime = millis() + UPDATE_PERIOD;
//  }
//}
//// -----------------------------------------------------------------------------------------


//// -----------------------------------------------------------------------------------------
//// Senden eines Integer-Pakets über die serielle Schnittstelle
//void sendPackage(const byte id, const byte amount, const int* values)  
//{
//  Serial.write(SERIAL_START);
//  Serial.write(SERIAL_START);
//  Serial.write(amount*2 + 2); 
//  for (int idx = 0; idx < amount; idx++)
//  {
//    Serial.write(highByte(values[idx]));
//    Serial.write( lowByte(values[idx]));
//  }
//  Serial.write(SERIAL_END);
//  Serial.write(SERIAL_END);
//}


void sendPackageDebug(const byte id, const byte amount, const int* values)  
{
  Serial.print("Status-Screen: ");
  Serial.print(statusScreen);
  Serial.print("\n");
//  Serial.print("FH-Mischer: ");
//  Serial.print(flapHoeheMischer);
//  Serial.print("\tFlaps: ");
//  Serial.print(flaps);
//  Serial.print("\tTrimm: ");
//  Serial.print(trimmHoehe);
//  Serial.print("\tMisch: ");
//  Serial.print(hoeheMisch);
//  Serial.print("\tHoehe: ");
//  Serial.print(joysticks[HOEHE]);
//  Serial.print("\tQS-Mischer: ");
//  Serial.print(querSeiteMischer);
//  Serial.print("\tQuer: ");
//  Serial.print(joysticks[QUER]);
//  Serial.print("\tMisch: ");
//  Serial.print(seiteMisch);
//  Serial.print("\tSeite: ");
//  Serial.print(joysticks[SEITE]);
//  Serial.print("\n");
  
  
//  Serial.print("Start-Byte: ");
//
//  
//  Serial.print(SERIAL_START);
//  Serial.print(" ");
//  Serial.print(SERIAL_START);
//  Serial.print(" Size: ");
//  Serial.print(amount*2 + 2);
//  Serial.print(" \t");
//  for (int idx = 0; idx < 10; idx++)
//  {
//    Serial.print(idx);
//    Serial.print(". ");
//    Serial.print("[");
//    Serial.print(values[idx]);
//    Serial.print("] \t");
//  }
//  Serial.print("Stop: ");
//  Serial.print(SERIAL_END);
//  Serial.print(" ");
//  Serial.print(SERIAL_END);
//  Serial.print("\n");
}
// -----------------------------------------------------------------------------------------
