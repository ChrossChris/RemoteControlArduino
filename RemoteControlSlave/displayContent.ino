// ------------------------------------------------------------------------------------------
// Screen Status: Aktueller Status von Laufzeit und Akkuladezustand
// ------------------------------------------------------------------------------------------
void displayStatus(const byte lcdAddress)
{
  // Darzustellender Inhalt wird vorgehalten
  char info[DISPLAY_HEIGHT][DISPLAY_WIDTH];

  // Laufzeit, Gas akkumuliert
  //               01234567890123456789
  sprintf(info[0],"Zeit:       %3dm:%02ds", (runningTimeSec / 60), (runningTimeSec % 60));

  //               01234567890123456789
  sprintf(info[1],"Accu: LiPo%ds %4dmAh  %2ds", accuCells, capacity);
  sprintf(info[2],"%5u  %4dmAh  %3d%%", throttleTotal, powerStateAccu, power);
  if (power > 0) setLevelMeter(info[3], power, 0, 100, 20);
  else
  { 
    //               01234567890123456789
    sprintf(info[3],"!! BATTERY  EMPTY !");
    info[3][DISPLAY_WIDTH-1] = '!';  // Null-Byte am Ende des Strings überschreiben
  }

  updateDisplay(lcdAddress, info);
}
// ------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------
// Screen Joystick-Positionen: Darstellung der Knüppelauslenkungen in allen Achsen
// ------------------------------------------------------------------------------------------
void displayJoystickPosition(const byte lcdAddress)
{
  char info[DISPLAY_HEIGHT][DISPLAY_WIDTH];

  // Joystick: Höhenruder
  int i = 0;
  sprintf(info[i],"H%4d", joysticks[i]);
  if (abs(joysticks[i]) > 5)  setLevelMeter(info[i], joysticks[i], joystickLimits[i][MIN], joystickLimits[i][MAX], 15); 
  else                        setLevelMeter(info[i],            0, joystickLimits[i][MIN], joystickLimits[i][MAX], 15); 
  if (joysticks[i] > -100)    info[i][1] = 'R';

  // Joystick: Seitenruder
  i = 1;
  sprintf(info[i],"S%4d", joysticks[i]);
  if (abs(joysticks[i]) > 5)  setLevelMeter(info[i], joysticks[i], joystickLimits[i][MIN], joystickLimits[i][MAX], 15); 
  else                        setLevelMeter(info[i],            0, joystickLimits[i][MIN], joystickLimits[i][MAX], 15); 
  if (joysticks[i] > -100)    info[i][1] = 'R';

  // Joystick: Querruder
  i = 2;
  sprintf(info[i],"Q%4d", joysticks[i]);
  if (abs(joysticks[i]) > 5)  setLevelMeter(info[i], joysticks[i], joystickLimits[i][MIN], joystickLimits[i][MAX], 15); 
  else                        setLevelMeter(info[i],            0, joystickLimits[i][MIN], joystickLimits[i][MAX], 15); 
  if (joysticks[i] > -100)    info[i][1] = 'R';

  // Joystick: Motor
  i = 3;
  sprintf(info[i],"Mo%3d", joysticks[i]);
  if (abs(joysticks[i]) > 5)  setLevelMeter(info[i], joysticks[i], joystickLimits[i][MIN], joystickLimits[i][MAX], 15); 
  else                        setLevelMeter(info[i],            0, joystickLimits[i][MIN], joystickLimits[i][MAX], 15); 

  updateDisplay(lcdAddress, info);
}
// ------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------
// Screen Joystick-Limits: Begrenzungen der Joysticks und Aktuierungsrichtung
// ------------------------------------------------------------------------------------------
void displayJoystickLimits(const byte lcdAddress)
{
  // Darzustellender Inhalt wird vorgehalten
  char info[DISPLAY_HEIGHT][DISPLAY_WIDTH];

  //                                              01234567890123456789
  if (flipHoehe == 1) sprintf(info[0],"HR ^^ %4d%% vv %4d%%", joystickLimits[0][MIN], joystickLimits[0][MAX]);
  else                sprintf(info[0],"HR vv %4d%% ^^ %4d%%", joystickLimits[0][MAX], joystickLimits[0][MIN]);
  if (flipSeite == 1) sprintf(info[1],"SR << %4d%% >> %4d%%", joystickLimits[1][MIN], joystickLimits[1][MAX]);
  else                sprintf(info[1],"SR >> %4d%% << %4d%%", joystickLimits[1][MAX], joystickLimits[1][MIN]);
  if (flipQuer == 1)  sprintf(info[2],"QR << %4d%% >> %4d%%", joystickLimits[2][MIN], joystickLimits[2][MAX]);
  else                sprintf(info[2],"QR >> %4d%% << %4d%%", joystickLimits[2][MAX], joystickLimits[2][MIN]);
  //               01234567890123456789
  sprintf(info[3],"Motor max.      %3d", joystickLimits[3][MAX]);
  info[3][DISPLAY_WIDTH-1] = '%';

  updateDisplay(lcdAddress, info);
}
// ------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------
// Screen Konfiguration: Auswahl des Akkus und des Modells
// ------------------------------------------------------------------------------------------
void displayConfiguration(const byte lcdAddress)
{
  // Darzustellender Inhalt wird vorgehalten
  char info[DISPLAY_HEIGHT][DISPLAY_WIDTH];

  //               01234567890123456789
  sprintf(info[0],"ExSR ");
  setLevelMeter(info[0], expoSeite, 0, 100, 15);
  sprintf(info[1],"MxMH ");
  setLevelMeter(info[1], motrHoeheMischer, 0, LIMIT_MH_MISCHER, 15);

  //                                                   01234567890123456789
  if (model == MULTIPLEX_HERON)       sprintf(info[2],"Multiplex Heron     ");
  else if (model == GRAUPNER_AMIGO4)  sprintf(info[2],"Graupner Amigo IV   ");
  else                                sprintf(info[2],"Model undefined     ");

  //                                                   01234567890123456789
  if      (accu == ACCU_4s_2400mAh)   sprintf(info[3],"Accu: LiPo4s 2400mAh");
  else if (accu == ACCU_3s_2200mAh)   sprintf(info[3],"Accu: LiPo3s 2200mAh");
  else if (accu == ACCU_3s_1800mAh)   sprintf(info[3],"Accu: LiPo3s 1800mAh");
  else                                sprintf(info[3],"Accu: undefined     ");

  updateDisplay(lcdAddress, info);
}
// ------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------
// Screen Zusatzufunktionen (Numerisch): Numerische Darstellung aller 
// Zusatzfunktionen (Expo, Mixer, etc.)
// ------------------------------------------------------------------------------------------
void displayAddFunctionsNumeric(const byte lcdAddress)
{
  // Darzustellender Inhalt wird vorgehalten
  char info[DISPLAY_HEIGHT][DISPLAY_WIDTH];

  //               01234567890123456789
  sprintf(info[0],"ExHR%4d%% TrHR %4d ",   expoHoehe,        trimmHoehe);
  sprintf(info[1],"ExQR%4d%% TrQR %4d ",   expoQuer,         trimmQuer);
  sprintf(info[2],"ExSR%4d%% MxQS %4d%%",  expoSeite,        querSeiteMischer);
  sprintf(info[3],"MxMH%4d%% MxQF %4d%%",  motrHoeheMischer, querFlapsMischer);
  info[3][DISPLAY_WIDTH-1] = '%'; // Null-Byte am Ende des Strings überschreiben
  updateDisplay(lcdAddress, info);
}
// ------------------------------------------------------------------------------------------


