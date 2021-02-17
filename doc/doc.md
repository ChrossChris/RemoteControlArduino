# Initial Documentation

just German, just forum article from Mikrocontroller.net


ich habe f�r mein RC-Flugmodell meine alte Fernbedienung umgebaut bzw.
entkernt und bis auf das Sendemodul komplett auf Basis von zwei Arduino
Nanos neu aufgebaut. Dazu habe ich sie mit zus�tzlichen Potis und
Schaltern erweitert, um die einzelnen Ruder zu trimmen, Steuerkennlinien
zu verschieben und miteinander zu mischen. Die gesamte Steuerlogik habe
ich auf einem Arduino Nano programmiert. Dieser liest permanent alle
Joysticks, Potis und Schalter ein, verrechnet sie entsprechend der
Konfiguration miteinander und erzeugt das PPM-Signal am Ausgang, der mit
dem HF-Sendemodul verbunden ist. Der zweite Arduinos ist ausschlie�lich
zur Ansteuerung der Displays vorgesehen. Beide kommunizieren
unidirektional �ber die serielle Schnittstelle miteinander, sodass der
erste Arduino nie auf den zweiten warten muss und somit von diesem der
Sicherheit halber vollst�ndig unabh�ngig ist. Mit den angeh�ngten
Bildern k�nnt ihr euch ein besseres Bild machen:
* Fernsteuerung__aussen_.jpg
* Fernsteuerung__innen_.jpg
* Hautplatine.png
Die Arduinos werden separat �ber eine Powerbank versorgt, die ich von
au�en an die Fernbedienung h�nge, um die Spannungsversorgung des
Sendemoduls nicht zu beeinflussen (kann man sicher geschickter l�sen f�r
die Zukunft). Die Software ist nach diversen Iterationen mittlerweile
sehr stabil und ausgereift. Das PPM-Signal wird sauber erzeugt und ist
zuverl�ssig. Ich bin das ganze Jahr �ber mit der Funke geflogen.