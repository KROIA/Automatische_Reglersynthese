% ======================================================
% Skript: Export eines State-Space-Modells in eine .m-Datei
%
% Beschreibung:
% Dieses Skript erstellt aus einem Simulink- oder MATLAB-
% State-Space-System (ss-Objekt oder Struktur mit A,B,C,D)
% eine MATLAB-Datei, die die Zustandsraum-Matrizen A, B, C, D
% als Variablen definiert. Die erzeugte Datei kann direkt in
% MATLAB geladen und für weitere Analysen oder Simulationen
% verwendet werden.
%
% Vorgehensweise:
% 1. Simulink-Modell linearisiert mittels linmod
%    (z.B. 'MotorMitSchwungmasseONLY')
% 2. Aufruf der Funktion exportStateSpace(sys, filename)
%    zur Erstellung einer MATLAB-Datei mit den Matrizen
% 3. Validierung der Eingaben: ss-Objekt oder Struct mit
%    Feldern A,B,C,D
% 4. Schreibvorgang in eine .m-Datei mit Header und MATLAB-
%    Syntax für jede Matrix
%
% Eingaben:
%  - sys      : ss-Objekt oder Struktur mit Feldern A,B,C,D
%  - filename : Name der zu erzeugenden MATLAB-Datei
%
% Ausgaben:
%  - MATLAB-Datei mit Definitionen von A, B, C, D
%  - Konsolenausgabe über erfolgreichen Export
%
% Beispiel:
%   ssSystem = linmod('MotorMitSchwungmasseONLY');
%   exportStateSpace(ssSystem, 'exportedStateSpaceModel.m');
%
% ======================================================



ssSystem = linmod('MotorMitSchwungmasseONLY');

exportStateSpace(ssSystem, 'exportedStateSpaceModel.m');


function exportStateSpace(sys, filename)
% exportStateSpace(sys, filename)
% --------------------------------
% Creates a MATLAB script containing the definition of the 
% state-space matrices A, B, C, D from the system 'sys'.
%
% INPUTS:
%   sys      - state-space system (ss object) or struct with A,B,C,D
%   filename - name of the .m script to generate (string)
%
% Example:
%   sys = ss(A,B,C,D);
%   exportStateSpace(sys, 'mySystem.m');
%

    % Validate inputs
    if nargin < 2
        error('Usage: exportStateSpace(sys, filename)');
    end

    % Allow raw matrices or ss object
    if isa(sys, 'ss')
        A = sys.a;
        B = sys.b;
        C = sys.c;
        D = sys.d;
    else
        % Expect fields A,B,C,D
        if ~all(isfield(sys, {'a','b','c','d'}))
            error('If input is not an ss object, it must have fields A,B,C,D.');
        end
        A = sys.a; B = sys.b; C = sys.c; D = sys.d;
    end

    % Open file for writing
    fid = fopen(filename, 'w');
    if fid == -1
        error('Could not open file "%s" for writing.', filename);
    end

    % Write header
    fprintf(fid, '%% Auto-generated state-space matrices\n');
    fprintf(fid, '%% Generated on %s\n\n', datestr(now));

    % Write matrices using MATLAB syntax
    writeMatrix(fid, 'A', A);
    writeMatrix(fid, 'B', B);
    writeMatrix(fid, 'C', C);
    writeMatrix(fid, 'D', D);

    fclose(fid);

    fprintf('File "%s" generated successfully.\n', filename);
end

function writeMatrix(fid, name, M)
    fprintf(fid, '%s = [\n', name);
    for i = 1:size(M,1)
        fprintf(fid, '  ');
        fprintf(fid, '%g ', M(i,:));
        fprintf(fid, ';\n');
    end
    fprintf(fid, '];\n\n');
end
