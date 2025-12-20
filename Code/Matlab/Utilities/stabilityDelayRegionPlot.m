function fig = stabilityDelayRegionPlot(G, varargin)
% plotGainDelayStability - Plots stability regions for gain-delay modifications
%
% Inputs:
%   G         - Transfer function of the open-loop system
%   maxDelay  - Maximum time delay to analyze (seconds)
%   numPoints - (optional) Number of grid points per axis (default: 100)
%
% Example:
%   G = tf(1, [1 2 1]);
%   plotGainDelayStability(G, 5, 150);

% Parse input arguments
p = inputParser;
s = tf('s');
addRequired(p, 'G');
addParameter(p, 'MagnitudeRangeDB', [-40, 40]);
addParameter(p, 'DelayRange', [0, 1]);
addParameter(p, 'Resolution', 100);
addParameter(p, 'UseThreads', true);
addParameter(p, 'UseLogscaleDelay', false);
parse(p, G, varargin{:});

MagnitudeRangeDB = p.Results.MagnitudeRangeDB;
delayRange = p.Results.DelayRange;
numPoints = p.Results.Resolution;
useThreads = p.Results.UseThreads;
useLogscaleDelay = p.Results.UseLogscaleDelay;


% Define gain range in dB (symmetric around 0)
dB_range = linspace(MagnitudeRangeDB(1), MagnitudeRangeDB(2), numPoints);

% Define delay range (linear, positive only)
if useLogscaleDelay
    delay_range = logspace(delayRange(1), delayRange(2), numPoints);
else
    if delayRange(1) < 0
        delayRange(1) = 0;
    end
    delay_range = linspace(delayRange(1), delayRange(2), numPoints);
end

% Preallocate stability matrix
stability = zeros(length(delay_range), length(dB_range));

% Extract numerator and denominator for parallel processing
[num_G, den_G] = tfdata(G, 'v');

% Calculate stability for each gain-delay combination using parallel processing
fprintf('Computing stability regions using parallel processing...\n');
fprintf('Grid size: %d x %d = %d points\n', length(delay_range), length(dB_range), ...
    length(delay_range)*length(dB_range));

% Start parallel pool if not already running
if isempty(gcp('nocreate'))
    parpool;
end

tic;
if useThreads
parfor i = 1:length(delay_range)
    stability_row = zeros(1, length(dB_range));
    
    for j = 1:length(dB_range)
        yTime = delay_range(i);
        xGain = 10^(dB_range(j)/20); % Convert dB to linear gain
        
        try
            % Create Pade approximation of delay
            if yTime > 0
                [num_pade, den_pade] = pade(yTime, 4);
                
                % Multiply transfer functions: k * G
                num_k = xGain * conv(num_pade, num_G);
                den_k = conv(den_pade, den_G);
            else
                num_k = xGain * num_G;
                den_k = den_G;
            end
            
            % Create closed-loop system
            % T = G_modified / (1 + G_modified)
            num_T = num_k;
            den_T = den_k + num_k;
            
            % Create transfer function for pole extraction
            T = tf(num_T, den_T);
            
            % Approximate closed-loop system with Pade and check poles
            p = pole(T);
            
            if all(real(p) < 0)
                stability_row(j) = 1; % Stable
            else
                stability_row(j) = 0; % Unstable
            end
        catch
            stability_row(j) = 0; % Consider as unstable if error occurs
        end
    end
    
    stability(i, :) = stability_row;
end
else
for i = 1:length(delay_range)
    stability_row = zeros(1, length(dB_range));
    
    for j = 1:length(dB_range)
        yTime = delay_range(i);
        xGain = 10^(dB_range(j)/20); % Convert dB to linear gain
        
        try
            % Create Pade approximation of delay
            if yTime > 0
                [num_pade, den_pade] = pade(yTime, 4);
                
                % Multiply transfer functions: k * G
                num_k = xGain * conv(num_pade, num_G);
                den_k = conv(den_pade, den_G);
            else
                num_k = xGain * num_G;
                den_k = den_G;
            end
            
            % Create closed-loop system with feedback gain of 8
            % T = G_modified / (1 + G_modified)
            num_T = num_k;
            den_T = den_k + num_k;
            
            % Create transfer function for pole extraction
            T = tf(num_T, den_T);
            
            % Approximate closed-loop system with Pade and check poles
            p = pole(pade(T, 2));
            
            if all(real(p) < 0)
                stability_row(j) = 1; % Stable
            else
                stability_row(j) = 0; % Unstable
            end
        catch
            stability_row(j) = 0; % Consider as unstable if error occurs
        end
    end
    
    stability(i, :) = stability_row;
