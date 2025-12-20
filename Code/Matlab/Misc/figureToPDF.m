function fig = figureToPDF(fig, outputPdf, varargin)
% FIGURETOPDF Save a figure to a PDF and create folders if needed
%
%   fig = figureToPDF(fig, outputPdf)
%
% Inputs:
%   fig       - figure handle
%   outputPdf - full path to output PDF file
%
% Output:
%   fig       - the same figure handle

    % Check inputs
    if nargin < 2 || isempty(outputPdf)
        error('You must provide a figure handle and an output PDF path.');
    end

    % Parse input arguments
    p = inputParser;
    %addRequired(p, 'G');
    addParameter(p, 'ContentType', 'vector');
    addParameter(p, 'Resolution', 600);
    addParameter(p, 'WidthRatio', -1);
    parse(p, varargin{:});
    
    contentType = p.Results.ContentType;
    resolution = p.Results.Resolution;
    widthRatio = p.Results.WidthRatio;


    % Extract folder from output path
    [folderPath, ~, ~] = fileparts(outputPdf);

    % Create folder if it doesn't exist
    if ~isempty(folderPath) && ~exist(folderPath, 'dir')
        mkdir(folderPath);
    end

    
    % Set figure to white background
    %set(fig, 'Color', 'white');
    
    % Get all axes in the figure
    %allAxes = findall(fig, 'type', 'axes');
    
    % Set each axes to light mode colors
    %for i = 1:length(allAxes)
    %    set(allAxes(i), 'Color', 'white', ...
    %                    'XColor', [0 0 0], ...
    %                    'YColor', [0 0 0], ...
    %                    'ZColor', [0 0 0], ...
    %                    'GridColor', [0.15 0.15 0.15], ...
    %                    'MinorGridColor', [0.1 0.1 0.1]);
    %end
    
    % Also update text colors if needed
    allText = findall(fig, 'Type', 'text');
    set(allText, 'Color', 'black');

    if widthRatio > 0
        % ajust aspect ratio
        fig.Position(3) = widthRatio * fig.Position(4);
    end

    % Save PDF tightly
    exportgraphics(fig, outputPdf, ...
        'ContentType', contentType, ...
        'Resolution', resolution, ...
        'BackgroundColor', 'white');


end
