void setLevelMeter(char str[DISPLAY_WIDTH], const int value, const int minValue, const int maxValue, int sizeLevelMeter)
{
  // Initial einmal alles löschen (ging auch durch Auffüllen der übrigen Zeichen, aber
  // Code-Effizienz plus Sonderfall ohne Ausschlag gegenüber Verständlichkeit hat verloren ;-)
  for (int idx = DISPLAY_WIDTH - sizeLevelMeter; idx < DISPLAY_WIDTH; idx++)  str[idx] = ' ';

  // Sinnvoller Wert übergeben, für die die Anzeige generiert werden kann?  
  if      ((sizeLevelMeter < 1) || (sizeLevelMeter > 20))  str = "ERR: levelMeter OoS ";
  else if (minValue >  0 )                                 str = "ERR: minValue > 0   ";
  else if (maxValue <= 0 )                                 str = "ERR: maxValue < 0   "; 

  // Eigentliche Ausgabe 
  else if (minValue < 0)
  {
    if ( (sizeLevelMeter % 2) == 0)  sizeLevelMeter--;
    int i0  = DISPLAY_WIDTH - sizeLevelMeter / 2 - 1;
    str[i0] = '|';
    if (value < 0) 
    {
      int digits = map(abs(value), 0, abs(minValue), 0, sizeLevelMeter/2 + 1);
      if (digits > sizeLevelMeter/2)            digits      = sizeLevelMeter/2;
      for (int idx = 1; idx < digits; idx++)    str[i0-idx] = '-';
      str[i0 - digits] = '<';
    }
    else if (value > 0) 
    {
      int digits = map(value, 0, maxValue, 0, sizeLevelMeter/2 + 1);
      if (digits > sizeLevelMeter/2)            digits      = sizeLevelMeter/2;
      for (int idx = 1; idx < digits; idx++)    str[i0+idx] = '-';
      str[i0 + digits] = '>';
    }
  }

  // Balkenanzeige von null bis zum Maximalwert
  else
  {
    int i0     = DISPLAY_WIDTH - sizeLevelMeter;
    int digits = map(value, minValue, maxValue, 0, sizeLevelMeter*2+1);
    digits     = constrain(digits, 0, sizeLevelMeter*2);
    for (int idx = 0;        idx < digits/2; idx++)        str[i0+idx]        = 0xDB;
    if ((digits % 2) == 1)                                 str[i0 + digits/2] = '>';
  }
}