end
end
elapsed = toc;

fprintf('Computation complete in %.2f seconds!\n', elapsed);

% Create symmetric logarithmic scale for x-axis
% Map dB values to a symmetric log-like scale
% where 0dB is at center, and ±∞ are at edges
x_display = tanh(dB_range / 20); % Maps [-inf, inf] to [-1, 1]

% Create the plot
fig = figure('Position', [100, 100, 500, 500]);

% Plot using pcolor for smooth color transitions
[X, Y] = meshgrid(x_display, delay_range);
h = pcolor(X, Y, stability);
set(h, 'EdgeColor', 'none');
set(h, 'HandleVisibility', 'off'); % Hide from legend

red = [240, 109, 89]/255;
green = [114, 240, 89]/255;
% Set colormap: red for unstable (0), green for stable (1)
colormap([red; green]);
clim([0 1]);

% Dynamically generate x-axis tick positions and labels based on MagnitudeRangeDB
dB_min = MagnitudeRangeDB(1);
dB_max = MagnitudeRangeDB(2);
dB_span = dB_max - dB_min;

% Priority values to always show if in range
priority_values = [0, -3, 3, -6, 6, -10, 10, -20, 20, -40, 40, -80, 80];

% Generate base tick values based on the range
if dB_span <= 20
    % Fine spacing for small ranges
    base_ticks = dB_min:2:dB_max;
elseif dB_span <= 40
    % Medium spacing
    base_ticks = dB_min:5:dB_max;
elseif dB_span <= 80
    % Standard spacing
    base_ticks = dB_min:10:dB_max;
else
    % Coarse spacing for very large ranges
    base_ticks = dB_min:20:dB_max;
end

% Add priority values that fall within the range
tick_dB_values = base_ticks;
for pval = priority_values
    if pval >= dB_min && pval <= dB_max && ~ismember(pval, tick_dB_values)
        tick_dB_values = [tick_dB_values, pval];
    end
end

% Sort the final tick values
tick_dB_values = sort(tick_dB_values);

% Convert dB values to display coordinates using tanh transform
xtick_positions = tanh(tick_dB_values / 20);

% Filter out ticks that are too close together in display space
min_spacing = 0.08; % Minimum spacing in display coordinates
filtered_indices = true(size(tick_dB_values));

for i = 2:length(tick_dB_values)
    if xtick_positions(i) - xtick_positions(i-1) < min_spacing
        % Keep priority values, remove others
        is_priority_current = ismember(tick_dB_values(i), priority_values);
        is_priority_prev = ismember(tick_dB_values(i-1), priority_values);
        
        if is_priority_current && ~is_priority_prev
            filtered_indices(i-1) = false; % Remove previous
        elseif ~is_priority_current
            filtered_indices(i) = false; % Remove current
        end
        % If both are priority, keep both (shouldn't happen with our spacing)
    end
end

% Apply filtering
xtick_positions = xtick_positions(filtered_indices);
tick_dB_values = tick_dB_values(filtered_indices);
xtick_labels = arrayfun(@num2str, tick_dB_values, 'UniformOutput', false);

xticks(xtick_positions);
xticklabels(xtick_labels);

% Labels and title
xlabel('Verstärkung [dB]', 'FontSize', 12, 'FontWeight', 'bold');
ylabel('Totzeit [s]', 'FontSize', 12, 'FontWeight', 'bold');
title('Stabilitätsregion', 'FontSize', 14, 'FontWeight', 'bold');

if useLogscaleDelay
    yscale log
end
% Add grid
grid on;
set(gca, 'Layer', 'top');

% Add a vertical line at 0 dB for reference (only if 0 is in range)
hold on;

if dB_min < 0 && dB_max > 0
    if useLogscaleDelay
        plot([0 0], [10^(delayRange(1)) 10^(delayRange(2))], 'k--', 'LineWidth', 1.5, 'HandleVisibility', 'off');
    else
        plot([0 0], [delayRange(1) delayRange(2)], 'k--', 'LineWidth', 1.5, 'HandleVisibility', 'off');
    end
end

% Add legend with dummy plots for stable/unstable regions
patch(NaN, NaN, green, 'EdgeColor', 'none', 'DisplayName', 'Stabil');
patch(NaN, NaN, red, 'EdgeColor', 'none', 'DisplayName', 'Instabil');
legend('Location', 'northeast', 'FontSize', 10);

hold off;

fprintf('Green region: Stable closed-loop system\n');
fprintf('Red region: Unstable closed-loop system\n');
end