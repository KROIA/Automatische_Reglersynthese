addpath('..');
addpath('../Utilities');
addpath('../Utilities/NyquistLog');
combinedNyquistPlotFileName = 'CombinedMethodes_SystemLogScaleNyquist.pdf';
combinedBodePlotFileName = 'CombinedMethodes_SystemBode.pdf';
combinedPIDBodePlotFileName = 'CombinedMethodes_PIDBode.pdf';
combinedSensitivityPlotFileName = 'CombinedMethodes_Sensitivity.pdf';

systuneStabilityMarginsPlotFileName = 'SystuneStabilityPhaseMarginsPlot.pdf';
geneticStabilityMarginsPlotFileName = 'GeneticStabilityPhaseMarginsPlot.pdf';
differentialStabilityMarginsPlotFileName = 'DifferentialStabilityPhaseMarginsPlot.pdf';

systuneStabilityDelayMarginsPlotFileName = 'SystuneStabilityDelayMarginsPlot.pdf';
geneticStabilityDelayMarginsPlotFileName = 'GeneticStabilityDelayMarginsPlot.pdf';
differentialStabilityDelayMarginsPlotFileName = 'DifferentialStabilityDelayMarginsPlot.pdf';

PlantsInit;
GeneticAndDifferentialParams;
if ~exist('systunePIDc', 'var')
    MotorMitSchwungmasseTuner; % tune PID using systune
end

%% Step 1: Define the Plant Model
motorSys = motorMitSchwungmasseStateSpace;
motorSys = tf(motorSys(2,1));

%% Systune "systunePID" gets defined in the MotorMitSchwungmasseTuner.m
systuneSystem = systunePIDc * motorSys; % 
systuneSystem.Name = 'Systune';

%% Genetic
geneticSystem = geneticPIDc * motorSys;
geneticSystem.Name = 'Genetic';

%% Differential Evolution
differentialSystem = differentialPIDc * motorSys;
differentialSystem.Name = 'Differential';


plotSystems(systuneSystem, geneticSystem, differentialSystem);
pzPlots(systunePIDc, geneticPIDc, differentialPIDc, motorSys);
stepInfos(systunePIDc, geneticPIDc, differentialPIDc, motorSys);


colors = {'#3189f5', '#2cf533', '#cc2cf5'};
nyquistPdfOutPath = combinedNyquistPlotFileName;
bodePdfOutPath = combinedBodePlotFileName;
bodePIDPdfOutPath = combinedPIDBodePlotFileName;
sensitivityPdfOutPath = combinedSensitivityPlotFileName;

systuneStabilityMarginsPlotOutPath = systuneStabilityMarginsPlotFileName;
geneticStabilityMarginsPlotOutPath = geneticStabilityMarginsPlotFileName;
differentialStabilityMarginsPlotOutPath = differentialStabilityMarginsPlotFileName;
systuneStabilityDelayMarginsPlotOutPath = systuneStabilityDelayMarginsPlotFileName;
geneticStabilityDelayMarginsPlotOutPath = geneticStabilityDelayMarginsPlotFileName;
differentialStabilityDelayMarginsPlotOutPath = differentialStabilityDelayMarginsPlotFileName;
if exist('motorMitSchwungmasseImagesOutputPath', 'var') == 1 && ~isempty(dokuImagesOutputPath) 
    nyquistPdfOutPath = [motorMitSchwungmasseImagesOutputPath '/' nyquistPdfOutPath];
    bodePdfOutPath = [motorMitSchwungmasseImagesOutputPath '/' bodePdfOutPath];
    bodePIDPdfOutPath = [motorMitSchwungmasseImagesOutputPath '/' bodePIDPdfOutPath];
    sensitivityPdfOutPath = [motorMitSchwungmasseImagesOutputPath '/' sensitivityPdfOutPath];


    systuneStabilityMarginsPlotOutPath = [motorMitSchwungmasseImagesOutputPath '/' systuneStabilityMarginsPlotOutPath];
    geneticStabilityMarginsPlotOutPath = [motorMitSchwungmasseImagesOutputPath '/' geneticStabilityMarginsPlotOutPath];
    differentialStabilityMarginsPlotOutPath = [motorMitSchwungmasseImagesOutputPath '/' differentialStabilityMarginsPlotOutPath];

    systuneStabilityDelayMarginsPlotOutPath = [motorMitSchwungmasseImagesOutputPath '/' systuneStabilityDelayMarginsPlotOutPath];
    geneticStabilityDelayMarginsPlotOutPath = [motorMitSchwungmasseImagesOutputPath '/' geneticStabilityDelayMarginsPlotOutPath];
    differentialStabilityDelayMarginsPlotOutPath = [motorMitSchwungmasseImagesOutputPath '/' differentialStabilityDelayMarginsPlotOutPath];
