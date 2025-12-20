clear; clc;

baseDir = "SimpleMotor/TestAmRealenProzess/StepResponses";
subfolders = ["DifferentialEvol", "Genetic", "Systune"];
csvNames = [
    "SimpleDCMotorStepDataStep1.csv"
    "SimpleDCMotorStepDataStep2.csv"
    "SimpleDCMotorStepDataStep3.csv"
];

dataPlot(baseDir, subfolders, csvNames);


function dataPlot(baseDir, subfolders, csvNames)
    figure; hold on;

    for f = 1:length(subfolders)
        folder = subfolders(f);
        folderPath = fullfile(baseDir, folder);
    
        % Load all CSVs
        data = cell(1, length(csvNames));
        %minRows = inf;
    
        for i = 1:length(csvNames)
            filePath = fullfile(folderPath, csvNames(i));
            data{i} = readmatrix(filePath);
            data{i} = data{i}(1:2000, :);
            plot(data{i}(:,3), "DisplayName", filePath);
    
            % Track smallest row count for alignment
            %minRows = min(minRows, size(data{i}, 1));
        end
    
        % Trim all to same length
        %for i = 1:length(data)
        %    data{i} = data{i}(1:minRows, :);
        %end
    
        % Average all CSVs (row-wise)
        %avgData = zeros(minRows, size(data{1}, 2));
        for i = 1:length(data)
            %avgData = avgData + data{i};
            %plot(data{i}, "DisplayName", name);
        end
        %avgData = avgData / length(data);
    
        % Store third column
        %thirdCols(folder) = avgData(:, 3);
    end

    grid on;
end

function averagedPlot(baseDir, subfolders, csvNames)
    thirdCols = containers.Map();
    for f = 1:length(subfolders)
        folder = subfolders(f);
        folderPath = fullfile(baseDir, folder);
    
        % Load all CSVs
        data = cell(1, length(csvNames));
        minRows = inf;
    
        for i = 1:length(csvNames)
            filePath = fullfile(folderPath, csvNames(i));
            data{i} = readmatrix(filePath);
    
            % Track smallest row count for alignment
            minRows = min(minRows, size(data{i}, 1));
        end
    
        % Trim all to same length
        for i = 1:length(data)
            data{i} = data{i}(1:minRows, :);
        end
    
        % Average all CSVs (row-wise)
        avgData = zeros(minRows, size(data{1}, 2));
        for i = 1:length(data)
            avgData = avgData + data{i};
        end
        avgData = avgData / length(data);
    
        % Store third column
        thirdCols(folder) = avgData(:, 3);
    end
    
    % ---- PLOT RESULTS ----
    figure; hold on;
    
    keysList = keys(thirdCols);
    for i = 1:length(keysList)
        name = keysList{i};
        plot(thirdCols(name), "DisplayName", name);
    end
    
    title("Averaged Third Column of Step Responses");
    xlabel("Index");
    ylabel("Value");
    legend show;
    grid on;
end
