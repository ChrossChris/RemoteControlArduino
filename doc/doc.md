# Initial Documentation

just German, just forum article from Mikrocontroller.net


ich habe für mein RC-Flugmodell meine alte Fernbedienung umgebaut bzw.
entkernt und bis auf das Sendemodul komplett auf Basis von zwei Arduino
Nanos neu aufgebaut. Dazu habe ich sie mit zusätzlichen Potis und
Schaltern erweitert, um die einzelnen Ruder zu trimmen, Steuerkennlinien
zu verschieben und miteinander zu mischen. Die gesamte Steuerlogik habe
ich auf einem Arduino Nano programmiert. Dieser liest permanent alle
Joysticks, Potis und Schalter ein, verrechnet sie entsprechend der
Konfiguration miteinander und erzeugt das PPM-Signal am Ausgang, der mit
dem HF-Sendemodul verbunden ist. Der zweite Arduinos ist ausschließlich
zur Ansteuerung der Displays vorgesehen. Beide kommunizieren
unidirektional über die serielle Schnittstelle miteinander, sodass der
erste Arduino nie auf den zweiten warten muss und somit von diesem der
Sicherheit halber vollständig unabhängig ist. Mit den angehängten
Bildern könnt ihr euch ein besseres Bild machen:
* Fernsteuerung__aussen_.jpg
* Fernsteuerung__innen_.jpg
* Hautplatine.png
Die Arduinos werden separat über eine Powerbank versorgt, die ich von
außen an die Fernbedienung hänge, um die Spannungsversorgung des
Sendemoduls nicht zu beeinflussen (kann man sicher geschickter lösen für
die Zukunft). Die Software ist nach diversen Iterationen mittlerweile
sehr stabil und ausgereift. Das PPM-Signal wird sauber erzeugt und ist
zuverlässig. Ich bin das ganze Jahr über mit der Funke geflogen.