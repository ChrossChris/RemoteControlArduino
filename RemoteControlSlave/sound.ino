int tickSound(int value, int valueFilter, int minValue, int maxValue, int steps, bool activeTone)
{
  const int scale             = 32767 / maxValue;
  const int delta             = ((long) scale * (maxValue - minValue)) / steps;
  const int center            = ((long) scale * (maxValue + minValue)) / 2;
  const int valueFilterNew    = ((long) scale * (value    - minValue)) / delta;
  const ToneSequence tick[1]  = { TS( 2000,  10, 0) };
  const ToneSequence tack[1]  = { TS( 2000, 200, 0) };
  const ToneSequence tock[1]  = { TS( 2000, 100, 0) };
  if ( activeTone && (valueFilter != valueFilterNew) )
  {
    if      ((valueFilterNew == steps/2)   || (valueFilter == steps/2))     setSound(tock,1);  // Mittelstellung
    else if ((valueFilterNew == 0)         && (valueFilter == 1))           setSound(tack,1);  // Erreichen unterer Anschlag 
    else if ((valueFilterNew == 1)         && (valueFilter == 0))           setSound(tack,1);  // Verlassen unterer Anschlag 
    else if ((valueFilterNew == steps-1)   && (valueFilter == steps-2))     setSound(tack,1);  // Erreichen oberer  Anschlag 
    else if ((valueFilterNew == steps-2)   && (valueFilter == steps-1))     setSound(tack,1);  // Verlassen oberer  Anschlag 
    else if ((valueFilterNew >  0)         && (valueFilter <  steps-1))     setSound(tick,1);  // Wechsel ins nächste Intervall
  }
  return valueFilterNew;
}


void setSound(ToneSequence toneSequence[], int size)
{
  // Initiale Checks
  if (size <  1)               return;
  if (size >= SEQUENCE_LENGTH) size = SEQUENCE_LENGTH;

  const unsigned long now = millis();
  noTone(BUZZER_PORT);
  frequency[0]     = toneSequence[0].frequency;
  tonDuration[0]   = toneSequence[0].duration;
  timeBaseSound[0] = now + 1;  // Wiedergabe startet von jetzt in 1ms
  for (int k = 1; k < size; k++)
  {
    frequency[k]     = toneSequence[k].frequency;
    tonDuration[k]   = toneSequence[k].duration;
    timeBaseSound[k] = timeBaseSound[k-1] + tonDuration[k-1] + toneSequence[k-1].pause;
  }
  for (int k = size; k < SEQUENCE_LENGTH; k++)
  {
    frequency[k]     = 0;
    tonDuration[k]   = 0;
    timeBaseSound[k] = timeBaseSound[k-1];
  }
  pulseIndex = 0;
}


void updateSound()
{
  static unsigned long next = millis();
  unsigned long now         = millis();

  // Initiale Checks
  if  (  (pulseIndex >= SEQUENCE_LENGTH) 
      || (frequency[pulseIndex] == 0)   )
  {
    noTone(BUZZER_PORT);
    return;
  }

  // Erkennung der Flanke für Wiedergabe des nächste neuen Tons
  if (now >= next)
  {
    tone(BUZZER_PORT,frequency[pulseIndex],tonDuration[pulseIndex]);
    pulseIndex++;
    if (pulseIndex < SEQUENCE_LENGTH) next = now + timeBaseSound[pulseIndex] - timeBaseSound[pulseIndex-1];
  }
}

  
