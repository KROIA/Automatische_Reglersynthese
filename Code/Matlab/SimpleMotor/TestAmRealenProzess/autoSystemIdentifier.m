% ======================================================
% Skript: Automatisierte Ausführung und Auswertung eines
%         Simulink-Modells zur Systemidentifikation
%
% Beschreibung:
% Dieses Skript führt das Simulink-Modell
% "Systemidentifikation.slx" mehrfach mit variierendem
% Step-Gain aus. Für jede Simulation werden die
% Ausgangsdaten des Signals 'y' in definierten
% Zeitintervallen (Störbereiche) ausgewertet.
%
% Die Messung wird mit unterschiedlichen Sprüngen durchgeführt damit der 
% vorhandene nichlineare DC-Gain gemessen wird.
%
% Für jeden Step-Gain werden:
%  - Mittelwerte des Ausgangssignals 'y' in drei
%    vordefinierten Zeitfenstern berechnet (Ermittlung des DC-Gains)
%  - Die Ergebnisse in Tabellen gesammelt
%  - Rohdaten der Simulation als CSV-Dateien gespeichert
%
% Zusätzlich werden:
%  - Ergebnis-Tabellen als CSV exportiert
%  - Grafische Auswertungen der Ergebnisse erstellt
%  - Lineare Regressionsmodelle für die Mittelwerte
%    berechnet und visualisiert
%
% Parameter:
%  - Step-Gain: von startStepGain bis endStepGain
%    mit definierter Schrittweite
%  - Drei feste Zeitintervalle zur Bewertung
%    unterschiedlicher Störsituationen
%
% Ausgabe:
%  - CSV-Dateien mit Simulationsdaten
%  - CSV-Dateien mit zusammengefassten Ergebnissen
%  - Grafische Darstellung und lineare Fits
%
% Voraussetzungen:
%  - Simulink-Modell "Systemidentifikation.slx"
%  - Signale: u, y, z1, z2 im Workspace verfügbar
%
% ======================================================

clear;
close all;


model = 'Systemidentifikation';   % Systemidentifikation.slx to execute
resultFolder = 'results';
stepsize = 1;
startStepGain = 0;
endStepGain = 10;
loopDelay = 2; % 2 second delay to let the system cool down

load_system(model);

% Prepare result storage
results1 = table([], [], [], 'VariableNames', ...
    {'StepGain', 'Disturbance', 'yAverage'});

results2 = table([], [], [], [], 'VariableNames', ...
    {'StepGain', 'yAverageD0', 'yAverageD1', 'yAverageD2'});

% Time intervals (areas)
areas = [2.5 3.6;   % Area 1
         5.0 6.0;   % Area 2
         8.0 9.0];  % Area 3

mkdir(resultFolder);
counter = 0;
% Loop for 10 simulation runs
for stepFinalValue = startStepGain : stepsize : endStepGain
    counter = counter+1;
    % Assign step gain to workspace so Simulink can use it
    assignin('base', 'stepFinalValue', stepFinalValue);

    % Run simulink model
    simOut = sim(model);

    % Retrieve simulated time series "y"
    t = y.Time;
    data = y.Data;

    % --- Process each disturbance area ---
    averageData = {};
    for d = 1:3
        tStart = areas(d,1);
        tEnd   = areas(d,2);

        % Select data within the interval
        idx = (t >= tStart) & (t <= tEnd);

        % Compute average
        data_avg = mean(data(idx));
        averageData = {averageData{:} data_avg};

        % Append result row to table
        newRow1 = {stepFinalValue, d-1, data_avg};
        results1 = [results1; newRow1];

       

        
    end
    newRow2 = {stepFinalValue, averageData{:}}
    results2 = [results2; newRow2];
    disp("Adding row: " + newRow2);

    % --- Save data and combined plot ---
    filename = sprintf('%s/simoutData%d.csv',resultFolder, counter);
    saveData(t, u.Data, y.Data, z1.Data, z2.Data, filename);
    pause(loopDelay);
end

% Save table to csv

writetable(results1, sprintf('%s/simulationresults1.csv',resultFolder), 'Delimiter',';');
writetable(results2, sprintf('%s/simulationresults2.csv',resultFolder), 'Delimiter',';');
%disp('Simulation completed. results1 saved to simulation_results1.csv');

% Get Linear Regressions point
fig = figure;
[a1, b1] = polyfitAndDisplay(fig, results2(:,[1 2]));
[a2, b2] = polyfitAndDisplay(fig, results2(:,[1 3]));
[a3, b3] = polyfitAndDisplay(fig, results2(:,[1 4]));

figure(fig);
% Display results as plots
plotResults(results1, results2);  % Function to plot the tables


% ======================================================
% FUNCTION: save data + save one combined plot
% ======================================================
function saveData(time, u, y, z1, z2, filename)

    % Create table for all signals
    T = table(time, u, y, z1, z2);

    % Save as CSV
    writetable(T, filename, 'Delimiter',';');

    % ----- Create combined plot -----
    %figure('Visible','off');  % Create invisible figure for speed
    %plot(time, u,  'LineWidth',1.2); hold on;
    %plot(time, y,  'LineWidth',1.2);
    %plot(time, z1, 'LineWidth',1.2);
    %plot(time, z2, 'LineWidth',1.2);
    %grid on;
