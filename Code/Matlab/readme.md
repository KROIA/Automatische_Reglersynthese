# MATLAB
[Go to Main Page](../../README.md)


## Inhalt
* [Info](#info)
* [Modelle](#modelle)
* [Scripte für die Optimierung der Systeme mit systune](#scripte-für-die-optimierung-der-systeme-mit-systune)
  * [DC-Motor](#dc-motor)
    * [Am Modell](#am-modell)
    * [Am Prozess](#am-prozess)
      * [Systemidentifikation](#systemidentifikation)
      * [PID Regler Tests](#pid-regler-tests)
  * [Motor mit Schwungmasse](#motor-mit-schwungmasse)
    * [Am Modell](#am-modell-1)
    * [Am Prozess](#am-prozess-1)
      * [Systemidentifikation](#systemidentifikation-1)
      * [PID Regler Tests](#pid-regler-tests-1)
* [Diverse andere Scripte](#diverse-andere-scripte)
* [Excel Format mit Darstellungsinformation](#excel-format-mit-darstellungsinformation)

---
#### Info
* Für diese Projekt wurde MATLAB 2023b verwendet
* Für die Ansteuerung der realen Prozesse wird die SoftHIL Hardware vorausgesetzt.
#### Modelle
Der MATLAB Ordner beinhaltet alle relevanten Scripte und Modelle.
Die Simulink-Modelle der verwendeten Prozesse, ist in der Bibliothek `Plants.slx` 
Für die Verwendung dieser Modelle wird das initialisieren mit der `PlantsInit.m` vorausgesetzt.
Die initialisierung sollte in allen relevanten Modellen und Scripte bereits automatisiert hinterlegt sein.

---
## Scripte für die Optimierung der Systeme mit systune
### DC-Motor
Die MATLAB-Skripte welche zu diesem System gehören sind im Ordner `Matlab/SimpleMotor` zu finden.
#### Am Modell
* [SimpleMotorTuner](SimpleMotor\SimpleMotorTuner.m) beinhaltet den code der systune Optimierung wie in der Arbeit beschrieben.
* [GeneticAndDifferentialParamsSimpleMotor](SimpleMotor\GeneticAndDifferentialParamsSimpleMotor.m) beinhaltet die Definition der Reglerparameter, welche vom genetischen Algorithmus (GA) und Differential Evolution (DE) ermittelt wurden.
* [SimpleMotorSystem](SimpleMotor\SimpleMotorSystem.slx) ist das Simulink-Modell zum testen der drei Regler am simulierten Prozess.
* [SimpleMotorSystemLogger](SimpleMotor\SimpleMotorSystemLogger.m) Kann verwendet werden um die vom `SimpleMotorSystem` erzeugten Messwerte als .csv im [Format](#excel-format-mit-darstellungsinformation) zu speichern.
* [SimpleMotorPIDResultAnalyzer](SimpleMotor\SimpleMotorPIDResultAnalyzer.m) Für die Erstellung diverser Diagramme für die Doku. (Wird von `DokuPlotsGenerator` aufgerufen)

#### Am Prozess
##### Systemidentifikation
* [autoSystemIdentifier](SimpleMotor\TestAmRealenProzess\autoSystemIdentifier.m) Script für die automatisierte Systemidentifikation.
* [Systemidentifikation](SimpleMotor\TestAmRealenProzess\Systemidentifikation.slx) Simulink-Modell welches von `autoSystemIdentifier.m` automatisch ausgeführt wird.

##### PID Regler Tests
* [PID_Test](SimpleMotor\TestAmRealenProzess\PID_Test.slx) Simulink-Modell mit SoftHIL Schnittstelle für die Tests der PID Regler am realen Prozess.
* [PID_Test_Logger](SimpleMotor\TestAmRealenProzess\PID_Test_Logger.m) Skript zum abspeichern der Messresultate durch das Simulink-Modell `PID_Test.slx`.



--- 
### Motor mit Schwungmasse
Die MATLAB-Skripte welche zu diesem System gehören sind im Ordner `Matlab/MotorMitSchwungmasse` zu finden.
#### Am Modell
* [MotorMitSchwungmasseTuner](MotorMitSchwungmasse\MotorMitSchwungmasseTuner.m) beinhaltet den code der systune Optimierung wie in der Arbeit beschrieben.
* [GeneticAndDifferentialParams](MotorMitSchwungmasse\GeneticAndDifferentialParams.m) beinhaltet die Definition der Reglerparameter, welche vom genetischen Algorithmus (GA) und Differential Evolution (DE) ermittelt wurden.
* [MotorMitSchwungmasse](MotorMitSchwungmasse\MotorMitSchwungmasse.slx) ist das Simulink-Modell zum testen der drei Regler am simulierten Prozess.
* [MotorMitSchwungmasseLogger](MotorMitSchwungmasse\MotorMitSchwungmasseLogger.m) Kann verwendet werden um die vom `MotorMitSchwungmasse` erzeugten Messwerte als .csv im [Format](#excel-format-mit-darstellungsinformation) zu speichern. 
* [MotorMitSchwungmassePIDResultAnalyzer](MotorMitSchwungmasse\MotorMitSchwungmassePIDResultAnalyzer.m) Für die Erstellung diverser Diagramme für die Doku. (Wird von `DokuPlotsGenerator` aufgerufen)

#### Am Prozess
##### Systemidentifikation
* [autoSystemIdentifier](MotorMitSchwungmasse\TestAmRealenProzess\autoSystemIdentifier.m) Script für die automatisierte Systemidentifikation.
* [Systemidentifikation](MotorMitSchwungmasse\TestAmRealenProzess\Systemidentifikation.slx) Simulink-Modell welches von `autoSystemIdentifier.m` automatisch ausgeführt wird.
* [ModelCreationStateSpaceConverter](MotorMitSchwungmasse\TestAmRealenProzess\ModelCreationStateSpaceConverter.m) exportiert das Modell des Motors mit Schwungmasse als Statespace in ein neues matlab Skript. Die Matrizen werden dann in das `PlantsInit.m` kopiert.
* [MotorMitSchwungmasseONLY](MotorMitSchwungmasse\TestAmRealenProzess\Systemidentifikation.slx) Modell welches vom `ModelCreationStateSpaceConverter.m` verwendet wird für die State-Space-Umwandlung.
##### PID Regler Tests
* [PID_Test](MotorMitSchwungmasse\TestAmRealenProzess\PID_Test.slx) Simulink-Modell mit SoftHIL Schnittstelle für die Tests der PID Regler am realen Prozess.
* [PID_Test_Logger](MotorMitSchwungmasse\TestAmRealenProzess\PID_Test_Logger.m) Skript zum abspeichern der Messresultate durch das Simulink-Modell `PID_Test.slx`.


---
## Diverse andere Scripte
* [DokuPlotsGenerator](DokuPlotsGenerator.m) zum automatisierten erstellen der in der Arbeit verwendeten Grafiken
#### Utilities Funktionen
* [NyquistLog](Utilities\NyquistLog\NyquistLog.m) für die Erstellung eines Logarithmisch skalierten Nyquist Diagrammes. [Credits und originaler code](https://github.com/lucaballotta/NyquistLog) 
* [NyquistLogArray](Utilities\NyquistLog\NyquistLogArray.m) angepasste Funktion des Originals, mit der Erweiterung für das nutzen mehrerer Systeme im gleichen Diagramm und diverse kleinere optische Anpassungen
* [exportAllCsvToPDF](Utilities\exportAllCsvToPDF.m) Exportiert alle gefundenen .csv Dateien in einem angegebenen Ordner als PDF-Diagramme. (Benötigt das [Format](#excel-format-mit-darstellungsinformation))
* [figureToPDF](Utilities\figureToPDF.m) Exportiert ein 'figure' Objekt als PDF
* [findFilesByName](Utilities\findFilesByName.m) Sucht rekursiv nach einer bestimmten Datei und gibt die Liste der gefundenen Dateien zurück
* [getMergedCSV](Utilities\getMergedCSV.m) Sucht in einem angegebenen Ordner nach .csv Dateien mit einem spezifischen Namen und kombiniert diese zu einer grossen Tabelle und gibt diese zurück
Dabei wird nur von der ersten gefundenen Tabelle die Spalte 1 verwendet und von den nachfolgenden Tabellen nur die Spalten ab 2+.
* [plotCsvToPDF](Utilities\plotCsvToPDF.m) Erzeugt ein Diagramm mit der angegebenen Tabelle ([Format](#excel-format-mit-darstellungsinformation)) und erzeugt ein PDF daraus
* [plotVisualisation3DGeneticStartConditions](Utilities\plotVisualisation3DGeneticStartConditions.m) Erzeugt ein 3D-Darstellung der Startbedingungen des in der Arbeit beschriebenen genetischen Algorithmus
* [sensitivityPlot](Utilities\sensitivityPlot.m) Erzeugt einen kombiniertes Sensitivitäts Diagramm für mehrere Systeme
* [stabilityDelayRegionPlot](Utilities\stabilityDelayRegionPlot.m) Erzeugt das Totzeit- & Verstärkungsreserve Diagramm
* [stabilityRegionPlot](Utilities\stabilityRegionPlot.m) Erzeugt das Phasen- & Verstärkungsreserve Diagramm
* [tableToFigure](Utilities\tableToFigure.m) Erzeugt eine MATLAB figure aus einer Tabelle mit der [vordefinierten Struktur](#excel-format-mit-darstellungsinformation) und gibt die figure zurück



## Excel Format mit Darstellungsinformation
Viele der verwendeten Excel Dateien beinhalten zusätzliche Informationen die von der Funktion **tableToFigure** 
verwendet werden um das resultierende Diagramm wie gewünscht darzustellen.
Das Excel (.csv), Trennzeichen ';' hat mehrere Spalten.
Die erste Spalte ist immer die Zeitachse.
Die folgenden Spalten können beliebige Messdaten darstellen.


| Zeile | Beschreibung |
|------:|--------------|
| **1** | Definiert die **Spaltennamen**.<br>Die **erste Zelle** definiert den Namen der **X-Achse**. |
| **2** | Definiert die **Linien-Styles** der Spalten.<br>Entspricht dem Text für den MATLAB-Befehl `plot('LineStyle', XX)` (z. B. `-`, `--`, `:`).<br>Die **erste Zelle** wird als Name der **Y-Achse** verwendet. |
| **3** | Definiert die **Linienstärke** der im Diagramm dargestellten Linien für die jeweilige Spalte.<br>Die **erste Spalte wird ignoriert**. |
| **4** | Definiert die **Farben** der im Diagramm dargestellten Linien für die jeweilige Spalte.<br>Farben müssen als **HEX-String** angegeben werden (z. B. `00bbae`, `1bfe3f`).<br>Beispiel im CSV-Text:<br>`0;00bbae;00bbae;00c9a0;` (ohne Anführungszeichen in der Datei). |
| **5+** | Enthält die **Datenpunkte**. |

#### Beispiel header der .csv Datei

```
Zeit [s];i;j;k;l;m;n;o;p;w;ds;e;g
Winkel [rad];-;--;-;--;-;--;-;--;-;--;-;--
0;1;1;1;1;1;1;1;1;1;1;1;1
0;00bbae;00bbae;00c9a0;00cab8;00d791;00d791;00e47d;00e47d;00f165;00f165;1bfe3f;1bfe3f
0;0;0;0;-5.68E-14;0;0;0;5.68E-14;0;-2.84E-14;0;2.84E-14
0.01;0;0;0;-5.68E-14;0;0;0;5.68E-14;0;-2.84E-14;0;2.84E-14
0.02;0;0;0;-5.68E-14;0;0;0;5.68E-14;0;-2.84E-14;0;2.84E-14
0.03;0;0;0;-5.68E-14;0;0;0;5.68E-14;0;-2.84E-14;0;2.84E-14
0.04;0;0;0;-5.68E-14;0;0;0;5.68E-14;0;-2.84E-14;0;2.84E-14
0.05;0;0;0;-5.68E-14;0;0;0;5.68E-14;0;-2.84E-14;0;2.84E-14
...
```

---


