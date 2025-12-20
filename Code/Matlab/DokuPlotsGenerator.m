%%
% Dieses Script dient zur generierung der meisten Graphischen Darstellungen 
% in der Dokumentation.
% Das Script muss in dem Verzeichnis ausgeführt werden, in dem es liegt.
% Das Script greift auf den Ordner "DokuPlotTables" zu, welcher diverse Simulationsergebnisse
% und Messresultate beinhaltet, aus denen die Darstellungen erzeugt werden.
% Das Script kann in einem leeren Umfeld gestartet werden, also keine Variabeln müssen 
% definiert sein, die benötigten Objekte werden automatisch generiert.
%


if ~endsWith(pwd, ['\Code\Matlab'], 'IgnoreCase', true)
    error('Execute this script hin the main folder: ....\Code\Matlab');
end



dokuImagesOutputPath = '..\..\Dokumentation\LaTeX\images';
resourcePath = 'DokuPlotTables';


simpleMotorResourcesRoot = [resourcePath '\SimpleMotor'];
simpleMotorImagesOutputPath = [dokuImagesOutputPath '\SimpleMotor'];

motorMitSchwungmasseResourceRoot = [resourcePath '\MotorMitSchwungmasse'];
motorMitSchwungmasseImagesOutputPath = [dokuImagesOutputPath '\MotorMitSchwungmasse'];
stabilityRegionPlotResolution = 500;
stabilityRegionDelayPlotResolution = 500;
simpleMotorStabilityRegionDelayPlotDelayRange = [-4 1];
motorMitSchwungmasseStabilityRegionDelayPlotDelayRange = [-3 2];
stabilityRegionDelayPlotUseLogScale = true;


addpath('SimpleMotor');
addpath('MotorMitSchwungmasse');
addpath('Utilities');
addpath('Utilities/NyquistLog');
%MotorInit;
%GeneticAndDifferentialParams;

generateSimpleMotorPlots(simpleMotorResourcesRoot, ...
    simpleMotorImagesOutputPath, ...
    dokuImagesOutputPath, ...
    stabilityRegionPlotResolution, ...
    stabilityRegionDelayPlotResolution, ...
    simpleMotorStabilityRegionDelayPlotDelayRange, ...
    stabilityRegionDelayPlotUseLogScale);

generateMotorMitSchwungmassePlots(motorMitSchwungmasseResourceRoot, ...
    motorMitSchwungmasseImagesOutputPath, ...
    dokuImagesOutputPath, ...
    stabilityRegionPlotResolution, ...
    stabilityRegionDelayPlotResolution, ...
    motorMitSchwungmasseStabilityRegionDelayPlotDelayRange, ...
    stabilityRegionDelayPlotUseLogScale);


fig = plotVisualisation3DGeneticStartConditions();
figureToPDF(fig, [dokuImagesOutputPath '\Genetic2DProblemSpaceVisualisation_1.pdf'], 'ContentType', 'image')
fig = plotVisualisation3DGeneticStartConditions('ScatterRadius', 3);
figureToPDF(fig, [dokuImagesOutputPath '\Genetic2DProblemSpaceVisualisation_2.pdf'], 'ContentType', 'image')



%% ------------------------------------------------------------------
%
%





