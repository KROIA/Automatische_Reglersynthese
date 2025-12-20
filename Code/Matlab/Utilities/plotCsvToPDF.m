function fig = plotCsvToPDF(csvFile, outputPdf, widthRatio, legendPosition)

    if nargin < 3
        widthRatio = 1;      % default value
    end

    

    % -------- Determine input type and read data --------
    if istable(csvFile)
        % Input is a table - convert to cell array format
        splitRows = tableToSplitRows(csvFile);
    elseif ischar(csvFile) || isstring(csvFile)
        % Input is a file path - read CSV
        raw = readlines(csvFile);
        splitRows = cellfun(@(x) strsplit(x, ';'), raw, 'UniformOutput', false);
    else
        error('Input must be either a CSV file path (string/char) or a MATLAB table.');
    end

    % -------- Read CSV --------
    %raw = readlines(csvFile);
    %splitRows = cellfun(@(x) strsplit(x, ';'), raw, 'UniformOutput', false);

    % -------- Header rows --------
    colNames   = splitRows{1};
    lineStyles = splitRows{2};
    lineWidths = splitRows{3};
    lineColors = splitRows{4};

    yAxisLabel = lineStyles{1};   % Get it before stripping the X column

    % Remove x-axis column
    seriesNames  = colNames(2:end);
    seriesStyles = lineStyles(2:end);
    seriesWidths = lineWidths(2:end);
    seriesColors = lineColors(2:end);

    numSeries = numel(seriesNames);

    % --- Normalize formatting arrays ---
    seriesStyles = padArray(seriesStyles, numSeries, "-"); 
    seriesWidths = padArray(seriesWidths, numSeries, "1.5");
    seriesColors = padArray(seriesColors, numSeries, "0x000000");

    % -------- Parse numeric data safely --------
    dataRows = splitRows(5:end);
    numRows = numel(dataRows);
    numCols = numel(colNames);

    numMatrix = zeros(numRows, numCols);

    for r = 1:numRows
        row = dataRows{r};

        % pad missing columns
        if numel(row) < numCols
            row(end+1:numCols) = {''};
        end

        for c = 1:numCols
            field = strrep(row{c}, ',', '.');
            val = str2double(field);
            if isnan(val)
                numMatrix(r,c) = NaN;
            else
                numMatrix(r,c) = val;
            end
        end
    end

    X = numMatrix(:,1);
    Y = numMatrix(:,2:end);

    % -------- Plotting --------
    fig = figure('Visible','off','Color','white'); 
    hold on;

    for i = 1:numSeries
        lw = str2double(seriesWidths{i});
        if isnan(lw), lw = 1.5; end

        % Parse hex color
        hex = char(seriesColors{i});
        %hex = upper(strrep(seriesColors{i}, "0X", ""));
        if length(hex) ~= 6
            hex = "000000";
        end

        [R, G, B] = hex2rgb(hex);
        %R = hex2dec(hex(1:2)) / 255;
        %G = hex2dec(hex(3:4)) / 255;
        %B = hex2dec(hex(5:6)) / 255;
        color = [R G B];

        plot(X, Y(:,i), ...
            'LineStyle', seriesStyles{i}, ...
            'LineWidth', lw, ...
            'Color', color, ...
            'DisplayName', seriesNames{i});
    end
    % Force the axis to match the CSV data
    xlim([min(X) max(X)]);

    xlabel(colNames{1});
    ylabel(yAxisLabel);
    legend('Location','best');
    
    grid on;

    % -------- Export PDF with tight bounding box --------
    %fig = gcf;
    
    % Remove all unnecessary white margins
    fig.Units = 'inches';
    ax = gca;
    outerpos = ax.OuterPosition;
    ti = ax.TightInset;
    left   = outerpos(1) + ti(1);
    bottom = outerpos(2) + ti(2);
    width  = outerpos(3) - ti(1) - ti(3);
    height = outerpos(4) - ti(2) - ti(4);
    ax.Position = [left bottom width height];

    % ajust aspect ratio
    fig.Position(3) = widthRatio * fig.Position(4);

    if nargin < 4
        %legend('Location','best');
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
    
    % Save PDF tightly
    figureToPDF(fig, outputPdf);
    %exportgraphics(fig, outputPdf, 'ContentType','vector', 'BackgroundColor','white');
end

% ------------ Helper function to convert table to splitRows format ------------
function splitRows = tableToSplitRows(T)
    % Convert a MATLAB table to the same cell array format as CSV reading
    % Expected table format:
    %   - First 4 rows contain metadata (column names, styles, widths, colors)
    %   - Remaining rows contain numeric data
    
    numRows = height(T) + 1;
    numCols = width(T);
    splitRows = cell(numRows, 1);

    rowData = cell(1, numCols);
    for h = 1:numCols
        rowData{h} = T.Properties.VariableNames{h};
    end
    splitRows{1} = rowData;
    
    for r = 2:numRows
        rowData = cell(1, numCols);
        for c = 1:numCols
            val = T{r-1, c};
            if isnumeric(val)
                rowData{c} = num2str(val);
            elseif ischar(val) || isstring(val)
                rowData{c} = char(val);
            else
                rowData{c} = val{1};
            end
        end
        splitRows{r} = rowData;
    end
end

% ------------ Helper function ------------
function arr = padArray(arr, targetLength, defaultValue)
    % Ensures the array has targetLength entries.
    if numel(arr) < targetLength
        arr(end+1:targetLength) = {defaultValue};
    elseif numel(arr) > targetLength
        arr = arr(1:targetLength);
    end
end

function [R, G, B] = hex2rgb(hexStr)
    % HEX2RGB Convert a hex color string to R, G, B values (0–255)
    % Usage:
    %   [R, G, B] = hex2rgb("0077B6");
    %   [R, G, B] = hex2rgb('#0077B6');
    %   [R, G, B] = hex2rgb('0077B6');
    %   [R, G, B] = hex2rgb(0077B6); % MATLAB auto-converts to string
    
    % Ensure input is a string
    hexStr = string(hexStr);

    % Remove quotes if somehow double-quoted inside a char array
    hexStr = replace(hexStr, """", "");

    % Remove leading '#' if present
    if startsWith(hexStr, "#")
        hexStr = extractAfter(hexStr, 1);
    end

    % Validate length
    if strlength(hexStr) ~= 6
        error("Hex string must be 6 characters long.");
    end

    % Parse R, G, B as hex
    R = hex2dec(extractBetween(hexStr, 1, 2)) / 255;
    G = hex2dec(extractBetween(hexStr, 3, 4)) / 255;
    B = hex2dec(extractBetween(hexStr, 5, 6)) / 255;
end