// ------------------------------------------------------------------------------------------
// Screen Zusatzufunktionen (Grafisch): Grafische Darstellung der aktuell aktivierten
// Zusatzfunktionen (Expo, Mixer, etc.)
// ------------------------------------------------------------------------------------------
void displayAddFunctionsGraphic(const byte lcdAddress)
{
  // Darzustellender Inhalt wird vorgehalten
  char info[DISPLAY_HEIGHT][DISPLAY_WIDTH];
  int  i = 0;
                                                             
  if (bitRead(remoteControlSetting, TRIMM_EXPO_HOEHE) == 0)
  {
    sprintf(info[i],"TrHR ");
    setLevelMeter(info[i++], trimmHoehe, -LIMIT_TRIMM, LIMIT_TRIMM, 15);
  }
  else                              
  {
    sprintf(info[i],"ExHR ");
    setLevelMeter(info[i++], expoHoehe, 0, 100, 15);
  }

  if (bitRead(remoteControlSetting, TRIMM_EXPO_QUER) == 0) 
  {
    sprintf(info[i],"TrQR ");
    setLevelMeter(info[i++], trimmQuer,  -LIMIT_TRIMM, LIMIT_TRIMM, 15);
  }
  else                              
  {
    sprintf(info[i],"ExQR ");
    setLevelMeter(info[i++], expoQuer, 0, 100, 15);
  }

  if (bitRead(remoteControlSetting, QUER_SEITE_FLAPS_MISCHER) == 0)
  {
    sprintf(info[i],"MxQS ");
    setLevelMeter(info[i++], querSeiteMischer, -LIMIT_QS_MISCHER, LIMIT_QS_MISCHER, 15);
  }
  else
  {
    sprintf(info[i],"MxQF ");
    setLevelMeter(info[i++], querFlapsMischer, -LIMIT_QF_MISCHER, LIMIT_QF_MISCHER, 15);
  }

  if (bitRead(remoteControlSetting, BUTTERFLY) == 0)
  {
    sprintf(info[i],"Flap ");
    setLevelMeter(info[i++], flaps, -LIMIT_FLAPS, LIMIT_FLAPS, 15);
  }
  else
  {
    sprintf(info[i],"Bfly ");
    setLevelMeter(info[i++], flaps, -LIMIT_FLAPS, LIMIT_FLAPS, 15);
  }
  

  // Null-Byte zur String-Terminierung wieder überschreiben
  for (i = 0; i<4; i++) info[i][4] = ' ';

  updateDisplay(lcdAddress, info);
}