end

%% Log scale Nyquist Plot
fig = NyquistLogArray({systuneSystem, geneticSystem, differentialSystem}, colors, false);
figureToPDF(fig, nyquistPdfOutPath);

%fig = NyquistLogArray({tf(systunePID), tf(geneticPID), tf(differentialPID)}, colors, false);

%% Bode Diagram
% Compute common frequency range for all systems
systems = {systuneSystem, geneticSystem, differentialSystem};

% Find the frequency range that covers all systems
wMin = inf;
wMax = -inf;

for i = 1:length(systems)
    [~, ~, w] = bode(systems{i});
    wMin = min(wMin, min(w));
    wMax = max(wMax, max(w));
end

% Create common frequency vector (logarithmically spaced)
wCommon = logspace(log10(wMin), log10(wMax), 500);

% Create overlaid Bode plots with common frequency vector
fig = figure('Position', [0, 0, 600, 700]);
bodeColored(systuneSystem, colors{1}, wCommon);
bodeColored(geneticSystem, colors{2}, wCommon);
bodeColored(differentialSystem, colors{3}, wCommon);
legend({'Systune','Genetic','Differential'}, 'Location','best');
figureToPDF(fig, bodePdfOutPath);

fig = figure('Position', [0, 0, 600, 700]);
bodeColored(systunePIDc, colors{1}, wCommon);
bodeColored(geneticPIDc, colors{2}, wCommon);
bodeColored(differentialPIDc, colors{3}, wCommon);
legend({'SystunePID','GeneticPID','DifferentialPID'}, 'Location','best');
figureToPDF(fig, bodePIDPdfOutPath);


%% Log scale stability margins plot
if ~exist('stabilityRegionPlotResolution', 'var')
    stabilityRegionPlotResolution = 300;
end

fig = stabilityRegionPlot(systuneSystem, 'Resolution', stabilityRegionPlotResolution);
figureToPDF(fig, systuneStabilityMarginsPlotOutPath);
fig = stabilityRegionPlot(geneticSystem, 'Resolution', stabilityRegionPlotResolution);
figureToPDF(fig, geneticStabilityMarginsPlotOutPath);
fig = stabilityRegionPlot(differentialSystem, 'Resolution', stabilityRegionPlotResolution);
figureToPDF(fig, differentialStabilityMarginsPlotOutPath);

fig = sensitivityPlot(systems, 'Colors', colors, 'FreqRange', [10^(-1), 10^2]);
figureToPDF(fig, sensitivityPdfOutPath, 'WidthRatio', 3);

if ~exist('stabilityRegionDelayPlotResolution', 'var')
    stabilityRegionDelayPlotResolution = 300;
end
if ~exist('stabilityRegionDelayPlotDelayRange', 'var')
    stabilityRegionDelayPlotDelayRange = [-3 3];
end
if ~exist('stabilityRegionDelayPlotUseLogScale', 'var')
    stabilityRegionDelayPlotUseLogScale = true;
end
fig = stabilityDelayRegionPlot(systuneSystem, ...
    'DelayRange', stabilityRegionDelayPlotDelayRange, ...
    'UseLogscaleDelay', stabilityRegionDelayPlotUseLogScale, ...
    'Resolution', stabilityRegionDelayPlotResolution);
figureToPDF(fig, systuneStabilityDelayMarginsPlotOutPath);

fig = stabilityDelayRegionPlot(geneticSystem, ...
    'DelayRange', stabilityRegionDelayPlotDelayRange, ...
    'UseLogscaleDelay', stabilityRegionDelayPlotUseLogScale, ...
    'Resolution', stabilityRegionDelayPlotResolution);
figureToPDF(fig, geneticStabilityDelayMarginsPlotOutPath);

fig = stabilityDelayRegionPlot(differentialSystem, ...
    'DelayRange', stabilityRegionDelayPlotDelayRange, ...
    'UseLogscaleDelay', stabilityRegionDelayPlotUseLogScale, ...
    'Resolution', stabilityRegionDelayPlotResolution);
