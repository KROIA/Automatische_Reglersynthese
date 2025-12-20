# Latzel 
---

## Voraussetzung
- System muss open loop stabil sein.
- Sprungantwort muss ermittelt werden können


## Vor/Nach-teile

    + Einfach in der umsetzung.
    + Erfordert kein Modell.
    + Parameter sind einfach zu ermitteln.
    + Ermöglicht die Angabe ob der geschlossene Regelkreis ein Überschwingen von 20% erlaubt oder nicht.
    + Kann auf Führungsverhalten oder Störverhalten ausgelegt werden.

    - Sysytem muss Openloop-Stabil sein.
    - Kann bei experimentellem Tuning zur instabilität des Systemes führen.
    - Kein optimaler Regler
    - Keine Garantie für "kein überschwingen"


---
## Vorgehen
  

### Parameter ablesen
Ks: DC-Gain
t10: Zeit bis y=10%*DC-Gain
t50: Zeit bis y=50%*DC-Gain
t90: Zeit bis y=90%*DC-Gain


### PID auslegen



---
#### Quellen
[Skript RegT 3-4 S135](../RegT34_Skript.pdf)