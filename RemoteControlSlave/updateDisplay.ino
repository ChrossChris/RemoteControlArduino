void updateDisplay(const byte lcdAddress, const char displayText[DISPLAY_HEIGHT][DISPLAY_WIDTH+1])
{
   
  //                                                               01234567890123456789
  static char displayTextRight[DISPLAY_HEIGHT][DISPLAY_WIDTH+1] = { "                    ",
                                                                  "                    ",
                                                                  "                    ",
                                                                  "                    " };
  static char displayTextLeft[DISPLAY_HEIGHT][DISPLAY_WIDTH+1]  = { "                    ",
                                                                  "                    ",
                                                                  "                    ",
                                                                  "                    " };  
  // Das komplette Display wird zyklisch Zeichen für Zeichen neu geschrieben,
  // falls es unverändert wurde, da sich Störungen in der Darstellung einschleichen,
  // die nicht mehr überschrieben werden (Empfindlichkeit I2C-Bus?)
  static int resetCounter = 0;

  // Zählt die Anzahl geänderten Zeichen. Durch Begrenzung auf maximale Anzahl pro Zyklus, wird ebenfalls
  // unnötiger Traffic in einem Zyklus vermieden und auf mehrere verteilt.
  int updateCounter = 0;

  for (int i = 0; i < DISPLAY_HEIGHT; i++)
  {
    for (int j = 0; j < DISPLAY_WIDTH; j++) 
    {
      const int position = i*DISPLAY_WIDTH + j;
      if (  ((lcdAddress  == DISPLAY_RIGHT) && (displayText[i][j] != displayTextRight[i][j]))
         || (resetCounter == position ) )
      {
        // Aktulisierung nur der aktuellen Cursor-Position im rechten Dispaly
        // (Vermeidung von unnötigen Traffic auf I2C-Buss)
        lcdRight.setCursor(j,i);
        lcdRight.print(displayText[i][j]);    
        displayTextRight[i][j] = displayText[i][j];
        updateCounter++;
      }
      else if (  ((lcdAddress  == DISPLAY_LEFT) && (displayText[i][j] != displayTextLeft[i][j]))
              || (resetCounter == position ) )
      {
        // Aktulisierung nur der aktuellen Cursor-Position im linken Display
        // (Vermeidung von unnötigen Traffic auf I2C-Buss)
        lcdLeft.setCursor(j,i);
        lcdLeft.print(displayText[i][j]);    
        displayTextLeft[i][j] = displayText[i][j];
        updateCounter++;
      }
      if (updateCounter > 20) return;
    }
  }

  // Counter wird nach Durchlauf über alle Zeichen des linken und rechten Displays zurückgesetzt,
  (++resetCounter) %= DISPLAY_WIDTH*DISPLAY_HEIGHT; 
}
