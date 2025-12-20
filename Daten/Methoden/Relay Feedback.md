# Relay Feedback

! Adaptive control ! -> weg lassen

---
## Grundidee

Statt einen PID-Regler einzustellen, schließt man die Regelstrecke in einen Regelkreis mit einem Relay (Schalter), das nur zwischen +h und −h ausgibt (also wie ein Zweipunktregler).
Dieses Relay zwingt die Strecke in stabile Dauerschwingungen (Grenzzyklus).
Aus diesen Schwingungen lassen sich wichtige Kennwerte der Strecke ableiten:
- die kritische Periodendauer Tu
- die Amplitude der Schwingungen

Mit diesen Werten kann man nach der Ziegler-Nichols-Methode oder anderen Tuning-Regeln die PID-Parameter berechnen.

## Voraussetzung
- 

## Vor/Nach-teile

    + Kein detailliertes Modell der Strecke notwendig.
    + Funktioniert auch bei unbekannten Prozessen.
    + Vollautomatisierbar

    - Der Prozess muss kurzzeitig in Schwingungen gezwungen werden -> kann bei kritischen Anlagen problematisch sein.
    - Funktioniert nur, wenn die Strecke stabil schwingen kann (monotone Prozesse mit Integratoren oder Totzeit sind manchmal schwierig).


---
## Vorgehen


### Parameter ablesen



### PID auslegen




---
#### Quellen
[RelayFeedbackMethode.pdf](../RelayFeedbackMethode.pdf)
[https://chatgpt.com](https://chatgpt.com)