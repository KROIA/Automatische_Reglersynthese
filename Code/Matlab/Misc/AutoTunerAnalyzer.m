
close all;
addpath('Utilities');

% Parent folder containing subfolders
parentFolder = '../CPP/AutoTuner/build/Results/StepResponses';


% The CSV filename you expect in each subfolder (change as needed)
%csvName = 'parameter_changes';
%csvName = 'learning_history';

%mergePlotAndSave(parentFolder, 'parameter_changes', [parentFolder]);
%mergePlotAndSave(parentFolder, 'learning_history', [parentFolder]);
%mergePlotAndSave(parentFolder, 'step_response', [parentFolder]);
plotCsvToPDF('../CPP/AutoTuner/build/Results/StepResponses/real/step_response.csv','../CPP/AutoTuner/build/Results/StepResponses/real/MotorSystemSystune_realResult.pdf',2,[0.7 0.7 0.7 0.7])
%mergePlotAndSave(parentFolder, 'step_response', [parentFolder]);
%outputFile = fullfile(parentFolder, [csvName '_merged']);
%mergedTable = getMergedCSV(parentFolder, csvName);
%fig = tableToFigure(mergedTable, 3);
%figure(fig);
%figureToPDF(fig, [outputFile '.pdf']);



function fig = mergePlotAndSave(csvRootFolder, csvName, outputFolder, pdfFileName)
    if nargin < 4
        pdfFileName = csvName;
    end 
    if nargin < 3
        outputFolder = csvRootFolder;
    end 

    mergedTable = getMergedCSV(csvRootFolder, csvName);
    fig = tableToFigure(mergedTable, 2);

    %% Hide legend of figure
    lgd = findobj(fig, 'Type', 'Legend');  % find legend in this figure
    %if ~isempty(lgd)
    %    lgd.Visible = 'off';               % hide it
    %end
    %lgd.Units = 'normalized';     % could also use 'pixels'
    %lgd.Position = [0.7 0.7 0.2 0.2];  % [left bottom width height]

    %changeYScale(fig);

    figure(fig);
    figureToPDF(fig, [outputFolder '/' pdfFileName '.pdf']);
end

function changeYScale(fig)
    ax = fig.CurrentAxes;      % Get the axes
    ax.YScale = 'log';         % Set y-axis to log scale
end