figureToPDF(fig, differentialStabilityDelayMarginsPlotOutPath);
% -----------------------------------------------------------------------
%   Functions
% -----------------------------------------------------------------------
function plotSystems(systuneSystem, geneticSystem, differentialSystem)
    close all;
    figure;
    %% --- TOP: Bode ---
    subplot(2,1,1);
    hold on;    

    bode(systuneSystem);
    hold on;
    bode(geneticSystem);
    bode(differentialSystem);
    grid on;
    legend({'Systune','Genetic','Differential'}, 'Location','best');
    
    %% --- BOTTOM: Nyquist plot ---
    subplot(2,1,2);
    hold on;
    
    nyquist(systuneSystem, 'b'); hold on;
    nyquist(geneticSystem, 'r');
    nyquist(differentialSystem, 'g');

    % Draw unit circle
    theta = linspace(0, 2*pi, 500);
    plot(cos(theta), sin(theta), 'r--', 'LineWidth', 1.5);

    
    title('Nyquist Diagram');
    grid on;
    legend({'Systune','Genetic','Differential'}, 'Location','best');

   
end

function pzPlots(systunePID, geneticPID, differentialPID, motorSys)
    %% --- Pole Zero plot ---
    figure;
    subplot(3,2,1);
    hold on;
    pzplot(systunePID);
    legend({'SystunePID'}, 'Location','best');

    subplot(3,2,2);
    pzplot(systunePID*motorSys);
    legend({'SystunePID*motor'}, 'Location','best');



    subplot(3,2,3);
    hold on;
    pzplot(geneticPID);
    legend({'geneticPID'}, 'Location','best');

    subplot(3,2,4);
    pzplot(geneticPID*motorSys);
    legend({'geneticPID*motor'}, 'Location','best');



    subplot(3,2,5);
    hold on;
    pzplot(differentialPID);
    legend({'differentialPID'}, 'Location','best');

    subplot(3,2,6);
    pzplot(differentialPID*motorSys);
    legend({'differentialPID*motor'}, 'Location','best');


end

function systemInfo(pid, motorSys, name)
    info = stepinfo(feedback(pid*motorSys, 1));
    fprintf(['\n=== ' name ' Performance ===\n']);
    fprintf('Rise Time:        %.3f s\n', info.RiseTime);
    fprintf('Settling Time:    %.3f s\n', info.SettlingTime);
    fprintf('Overshoot:        %.2f %%\n', info.Overshoot);

    [Gm, Pm] = margin(pid*motorSys);
    fprintf('\nGain Margin:    %.2f dB\n', 20*log10(Gm));
    fprintf('Phase Margin:     %.2f °\n', Pm);
end
function stepInfos(systunePID, geneticPID, differentialPID, motorSys)
    %% systune
    systemInfo(systunePID, motorSys, 'systunePID');
    systemInfo(geneticPID, motorSys, 'geneticPID');
    systemInfo(differentialPID, motorSys, 'differentialPID');

end

function bodeColored(sys, color, wCommon)
    % bodeColored Creates a Bode plot with a specified color
    %
    % Inputs:
    %   sys     - LTI system (transfer function, state-space, etc.)
    %   color   - Color specification (RGB array, char, or hex string)
    %   wCommon - (Optional) Common frequency vector for all plots
    
    % If no common frequency vector provided, generate one based on system
    if nargin < 3 || isempty(wCommon)
        [mag, phase, wout] = bode(sys);
    else
        [mag, phase] = bode(sys, wCommon);
        wout = wCommon;
    end
    
    % Squeeze to remove singleton dimensions
    mag = squeeze(mag);
    phase = squeeze(phase);
    
    % Convert magnitude to dB
    magdB = 20*log10(mag);
    
    % Create or get current axes
    ax = gca;
    
    % Check if this is a subplot layout or if we need to create subplots
    if strcmp(get(ax, 'Tag'), 'bode_magnitude') || strcmp(get(ax, 'Tag'), 'bode_phase')
        % Already in a Bode plot layout, continue using it
    else
        % Create subplot layout for first call
        clf;
        subplot(2,1,1);
        set(gca, 'Tag', 'bode_magnitude');
        subplot(2,1,2);
        set(gca, 'Tag', 'bode_phase');
    end
    
    % Plot magnitude
    subplot(2,1,1);
    semilogx(wout, magdB, 'Color', color, 'LineWidth', 1.5);
    hold on;
    ylabel('Amplitude (dB)');
    title('Bode Diagram');
    grid on;
    set(gca, 'Tag', 'bode_magnitude');
    
    % Plot phase
    subplot(2,1,2);
    semilogx(wout, phase, 'Color', color, 'LineWidth', 1.5);
    hold on;
    xlabel('Frequenz (rad/s)');
    ylabel('Phase (deg)');
    grid on;
    set(gca, 'Tag', 'bode_phase');
end