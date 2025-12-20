% ======================================================
% Skript: Mehrfache Ausführung eines Simulink-Modells
%         und Speicherung der Ausgangsdaten
%
% Beschreibung:
% Dieses Skript führt das Simulink-Modell
% "SystemIdentification.slx" mehrfach aus, wobei der
% Step-Gain schrittweise von startStepGain bis endStepGain
% variiert wird. Für jede Simulation werden die Signale
% u1, u2, y1 und y2 zusammen mit der Zeit extrahiert,
% Offset-Korrekturen für die Encoder-Signale vorgenommen
% und die Ergebnisse in einer CSV-Datei gespeichert.
%
% Vorgehensweise:
% 1. Simulink-Modell laden und vorbereiten
% 2. Schleife über definierte Step-Gain-Werte:
%    - Step-Gain in den Workspace schreiben
%    - Simulation ausführen
%    - Offset der Encoder-Signale korrigieren
%    - Daten in eine Zell-Tabelle zusammenfassen
%    - Tabelle als CSV-Datei speichern
% 3. Kurze Pause zwischen den Simulationen, um das System
%    abkühlen zu lassen
%
% Eingaben:
%  - Simulink-Modell "SystemIdentification.slx"
%  - Parameter: startStepGain, endStepGain, stepsize
%
% Ausgaben:
%  - CSV-Dateien mit Zeit und Signaldaten für jede Simulation
%    (z.B. simoutData1.csv, simoutData2.csv, ...)
%
% ======================================================

clear;
close all;


model = 'Systemidentifikation';   % Systemidentifikation.slx to execute
resultFolder = 'SystemIdentifikation';
stepsize = 500;
startStepGain = 500;
endStepGain = 3000;
loopDelay = 2; % 2 second delay to let the system cool down

labels = {'Zeit [s]', 'u1','u2', 'y1','y2'};
load_system(model);


mkdir(resultFolder);
counter = 0;
% Loop for 10 simulation runs
for stepFinalValue = startStepGain : stepsize : endStepGain
    counter = counter+1;
    % Assign step gain to workspace so Simulink can use it
    assignin('base', 'stepFinalValue', stepFinalValue);

    % Run simulink model
    sim(model);
    
    simout(:,4) = simout(:,4) - simout(1,4); % Remove offset of encoder 1
    simout(:,5) = simout(:,5) - simout(1,5); % Remove offset of encoder 2


    result = [
        labels;
        num2cell(simout)
    ];


    % --- Save data and combined plot ---
    filename = sprintf('%s/simoutData%d.csv',resultFolder, counter);
    %saveData(t, u.Data, z.Data, y1Data, y2Data, filename);
    writecell(result, filename, 'Delimiter', ';');
    pause(loopDelay);
end
