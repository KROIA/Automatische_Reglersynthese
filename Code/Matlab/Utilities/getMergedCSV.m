function mergedTable = getMergedCSV(parentFolder, subfolderCSVName)
    %outputFile = fullfile(parentFolder, [csvName '_merged']);
    
    fileList = findFilesByName(parentFolder, [subfolderCSVName '.csv']);
   % fileList = dir(fullfile(parentFolder, '**', [subfolderCSVName '.csv']));
    if isempty(fileList)
        error('No "%s" files found.', [subfolderCSVName '.csv']);
    end
    
    tables = {};
    maxRows = 0;
    
    for k = 1:numel(fileList)
        csvPath = fileList{k};
        fprintf('Reading: %s\n', csvPath);
    
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
    
        % --- Remove first column only if not first CSV ---
        if k > 1
            T = T(:, 2:end);
        end
    
        % Prefix column names by folder name
        %[~, folderName] = fileparts(fileList(k).folder);
        %prefix = matlab.lang.makeValidName(folderName);
        if k > 1
            T.Properties.VariableNames = strcat(T.Properties.VariableNames, "_", string(k));
        else
            for i=2:numel(T.Properties.VariableNames)
                T.Properties.VariableNames{i} = char(strcat(T.Properties.VariableNames{i}, "_", string(k)));
            end
        end
    
        % Track max rows
        maxRows = max(maxRows, height(T));
    
        tables{k} = T;
    end
    
    % --- Pad tables ---
    for k = 1:numel(tables)
        T = tables{k};
        if height(T) < maxRows
            missing = maxRows - height(T);
            pad = cell2table( repmat({''}, missing, width(T)), ...
                'VariableNames', T.Properties.VariableNames );
            T = [T; pad];
        end
        tables{k} = T;
    end
    
    % --- Horizontal merge ---
    mergedTable = tables{1};
    for k = 2:numel(tables)
        mergedTable = [mergedTable tables{k}];
    end
    
    % --- Write semicolon CSV ---
    %writetable(mergedTable, [outputFile '.csv'], 'Delimiter', ';');
    %figure(plotCsvToPDF([outputFile '.csv'], [outputFile '.pdf'],3));
    
    table = mergedTable;
    %fprintf('\nMerged horizontally into:\n%s\n', outputFile);
end