function generateSimpleMotorPlots(simpleMotorResourcesRoot, ...
    simpleMotorImagesOutputPath, ...
    dokuImagesOutputPath, ...
    stabilityRegionPlotResolution, ...
    stabilityRegionDelayPlotResolution, ...
    stabilityRegionDelayPlotDelayRange, ...
    stabilityRegionDelayPlotUseLogScale)


    stepResponseLegendXPos = 0.85;
    scoreHistoryMaxY = 10;
    scoreHistoryStartX = 30;
    scoreHistoryLogSetting = [1,1]; % X-Axis log scale and Y-Axis log scale
    tuningGoalPlotWidthRatio = 1.2;
    %% SimpleMotor
    % Stimuli signals
    plotCsvToPDF([simpleMotorResourcesRoot '/Stimuli.csv'],[simpleMotorImagesOutputPath '/Stimuli.pdf'],2,[stepResponseLegendXPos 0.5 0.1 0.1]);
    
    % Systune Results
    plotCsvToPDF([simpleMotorResourcesRoot '/Systune_realResult.csv'],[simpleMotorImagesOutputPath '/Systune_realResult.pdf'],2,[stepResponseLegendXPos 0.5 0.1 0.1]);
    plotCsvToPDF([simpleMotorResourcesRoot '/Systune_simulinkResult.csv'],[simpleMotorImagesOutputPath '/Systune_simulinkResult.pdf'],2,[stepResponseLegendXPos 0.5 0.1 0.1]);
    
    % Genetic results
    plotCsvToPDF([simpleMotorResourcesRoot '/Genetic_realResult.csv'],[simpleMotorImagesOutputPath '/Genetic_realResult.pdf'],2,[stepResponseLegendXPos 0.5 0.1 0.1]);
    
    % Learncurve
    geneticSimulationOutputRoot = [simpleMotorResourcesRoot '/geneticSimulationOutputRoot'];
    mergePlotAndSave(geneticSimulationOutputRoot, 'learning_history', simpleMotorImagesOutputPath, 'Genetic_Simulation_LearningHistory_1', scoreHistoryLogSetting, 0, [scoreHistoryStartX, 5000, 0, scoreHistoryMaxY])
    mergePlotAndSave(geneticSimulationOutputRoot, 'step_response', simpleMotorImagesOutputPath, 'Genetic_Simulation_StepResponse_1', 0, 0)
    
    % Differential results
    plotCsvToPDF([simpleMotorResourcesRoot '/Differential_realResult.csv'],[simpleMotorImagesOutputPath '/Differential_realResult.pdf'],2,[stepResponseLegendXPos 0.5 0.1 0.1]);
    
    % Learncurve
    differentialSimulationOutputRoot = [simpleMotorResourcesRoot '/differentialSimulationOutputRoot'];
    mergePlotAndSave(differentialSimulationOutputRoot, 'learning_history', simpleMotorImagesOutputPath, 'Differential_Simulation_LearningHistory_1', scoreHistoryLogSetting, 0, [scoreHistoryStartX, 201, 0, scoreHistoryMaxY]);
    mergePlotAndSave(differentialSimulationOutputRoot, 'step_response', simpleMotorImagesOutputPath, 'Differential_Simulation_StepResponse_1', 0, 0);
    
    % Combined
    plotCsvToPDF([simpleMotorResourcesRoot '/CombinedMethodes_realResult.csv'],[simpleMotorImagesOutputPath '/CombinedMethodes_realResult.pdf'],2,[0.82 0.5 0.1 0.1]);
    combinedLearningHistoryFolderName = 'combinedLearningHistory';
    prepareCombinedLearningHistory(simpleMotorResourcesRoot, combinedLearningHistoryFolderName, geneticSimulationOutputRoot, differentialSimulationOutputRoot);
    mergePlotAndSave([simpleMotorResourcesRoot '/combinedLearningHistory'], 'learning_history', simpleMotorImagesOutputPath, 'CombinedMethodes_Simulation_LearningHistory_1', scoreHistoryLogSetting, 0, [scoreHistoryStartX, 5000, 0, scoreHistoryMaxY])
    
    dcGainPlot = simpleMotorDCGainApproxPlot();
    figureToPDF(dcGainPlot, [simpleMotorImagesOutputPath '/DcGainPlot.pdf']);

    % Call PID Result Analyzer script to generate additional plots
    SimpleMotorPIDResultAnalyzer
end

