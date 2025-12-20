# ZieglerNichols 2
---

## Voraussetzung
- System muss open loop stabil sein.
- Impulsantwort muss ermittelt werden können
- System muss mit einem P-Regler in den grenzstabilen Zustand gebracht werden können.

## Vor/Nach-teile

    + Einfach in der umsetzung.
    + Erfordert kein modell.
    + Parameter sind einfach zu ermitteln.

    - Sysytem muss Openloop-Stabil sein.
    - Kein optimaler Regler
    - Keine Garantie für "kein überschwingen"



---
## Vorgehen
- Geschlossener Regelkreis mit einem P-Regler versehen.
- P-Anteil erhöhen bis Konstante Schwingung erreicht

### Parameter ablesen
- Schwingungsperiode ablesen
- Kp = 0.6 * (P-Regler-Gain)
- Ti = 0.5 * (Schwingungsperiode) 
- Td = 0.125 * (Schwingungsperiode)


### PID auslegen
Regler: R(s)=(Kp + Kp * Ti * s + Kp * Td * Ti * s^2)/(Ti * s)




---
#### Quellen
[https://aleksandarhaber.com](https://aleksandarhaber.com/model-assisted-ziegler-nichols-pid-control-tuning-method/)
[https://www.youtube.com](https://www.youtube.com/watch?v=YYxkS1iFdVk)