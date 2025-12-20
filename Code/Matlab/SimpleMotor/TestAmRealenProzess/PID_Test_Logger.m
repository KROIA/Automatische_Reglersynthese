% ======================================================
% Skript: Speichern von Simulink-Signalen in CSV-Datei
%         Kann nach der ausführung des Modells: "PID_Test.slx" zum 
%         abspeichern der realen Messwerte verwendet werden.
%
% Beschreibung:
% Dieses Skript extrahiert die Zeitreihen und Signaldaten
% eines Simulink-Modells aus den Workspace-Variablen:
% u, y, z1, z2 und r. Anschließend werden die Daten in
% einer CSV-Datei gespeichert, wobei jede Ausführung
% automatisch durchnummeriert wird.
%
% Vorgehensweise:
% 1. Extraktion der Zeit- und Signaldaten aus den Workspace-Variablen
% 2. Zusammenfassung aller Signale in einer Tabelle
% 3. Speichern der Tabelle als CSV-Datei mit
%    fortlaufender Nummerierung (simout_data_<counter>.csv)
%
% Parameter / Variablen:
%  - time   : Zeitvektor
%  - data_u : Eingangssignal u
%  - data_y : Ausgangssignal y
%  - data_z1: Wirbelstrombremse 1
%  - data_z2: Wirbelstrombremse 2
%  - data_r : Referenzsignal r
%  - counter: Zähler zur eindeutigen Dateibenennung
%
% Ausgabe:
%  - CSV-Datei mit allen Signalen, getrennt durch Semikolon
%
% ======================================================




% Extract time and data
time = u.Time;
data_u = u.Data;
data_y = y.Data;
data_z1 = z1.Data;
data_z2 = z2.Data;

data_r = r.Data;

if ~exist('counter','var')
    counter = 0;
end
counter = counter +1;

% Write to CSV
T = table(time,data_u,data_y, data_z1, data_z2, data_r);
filename = sprintf('simout_data_%d.csv', counter);
writetable(T, filename,'Delimiter', ';');