function generateMotorMitSchwungmassePlots(motorMitSchwungmasseResourceRoot, ...
    motorMitSchwungmasseImagesOutputPath, ...
    dokuImagesOutputPath, ...
    stabilityRegionPlotResolution, ...
    stabilityRegionDelayPlotResolution, ...
    stabilityRegionDelayPlotDelayRange, ...
    stabilityRegionDelayPlotUseLogScale)

    stepResponseLegendXPos = 0.85;
    scoreHistoryMaxY = 100000000;
    scoreHistoryStartX = 30;
    scoreHistoryLogSetting = [1,1]; % X-Axis log scale and Y-Axis log scale
    tuningGoalPlotWidthRatio = 1.2;
    %% Motor mit Schwungmasse
    % Systemidentifikation
    plotCsvToPDF([motorMitSchwungmasseResourceRoot '/MotorMitSchwungmasseSystemIdentifikationsmessungInputs.csv'],[motorMitSchwungmasseImagesOutputPath '/MotorMitSchwungmasseSystemIdentifikationsmessungInputs.pdf'],2, [stepResponseLegendXPos 0.2 0.1 0.1]);
    plotCsvToPDF([motorMitSchwungmasseResourceRoot '/MotorMitSchwungmasseSystemIdentifikationsmessungOutputs.csv'],[motorMitSchwungmasseImagesOutputPath '/MotorMitSchwungmasseSystemIdentifikationsmessungOutputs.pdf'],2, [stepResponseLegendXPos 0.5 0.1 0.1]);
    plotCsvToPDF([motorMitSchwungmasseResourceRoot '/MotorMitSchwungmasseModelEvalOutputs.csv'],[motorMitSchwungmasseImagesOutputPath '/MotorMitSchwungmasseModelEvalOutputs.pdf'],2);
    plotCsvToPDF([motorMitSchwungmasseResourceRoot '/MotorMitSchwungmasseModelEvalOutputsAllY1.csv'],[motorMitSchwungmasseImagesOutputPath '/MotorMitSchwungmasseModelEvalOutputsAllY1.pdf'],2, 0);
    plotCsvToPDF([motorMitSchwungmasseResourceRoot '/MotorMitSchwungmasseModelEvalOutputsAllY2.csv'],[motorMitSchwungmasseImagesOutputPath '/MotorMitSchwungmasseModelEvalOutputsAllY2.pdf'],2, 0);
    
    
    
    % Stimuli signals
    plotCsvToPDF([motorMitSchwungmasseResourceRoot '/Stimuli.csv'],[motorMitSchwungmasseImagesOutputPath '/Stimuli.pdf'],2,[stepResponseLegendXPos 0.7 0.1 0.1]);
    %
    %% Systune Results
    plotCsvToPDF([motorMitSchwungmasseResourceRoot '/Systune_realResult.csv'],[motorMitSchwungmasseImagesOutputPath '/Systune_realResult.pdf'],2,[stepResponseLegendXPos 0.7 0.1 0.1]);
    %plotCsvToPDF([motorMitSchwungmasseResourceRoot '/Systune_simulinkResult.csv'],[motorMitSchwungmasseImagesOutputPath '/Systune_simulinkResult.pdf'],2,[stepResponseLegendXPos 0.5 0.1 0.1]);
    %
    %% Genetic results
    plotCsvToPDF([motorMitSchwungmasseResourceRoot '/Genetic_realResult.csv'],[motorMitSchwungmasseImagesOutputPath '/Genetic_realResult.pdf'],2,[stepResponseLegendXPos 0.7 0.1 0.1]);
    %
    %% Learncurve
    geneticSimulationOutputRoot = [motorMitSchwungmasseResourceRoot '/geneticSimulationOutputRoot'];
    mergePlotAndSave(geneticSimulationOutputRoot, 'learning_history', motorMitSchwungmasseImagesOutputPath, 'Genetic_Simulation_LearningHistory_1', scoreHistoryLogSetting, 0, [scoreHistoryStartX, 1000, 0, scoreHistoryMaxY])
    mergePlotAndSave(geneticSimulationOutputRoot, 'step_response', motorMitSchwungmasseImagesOutputPath, 'Genetic_Simulation_StepResponse_1', 0, 0, [0, 50, -60, 60])
    %
    %% Differential results
    plotCsvToPDF([motorMitSchwungmasseResourceRoot '/Differential_realResult.csv'],[motorMitSchwungmasseImagesOutputPath '/Differential_realResult.pdf'],2,[stepResponseLegendXPos 0.7 0.1 0.1]);
    %
    %% Learncurve
    differentialSimulationOutputRoot = [motorMitSchwungmasseResourceRoot '/differentialSimulationOutputRoot'];
    mergePlotAndSave(differentialSimulationOutputRoot, 'learning_history', motorMitSchwungmasseImagesOutputPath, 'Differential_Simulation_LearningHistory_1', scoreHistoryLogSetting, 0, [scoreHistoryStartX, 1000, 0, 1000]);
    mergePlotAndSave(differentialSimulationOutputRoot, 'step_response', motorMitSchwungmasseImagesOutputPath, 'Differential_Simulation_StepResponse_1', 0, 0, [0, 50, -60, 60]);
    %
    %% Combined
    plotCsvToPDF([motorMitSchwungmasseResourceRoot '/CombinedMethodes_realResult.csv'],[motorMitSchwungmasseImagesOutputPath '/CombinedMethodes_realResult.pdf'],2,[0.82 0.7 0.1 0.1]);
    combinedLearningHistoryFolderName = 'combinedLearningHistory';
    prepareCombinedLearningHistory(motorMitSchwungmasseResourceRoot, combinedLearningHistoryFolderName, geneticSimulationOutputRoot, differentialSimulationOutputRoot);
    mergePlotAndSave([motorMitSchwungmasseResourceRoot '/combinedLearningHistory'], 'learning_history', motorMitSchwungmasseImagesOutputPath, 'CombinedMethodes_Simulation_LearningHistory_1', scoreHistoryLogSetting, 0, [scoreHistoryStartX, 1000, 0, scoreHistoryMaxY])
    
    
    MotorMitSchwungmassePIDResultAnalyzer;
