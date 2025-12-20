% ======================================================
% Skript: Speicherung und Formatierung von Simulink-Daten
%
% Beschreibung:
% Dieses Skript verarbeitet Simulink-Ausgaben (out.simout),
% konvertiert numerische Daten in Zellen, versieht sie mit
% Metadaten wie Beschriftungen, Linienstilen, Linienstärken
% und Farben und speichert die Ergebnisse als CSV-Dateien.
%
% Das Skript kann nach der Ausführung des Modells "MotorMitSchwungmasse.slx" dazu verwendet werden,
% um simulierte Messresultate als csv zu speichern welches dann mit der matlab Funktion "tableToFigure" 
% zu einem Diagramm umgewandelt werden kann.
%
% Vorgehensweise:
% 1. Simulink-Ausgabe aus der Variable 'out.simout' extrahieren
% 2. Numerische Daten in Zell-Arrays umwandeln
% 3. Metadaten (Labels, Linienstile, Linienstärken, Farben) hinzufügen
% 4. Ausgewählte Spalten werden als separate CSV-Dateien exportiert
% 5. Optional: Tabellen können als PDF gespeichert werden (Funktion saveTableToPDF)
%
% Eingaben:
%  - out.simout : Simulink-Simulationsdaten
%
% Ausgaben:
%  - CSV-Dateien mit Metadaten und Simulationsdaten
%    (z.B. Systune_simulinkResult.csv, Genetic_simulinkResult.csv,
%     Differential_simulinkResult.csv)
%
% Zusatzfunktionen:
%  - saveTableToCSV    : Speichert ein Zell-Array als CSV-Datei
%  - saveTableToPDF    : Speichert ein Zell-Array als PDF-Datei
%
% ======================================================




addpath('../');
addpath('../Utilities');
close all;

% Convert numeric data to cell
data = out.simout;
dataCell = num2cell(data);

% Make sure the metadata rows are cell arrays
%                     1        2        3     4       5    6    7    8    9    10
labels        = {'Zeit [s]','r [rad]','u2','e [rad]','u1','y2','u1','y2','u1','y2'};
lineStyles    = {'', '-', '-', '-', '-', '-', '-', '-', '-', '-'};
lineThickness = {0,4,2,1,1,1,1,1,1,1};
%colors        = {0,'f2c94e','eb3a23','f2c94e','69ed58','287d1d','63e0d8','228a83','506ce6','15297d'};
colors        = {0,'ebbd34','e0162e','f2c94e','8888FF','0000AA','8888FF','0000AA','8888FF','0000AA'};

% Append as new rows
result = [
    labels;
    lineStyles;
    lineThickness;
    colors;
    dataCell
];


% Spalten index auswählen und ausgabename angeben
%figure(saveTableToPDF(result(:,[1 2 3]), 'Genetic_SimulationStimuli', 2));
%figure(saveTableToPDF(result(:,[1 2 3 5 6]), 'Systune_simulinkResult', 2));
%figure(saveTableToPDF(result(:,[1 2 3 5 6]), 'Systune_simulinkResult', 2));


saveTableToCSV(result(:,[1 2 3 5 6]) , '../DokuPlotTables/MotorMitSchwungmasse/Systune_simulinkResult.csv');
saveTableToCSV(result(:,[1 2 3 7 8]) , '../DokuPlotTables/MotorMitSchwungmasse/Genetic_simulinkResult.csv');
saveTableToCSV(result(:,[1 2 3 9 10]) , '../DokuPlotTables/MotorMitSchwungmasse/Differential_simulinkResult.csv');

% -------------------------------------------------------------------------------------------------
function fig = saveTableToPDF(cellArray, pdfFileName, widthRatio)
    exportPath = '../../../Dokumentation/LaTeX/images/MotorMitSchwungmasse';
    %exportPath = '../../../CPP/AutoTuner/build/Results/StepResponses/simulink';
   % tmpExcelPath = [exportPath '/tmp.csv'];
    tmpExcelPath = 'tmp.csv';
    writecell(cellArray, tmpExcelPath, 'Delimiter', ';');
    fig = plotCsvToPDF(tmpExcelPath, [exportPath '/' pdfFileName '.pdf'], widthRatio);
end

function saveTableToCSV(cellArray, pathToCsv)
    writecell(cellArray, pathToCsv, 'Delimiter', ';');
end