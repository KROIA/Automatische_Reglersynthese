% Sandbox um schnell sachen zu probieren...

%delays = [0, 0.005 0.01];
delays = linspace(0,0.05,100);

%systems = {systuneSystem};
systems = {geneticSystem};
%systems = {differentialSystem};
systems = {systuneSystem, geneticSystem, differentialSystem};
startColors = {'#3189f5', '#2cf533', '#cc2cf5'};
endColors   = {'#bbd8fa', '#9dfca0', '#e1a0f2'};

systemsList = {};
colorList = [];
for i = 1:length(delays)
    delay = delays(i);

    colorGradientValue = i/(length(delays)-1);
    timeDelayTF = exp(-s * delay);

    for j = 1:length(systems)
        colorList = [colorList, {colorGradient(startColors{j}, endColors{j}, colorGradientValue)}];
        newSys = systems{j} * timeDelayTF;
        newSys.Name = ['(' systems{j}.Name ') * exp(-s*' char(string(delay)) ')'];
        systemsList{end+1} = newSys;
    end
end
%systemsCells = num2cell(systemsList);
sensitivityPlot(systemsList, 'Colors', colorList, 'ShowLegend', false, 'FreqRange', [10^(0), 10^2]);


function color = colorGradient(color1, color2, t)
    % COLORGRADIENT Linearly interpolates between two hex colors.
    %
    % color1, color2 : char arrays like '#RRGGBB'
    % t              : interpolation value in [0,1]
    % color          : resulting hex color '#RRGGBB'
    
    % Remove leading # if present
    if color1(1) == '#', color1 = color1(2:end); end
    if color2(1) == '#', color2 = color2(2:end); end

    % Convert hex to numeric RGB (0–255)
    rgb1 = sscanf(color1, '%2x%2x%2x').';
    rgb2 = sscanf(color2, '%2x%2x%2x').';

    % Linear interpolation
    rgb = (1 - t) * rgb1 + t * rgb2;
    rgb = round(rgb); % round to nearest integer

    % Clamp to valid range (just in case)
    rgb = max(0, min(255, rgb));

    % Convert back to hex string
    color = sprintf('#%02X%02X%02X', rgb(1), rgb(2), rgb(3));
end