%
    %xlabel('Time (s)');
    %ylabel('Signal Value');
    %title(['Simulation Signals for stepFinalValue = ' filename]);
    %legend('u','y','z1','z2');
%
    %% Save plot as PNG
    %pngname = replace(filename, '.csv', '.png');
    %saveas(gcf, pngname);
%
    %close(gcf);
end



% ======================================================
% FUNCTION: Plot results1 and results2
% ======================================================
function plotResults(results1, results2)

    % Create a figure to display both tables
    figure;
    
    % Subplot 1: Plot for results1 (StepGain, Disturbance, yAverage)
    subplot(2, 1, 1);  % 2 rows, 1 column, first subplot
    hold on;
    scatter(results1.StepGain, results1.yAverage, 100, 'filled');
    xlabel('Step Gain');
    ylabel('y Average');
    title('Results1: y Average by Step Gain and Disturbance');
    grid on;
    
    % Plot each disturbance area (Disturbance: 0, 1, 2)
    for i = 0:2
        idx = results1.Disturbance == i;
        scatter(results1.StepGain(idx), results1.yAverage(idx), 100, 'DisplayName', ['Disturbance ' num2str(i)]);
    end
    legend show;

    % Subplot 2: Plot for results2 (StepGain vs. 3 Areas)
    subplot(2, 1, 2);  % 2 rows, 1 column, second subplot
    plot(results2.StepGain, results2.yAverageD0, '-o', 'DisplayName', 'yAverageD0', 'LineWidth', 1.5);
    hold on;
    plot(results2.StepGain, results2.yAverageD1, '-s', 'DisplayName', 'yAverageD1', 'LineWidth', 1.5);
    plot(results2.StepGain, results2.yAverageD2, '-^', 'DisplayName', 'yAverageD2', 'LineWidth', 1.5);
    xlabel('Step Gain');
    ylabel('Average Value');
    title('Results2: y Average for Each Disturbance Area');
    grid on;
    legend show;
    
end




function [a, b] = linearRegression(x, y)
    % This function calculates the linear regression parameters "a" and "b" for the line y = a*x + b
    % Input:
    %   x - Independent variable (vector of x points)
    %   y - Dependent variable (vector of y points)
    % Output:
    %   a - Slope of the line
    %   b - Intercept of the line

    % Check if inputs are the same length
    if length(x) ~= length(y)
        error('Input vectors x and y must have the same length.');
    end

    % Perform linear regression using polyfit (degree 1 polynomial = line)
    p = polyfit(x, y, 1);  % p(1) = a (slope), p(2) = b (intercept)
    
    a = p(1);  % Slope
    b = p(2);  % Intercept
end


function [a, b] = polyfitAndDisplay(figHandle, selectedTable)
    [a, b] = linearRegression(selectedTable{:,1}, selectedTable{:,2});
    % Display the results
    disp(['Slope (a): ', num2str(a)]);
    disp(['Intercept (b): ', num2str(b)]);
    plotLinearFitToFigure(figHandle, selectedTable{:,1}, selectedTable{:,2}, a, b);
end


function plotLinearFitToFigure(figHandle, x, y, a, b)
    % plotLinearFitToFigure: Adds a linear fit plot to an existing figure.
    %
    % Inputs:
    %   figHandle - Handle to the figure where the plot will be added
    %   x - Independent variable (vector of x values)
    %   y - Dependent variable (vector of y values)
    %   a - Slope of the line
    %   b - Intercept of the line
    %
    % Output:
    %   The function adds a scatter plot and fitted line to the specified figure.

    % Make the provided figure the current one
    figure(figHandle);

    % Ensure hold on persists between multiple calls
    hold on;

    % Plot the original data points
    scatter(x, y, 100, 'filled', 'DisplayName', 'Data Points');
    
    % Generate a range of x values for plotting the fitted line
    xFit = linspace(min(x), max(x), 100);  % 100 points between min(x) and max(x)

    % Calculate the corresponding y values using the linear equation y = a*x + b
    yFit = a * xFit + b;

    % Plot the fitted line
    plot(xFit, yFit, '-r', 'LineWidth', 2, 'DisplayName', 'Fitted Line');

    % Add labels and title only once (after the first plot)
    if isempty(get(gca, 'Children'))  % Check if it's the first plot in the figure
        xlabel('x');
        ylabel('y');
        title('Linear Fit to Data');
        grid on;
    end

    % Display the polynomial equation on the plot
    equationStr = sprintf('y = %.2fx + %.2f', a, b);
    text(min(x) + 0.5, max(y) - 1, equationStr, 'FontSize', 12, 'Color', 'blue');

    % Add a legend
    legend show;

    % No need for hold off, as it’s handled by the next function call
end