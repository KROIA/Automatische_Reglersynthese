function fig = stabilityRegionPlot(G, varargin)
% STABILITY_REGION_PLOT - Plots stability regions in log-scaled complex plane
%
% Uses logarithmic scaling from NyquistLog where:
%   L(M) = M^(log10(2))           if M < 1
%   L(M) = 2 - M^(-log10(2))      if M >= 1
%
% This maps:
%   - Unit circle (|z|=1, 0dB) to radius = 1
%   - Infinity to radius = 2
%
% Syntax: stability_region_plot(G)
%         stability_region_plot(G, Name, Value)
%
% Inputs:
%    G - Open-loop transfer function (tf, zpk, or ss object)
%
% Optional Name-Value pairs:
%    'MagnitudeRange' - [min, max] magnitude in log scale (default: [-2, 2])
%    'AngleRange'     - [min, max] angle in degrees (default: [-180, 180])
%    'Resolution'     - Number of points in each dimension (default: 300)
%    'UseThreads'     - true or false, to enable/disable multithreaded grid calculation
%
% Example:
%    G = tf(1, [1 2 1]);
%    stability_region_plot(G);

    % Parse input arguments
    p = inputParser;
    %s = tf('s');
    addRequired(p, 'G');
    addParameter(p, 'MagnitudeRange', [-4, 4]);
    addParameter(p, 'AngleRange', [-180, 180]);
    addParameter(p, 'Resolution', 300);
    addParameter(p, 'UseThreads', true);
    addParameter(p, 'LegendRect', [0.7    0.85    0.1343    0.0770]);
    addParameter(p, 'UseTimeDelay', false);
    parse(p, G, varargin{:});
    
    magRange = p.Results.MagnitudeRange;
    angleRange = p.Results.AngleRange;
    res = p.Results.Resolution;
    useThreads = p.Results.UseThreads;
    legendRect = p.Results.LegendRect;
    useTimeDelay = p.Results.UseTimeDelay;
    
    % Convert to transfer function if needed
    if ~isa(G, 'tf')
        G = tf(G);
    end
    
    % Create magnitude and angle grids
    log_mags = linspace(magRange(1), magRange(2), res);
    angles = linspace(angleRange(1), angleRange(2), res);
    
    [LogM, Theta] = meshgrid(log_mags, angles);
    
    % Convert to actual magnitudes
    M = 10.^LogM;
    
    % Create complex number grid: z = M * exp(j*Theta)
    Z = M .* exp(1j * Theta * pi/180);
    
    % Initialize stability matrix
    isStable = zeros(size(Z));
    
    % Test stability for each complex number
    fprintf('Computing stability regions in complex plane...\n');
    fprintf('Testing %d complex values...\n', numel(Z));
    
    
    if useThreads
        parfor i = 1:numel(Z)
            z = Z(i);
        
            try
                if useTimeDelay
                    [num, den] = pade(abs(imag(z)),4);
                    k = real(z) * tf(num, den);
                else
                    k = z;
                end

                %k = real(z)* exp(-s*abs(imag(z)));

                G_modified = k * G;
                T = feedback(G_modified, 1);
                p = pole(pade(T,2));
        
                if all(real(p) < 0)
                    isStable(i) = 1;
                else
                    isStable(i) = 0;
                end
            catch
                isStable(i) = 0;
            end
        end
    else
        for i = 1:numel(Z)
            z = Z(i);
            
            % Multiply complex number with G: z*G
            % Closed-loop system: T = (z*G) / (1 + z*G)
            try
                G_modified = z * G;
                T = feedback(G_modified, 1);
                p = pole(T);
                
                % System is stable if all poles have negative real parts
                if all(real(p) < 0)
                    isStable(i) = 1;
                end
            catch
                isStable(i) = 0;
            end
            
            % Progress indicator
            if mod(i, floor(numel(Z)/20)) == 0
                fprintf('Progress: %.0f%%\n', 100*i/numel(Z));
            end
        end
    end
    %
    

    
    % Apply logarithmic transformation for plotting
    % This is the key transformation from NyquistLog
    n = 2; % Base of the L function
    MD = abs(Z);
    PH = angle(Z);
    
    % Apply L(M) transformation
    MD_L = MD.^(log10(n));
    ind = find(MD_L > 1);
    MD_L(ind) = 2 - 1./MD_L(ind);
    
    % Convert to Cartesian coordinates in log-space
    Z_L = MD_L .* exp(1j * PH);
    X_plot = real(Z_L);
    Y_plot = imag(Z_L);
    
    % Create the plot
    fig = figure('Color', 'w', 'Position', [100, 100, 500, 500]);
    % Remove tick labels (the numbers) but keep the axis lines
    grid off;
    
    
    hold on;
    % Plot the stability regions with transparency
    h_contour = contourf(X_plot, Y_plot, isStable, 1, 'LineStyle', 'none');
    
    % Create custom colormap: red for unstable (0), green for stable (1)
    red = [240, 109, 89]/255;
    green = [114, 240, 89]/255;

    cmap = [red; green]; 
    colormap(cmap);
    
    % Set transparency - find patch objects in current axes
    %drawnow;
    %ax = gca;
    %patches = findobj(ax, 'Type', 'Patch');
    %set(patches, 'FaceAlpha', 0.5);
    
    % Add contour line at boundary
    % contour(X_plot, Y_plot, isStable, [0.5 0.5], 'k-', 'LineWidth', 2.5);




    % Draw the log-scaled grid structure first
    Nd = 4; % Number of level lines
    
    % Infinity circle (radius = 2)
    plot(2*exp(1i*(0:0.01:1)*2*pi), 'k-', 'LineWidth', 2);
    
    % Unit circle (0 dB, radius = 1)
    plot(exp(1i*(0:0.01:1)*2*pi), 'k-', 'LineWidth', 2);
    
    % Critical point -1
    plot(exp(1i*pi), '*b', 'MarkerSize', 10, 'LineWidth', 2);
    text(-1.2, 0.0, '-1', 'FontSize', 10, 'FontWeight', 'bold');
    
    % 0 dB label
    dbLabelOffset = 1.1;
    xx = 1*exp(1i*pi/4) * dbLabelOffset;
    text(real(xx), imag(xx), '0dB', 'FontSize', 9, 'FontWeight', 'bold');
    
    % dB circles
    for ii = 1:Nd
        % Negative dB (inside unit circle)
        plot(1/(n^ii)*exp(1i*(0:0.01:1)*2*pi), ':', 'LineWidth', 0.8);
        % Positive dB (between unit and infinity circle)
        plot((2-1/(n^ii))*exp(1i*(0:0.01:1)*2*pi), ':', 'LineWidth', 0.8);
        
        if ii < 4
            % Negative dB labels
            xx = 1/(n^ii)*exp(1i*pi/4);
            text(real(xx), imag(xx), [num2str(-ii*20) 'dB'], 'FontSize', 8);
            % Positive dB labels
            xx = (2-n^(-ii))*exp(1i*pi/4);
            text(real(xx), imag(xx), [num2str(ii*20) 'dB'], 'FontSize', 8);
        end
    end
    
    % Radial angle lines (sectors of π/6)
    for ii = 1:12
        ps = 2*exp(1i*ii*pi/6);
        plot([0 real(ps)], [0 imag(ps)], ':', 'Color', [0.6 0.6 0.6], 'LineWidth', 0.5);
    end
    
    % Axes through origin
    %plot([-1 1]*2, [0 0], ':', 'LineWidth', 0.5);
    %plot([0 0], [-1 1]*2, ':', 'LineWidth', 0.5);
    

    
    % Labels and formatting
    
    if useTimeDelay
        xlabel('Verstärkung [dB]', 'FontSize', 12, 'FontWeight', 'bold');
        ylabel('Zeitverzögerung [dB]', 'FontSize', 12, 'FontWeight', 'bold');
    else
        xlabel('Reale Verstärkung [dB]', 'FontSize', 12, 'FontWeight', 'bold');
        ylabel('Imaginäre Verstärkung [dB]', 'FontSize', 12, 'FontWeight', 'bold');
    end
    title({'Stabilitätsregion'}, ...
          'FontSize', 14, 'FontWeight', 'bold');
    
    axis equal;
    axis([-1 1 -1 1]*2.2);
    

    %axes('box','off','xtick',[],'ytick',[],'ztick',[],'xcolor',[1 1 1],'ycolor',[1 1 1]);
    ax = gca; 
    %set(ax, 'FontSize', 11);
    %set(ax, 'XTickLabel', []);
    %set(ax, 'YTickLabel', []);

    ax.XAxis.TickValues = [];
    ax.YAxis.TickValues = [];
    % Remove rectangular grid
    grid off;
    
    % Add legend
    h1 = patch(NaN, NaN, green, 'EdgeColor', 'none');
    h2 = patch(NaN, NaN, red, 'EdgeColor', 'none');
    legend([h1, h2], {'Stabil', 'Instabil'}, ...
           'Location', 'northeast', 'FontSize', 10);
    
    ax.Legend.Position = legendRect;

    hold off;
    
    % Create second plot: Magnitude-Phase representation
    %figure('Color', 'w', 'Position', [150, 150, 900, 700]);
    %
    %hold on;
    %h_contour2 = contourf(LogM, Theta, isStable, 1, 'LineStyle', 'none');
    %colormap(cmap);
    %drawnow;
    %ax2 = gca;
    %patches2 = findobj(ax2, 'Type', 'Patch');
    %set(patches2, 'FaceAlpha', 0.6);
    %contour(LogM, Theta, isStable, [0.5 0.5], 'k-', 'LineWidth', 2);
    %
    %xlabel('Log_{10}(Betrag)', 'FontSize', 12, 'FontWeight', 'bold');
    %ylabel('Phasenwinkel (Grad)', 'FontSize', 12, 'FontWeight', 'bold');
    %title({'Stabilitätsregionen: Log-Betrag vs Phase', ...
    %       'Grün = Stabil | Rot = Instabil'}, ...
    %      'FontSize', 14, 'FontWeight', 'bold');
    %
    %grid on;
    %set(gca, 'GridAlpha', 0.3, 'FontSize', 11);
    %
    % % Add magnitude labels on x-axis
    %xtick_vals = unique(round(linspace(magRange(1), magRange(2), 9)));
    %xticks(xtick_vals);
    %xticklabels(arrayfun(@(x) sprintf('10^{%d}', x), xtick_vals, 'UniformOutput', false));
    %
    %h1 = patch(NaN, NaN, [0.2 0.8 0.2], 'EdgeColor', 'none', 'FaceAlpha', 0.6);
    %h2 = patch(NaN, NaN, [1 0.2 0.2], 'EdgeColor', 'none', 'FaceAlpha', 0.6);
    %legend([h1, h2], {'Stabile Region', 'Instabile Region'}, ...
    %       'Location', 'best', 'FontSize', 11);
    %
    %hold off;
    
    % Display system information
    fprintf('\n--- Systemanalyse abgeschlossen ---\n');
    fprintf('System: %s\n', formattedDisplayText(G));
    fprintf('Komplexe Ebene Abdeckung:\n');
    fprintf('  Betragsbereich: 10^%.1f bis 10^%.1f (%.3f bis %.1f)\n', ...
            magRange(1), magRange(2), 10^magRange(1), 10^magRange(2));
    fprintf('  Winkelbereich: %.0f° bis %.0f°\n', angleRange(1), angleRange(2));
    fprintf('Stabile Region Abdeckung: %.1f%%\n', 100*sum(isStable(:))/numel(isStable));
    
end