end


%% Utilities
function fig = mergePlotAndSave(csvRootFolder, csvName, outputFolder, pdfFileName, useLogScale, legendPosition, axisLimits)
    
    if nargin < 5 || length(useLogScale) == 1
        useLogScale = [0, 0];
    end
    if nargin < 4
        pdfFileName = csvName;
    end 
    if nargin < 3
        outputFolder = csvRootFolder;
    end 
    

    mergedTable = getMergedCSV(csvRootFolder, csvName);

    %if nargin >= 7
    %    mergedTable = mergedTable(1:axisLimits(2)+3,:);
    %end

    fig = tableToFigure(mergedTable, 2);

    %% Hide legend of figure
    if nargin < 6
        
    else
        lgd = findobj(fig, 'Type', 'Legend');  % find legend in this figure
        if ~isempty(lgd)
            if length(legendPosition) < 4
                lgd.Visible = 'off';               % hide it
            else                
                lgd.Units = 'normalized';     % could also use 'pixels'
                lgd.Position = legendPosition;  % [left bottom width height]
            end
        end
    end

    ax = fig.CurrentAxes;      % Get the axes
    if useLogScale(1) == 1
        ax.XScale = 'log'; 
    end
    if useLogScale(2) == 1
        ax.YScale = 'log'; 
    end
    if nargin >= 7
        ax.XLim = [axisLimits(1), axisLimits(2)];
        ax.YLim = [axisLimits(3), axisLimits(4)];
    end

    %figure(fig);
    figureToPDF(fig, [outputFolder '/' pdfFileName '.pdf']);
end



