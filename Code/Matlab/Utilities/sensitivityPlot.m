function fig = sensitivityPlot(sys, varargin)
    % sensitivityPlot - Plot sensitivity function for single or multiple systems
    %
    % Syntax: fig = sensitivityPlot(sys, 'Name', Value, ...)
    %
    % Inputs:
    %   sys     - Single transfer function or cell array of transfer functions
    %
    % Name-Value Pairs:
    %   'Colors'    - Colors for each system (default: MATLAB default colors)
    %                 Can be:
    %                 - Cell array of colors: {'r', 'b', [0 1 0]}
    %                 - Nx3 matrix of RGB values
    %   'FreqRange' - Frequency range [minFreq, maxFreq] in rad/s (default: auto)
    %                 If specified, sensitivity is calculated at these frequencies
    %                 without extrapolation
    %
    % Output:
    %   fig - Figure handle
    
    % Check if input is a cell array
    if iscell(sys)
        systems = sys;
        numSys = length(systems);
    else
        systems = {sys};
        numSys = 1;
    end
    
    % Parse input arguments
    p = inputParser;
    addParameter(p, 'Colors', {});
    addParameter(p, 'FreqRange', []);  % [minFreq, maxFreq] in rad/s
    addParameter(p, 'ShowLegend', true);
    parse(p, varargin{:});
    colors = p.Results.Colors;
    freqRange = p.Results.FreqRange;
    showLegend = p.Results.ShowLegend;
    
    % Handle color input
    if isempty(colors)
        % Use default MATLAB colors
        defaultColors = get(groot, 'defaultAxesColorOrder');
        if numSys > size(defaultColors, 1)
            defaultColors = repmat(defaultColors, ceil(numSys/size(defaultColors,1)), 1);
        end
        colors = mat2cell(defaultColors(1:numSys,:), ones(numSys,1), 3);
    elseif isnumeric(colors) && size(colors, 2) == 3
        % Nx3 color matrix - convert to cell array
        if size(colors, 1) < numSys
            warning('Not enough colors specified. Reusing colors.');
            colors = repmat(colors, ceil(numSys/size(colors,1)), 1);
        end
        colors = mat2cell(colors(1:numSys,:), ones(numSys,1), 3);
    elseif iscell(colors)
        % Cell array of colors
        if length(colors) < numSys
            warning('Not enough colors specified. Reusing colors.');
            colors = repmat(colors, 1, ceil(numSys/length(colors)));
        end
        colors = colors(1:numSys);
    else
        error('Invalid color specification.');
    end
    
    % Determine frequency range
    if isempty(freqRange)
        % Auto-detect from all systems
        allFreqs = [];
        
        for i = 1:numSys
            % Calculate sensitivity function S = 1/(1+L)
            S = feedback(1, systems{i});
            
            % Get frequency response to determine range
            [~, ~, w] = bode(S);
            w = squeeze(w);      % ensure it's a vector
            allFreqs = [allFreqs; w];
        end
        
        % Determine common frequency range
        minFreq = min(allFreqs);
        maxFreq = max(allFreqs);
    else
        % Use specified frequency range
        minFreq = freqRange(1);
        maxFreq = freqRange(2);
    end
    
    % Create a common frequency vector (logarithmically spaced)
    numPoints = 1000;
    commonFreq = logspace(log10(minFreq), log10(maxFreq), numPoints)';
    
    % Create figure
    fig = figure;
    hold on;
    grid on;
    
    % Plot each system
    for i = 1:numSys
        % Calculate sensitivity function S = 1/(1+L)
        S = feedback(1, systems{i});
        
        % Calculate at the common frequency range directly
        [mag, ~, ~] = bode(S, commonFreq);
        magPlot = squeeze(mag);
        
        % Get color for this system
        currentColor = colors{i};
        
        % Plot
        semilogx(commonFreq, magPlot, 'LineWidth', 1.5, 'Color', currentColor, ...
                 'DisplayName', systems{i}.Name);
    end
    
    hold off;
    
    % Labels and formatting
    xlabel('Frequenz [rad/s]');
    ylabel('|S(j\omega)|');
    title('Sensitivität');
    set(gca, 'XScale', 'log');  % Ensure log scale on x-axis
    
    % Show legend if multiple systems
    if numSys > 1 & showLegend == true
        legend('show', 'Location', 'northwest');
    end

    
end