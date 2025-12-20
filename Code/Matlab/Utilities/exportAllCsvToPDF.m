function exportAllCsvToPDF(folderPath, outputFolder)
% process_all_csv(folderPath, outputFolder)
% Searches the folder for all .csv files and exports a PDF for each one.
%
% INPUTS:
%   folderPath   : Folder containing CSV files
%   outputFolder : (optional) Folder to save PDFs. If not set, PDFs are
%                  saved in the same folder as the CSVs.

    if nargin < 1
        error("You must specify the folder path.");
    end

    if ~isfolder(folderPath)
        error("Folder does not exist: %s", folderPath);
    end

    % Find all CSV files in the folder
    csvFiles = dir(fullfile(folderPath, "*.csv"));

    if isempty(csvFiles)
        fprintf("No CSV files found in folder: %s\n", folderPath);
        return;
    end

    % If output folder is provided, create it if it doesn't exist
    useCustomOutput = nargin >= 2 && ~isempty(outputFolder);
    if useCustomOutput
        if ~isfolder(outputFolder)
            mkdir(outputFolder);
        end
    end

    fprintf("Found %d CSV files.\n", numel(csvFiles));

    % Process each CSV
    for k = 1:numel(csvFiles)
        csvPath = fullfile(folderPath, csvFiles(k).name);

        % Determine PDF output path
        [~, baseName, ~] = fileparts(csvFiles(k).name);
        if useCustomOutput
            pdfPath = fullfile(outputFolder, baseName + ".pdf");
        else
            pdfPath = fullfile(folderPath, baseName + ".pdf");
        end

        fprintf("Processing %s --> %s\n", csvFiles(k).name, pdfPath);

        try
            plotCsvToPDF(csvPath, pdfPath);
        catch ME
            fprintf("Error processing %s:\n%s\n", csvFiles(k).name, ME.message);
        end
    end

    fprintf("Done.\n");
end