function prepareCombinedLearningHistory(resourcePath, combinedLearningHistoryFolderName, geneticSimulationOutputRoot, differentialSimulationOutputRoot)
    filename = 'learning_history.csv';
    geneticDestination = [resourcePath '/' combinedLearningHistoryFolderName '/genetic'];
    differentailDestination = [resourcePath '/' combinedLearningHistoryFolderName '/differential'];
    
    geneticFiles = findFilesByName(geneticSimulationOutputRoot, filename);
    differentialFiles = findFilesByName(differentialSimulationOutputRoot, filename);

    % create folder
    mkdir(geneticDestination);
    mkdir(differentailDestination);

    % Copy all files 
    for i = 1:numel(geneticFiles)
        destinationPath = [geneticDestination '/sub_' char(string(i))];
        mkdir(destinationPath);
        copyfile(geneticFiles{i}, destinationPath);
    end
    for i = 1:numel(differentialFiles)
        destinationPath = [differentailDestination '/sub_' char(string(i))];
        mkdir(destinationPath);
        copyfile(differentialFiles{i}, destinationPath);
    end

    % Changing the colors of the differential tables
    color1 = 'b048fa';
    color2 = '560191';
    differentialFiles = findFilesByName(differentailDestination, filename);
    for i = 1:numel(differentialFiles)
        csvPath = differentialFiles{i};
        newColor = colorGradient(color1, color2, i/numel(differentialFiles));
        % --- Import options for semicolon CSV ---
        opts = detectImportOptions(csvPath, 'Delimiter', ';');

        % Preserve the original variable names exactly
        opts.PreserveVariableNames = true;
    
        % --- Force all columns to char (text) ---
        for c = 1:numel(opts.VariableNames)
            opts = setvartype(opts, opts.VariableNames{c}, 'char');
        end
    
        % --- Skip second row (Params;-) ---
        % Data starts at row 3
        opts.DataLines = [2 Inf];
    
        % Read table
        T = readtable(csvPath, opts);
        T.Durchschnitt(3) = {newColor};
        writetable(T, csvPath, 'Delimiter', ';');
    end


end



function hexOut = colorGradient(hex1, hex2, x)
% COLORGRADIENT Linearly interpolates between two hex colors.
%   hexOut = colorGradient(hex1, hex2, x)
%
%   hex1, hex2 : input colors as hex strings (e.g. '#FF0000' or 'FF0000')
%   x          : interpolation parameter in [0, 1]
%   hexOut     : resulting color as hex string '#RRGGBB'

    % Remove leading '#' if present
    if hex1(1) == '#', hex1 = hex1(2:end); end
    if hex2(1) == '#', hex2 = hex2(2:end); end

    % Convert hex to RGB values (0–255)
    rgb1 = sscanf(hex1, '%2x%2x%2x').';
    rgb2 = sscanf(hex2, '%2x%2x%2x').';

    % Linear interpolation
    rgbOut = (1 - x) * rgb1 + x * rgb2;

    % Convert back to hex
    hexOut = sprintf('%02X%02X%02X', round(rgbOut));
end


function fig = simpleMotorDCGainApproxPlot()
    % Define the piecewise function F(x)
    % Section 1: x in [0, 1], F(x) = 0.43*x^2 + 0.21*x
    % Section 2: x in (1, inf), F(x) = 1.07*x - 0.43
    
    % Create x values for first section [0, 1]
    x1 = linspace(0, 1, 100);
    F1 = 0.43*x1.^2 + 0.21*x1;
    
    % Create x values for second section (1, 10]
    x2 = linspace(1, 3, 100);
    F2 = 1.07*x2 - 0.43;
    
    % Create the plot
    fig = figure;
    hold on;
    
    % Plot first section in blue
    plot(x1, F1, 'b', 'LineWidth', 2);
    
    % Plot second section in orange
    plot(x2, F2, 'Color', [1 0.5 0], 'LineWidth', 2);
    
    % Add labels to each section
    text(0.5, 0.43*0.5^2 + 0.21*0.5 + 0.2, 'f_1', 'FontSize', 12, 'FontWeight', 'bold');
    text(2, 1.07*2 - 0.43 + 0.3, 'f_2', 'FontSize', 12, 'FontWeight', 'bold');
    
    % Add labels and title
    xlabel('x');
    ylabel('DC-Verstärkung(x)');
   % title('Annäherung der DC-Verstärkung');
    grid on;
    hold off;
end

