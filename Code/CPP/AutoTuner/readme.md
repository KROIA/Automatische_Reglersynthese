# AutoTuner Applikation
[Go to Main Page](../../../readme.md)

---
## Buildumgebung
### Benötigte Tools und Bibliotheken
* [QT](https://www.qt.io/development/download)
  * Version: **5.15.2**
  * Compiler: **MSVC 2019 64bit**
  
  Module:
  * Core
  * Gui
  * Widgets
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/de/vs/older-downloads/) Kann eventuell auch mit einer neueren Version von Visual Studio Community funktionieren.
  Benötigte Erweiterungen zum installieren:
  * Qt Visual Studio Tools

* [Cmake](https://cmake.org/download/) Eventuell muss Cmake noch installiert werden. Bin mir nicht sicher wie gut das in Visual Studio integriert ist.

### Projekt Konfigurieren
Öffne diese Cmake C++ Projekt in Visual Studio mit `Datei->Öffnen->CMake`, navigiere in diesen Ordner und bestätige.

Visual Studio sollte direkt beginnen das Projekt zu konfigurieren.
Das Projekt muss immer dann konfiguriert werden, wenn:
- Eine Source Datei erstellt/entfernt wurde
- QT GUI Designs verändert wurden
  
> [!CAUTION]  
> Beim hinzufügen von neuen Dateien fragt VS nach ob sie dem CMake Build System hinzugefügt werden sollen,
> dies muss immer abgelehnt werden, da meine CMake Build scripts automatisch nach den source Dateien sucht und
> sie ansonsten doppelt kompiliert werden.

Das Projekt kann folgender massen manuell konfiguriert werden: 
In Visual Studio: `Projekt->Cache Konfigurieren`.
Oder für eine cleane Konfiguration: `Projekt->Cache löschen und neu konfigurieren`.
Manchmal kann es sein, dass sich Visual Studio ins Bein schiesst und nicht mehr konfiguriert oder builden kann.
Dabei kann ein manuelles Löschen des Ordners `build` helfen.
Nach dem löschen dieses Ordners muss das Projekt neu konfiguruert werden.


### Projekt Erzeugen
Wenn das Projekt korrekt konfiguriert ist, in Visual Studio oben ca. in der Mitte
das `Startelement` mit dem Drop-Down-Menu ausgewählt werden.
Wähle: `SimpleMotorTuner` und klicke auf den grünen Startknopf.
Beim ersten mal kompilieren wird die Anwendung nicht gestartet, weil QT-Dlls fehlen.
Diese können automatisch zusammenkopiert werden mit: `Erstellen->"AutoTuner" installieren`



### Weitere Hilfen bei Probleme
[https://github.com/KROIA/QT_cmake_library_template](https://github.com/KROIA/QT_cmake_library_template) Das Projekt basiert auf dieser Vorlage und eventuell findet sich dieser Doku die gesuchte Lösung zu einem Problem.



