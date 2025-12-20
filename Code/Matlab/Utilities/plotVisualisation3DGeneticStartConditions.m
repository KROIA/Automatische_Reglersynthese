function fig = plotVisualisation3DGeneticStartConditions(varargin)

% Random 3D Shape Generator
% This script creates a 3D surface plot of a randomly generated function

% Parse input arguments
p = inputParser;
addParameter(p, 'PointCount', 20);
addParameter(p, 'ScatterRadius', 0.3);
addParameter(p, 'Position', [-1.6 1.7]);
addParameter(p, 'ViewAngle', [-45 45]);
parse(p, varargin{:});

pointCount = p.Results.PointCount;
scatterRadius = p.Results.ScatterRadius;
position = p.Results.Position;
viewAngle = p.Results.ViewAngle;
%delayRange = p.Results.DelayRange;
%numPoints = p.Results.Resolution;
%useThreads = p.Results.UseThreads;
%useLogscaleDelay = p.Results.UseLogscaleDelay;

% Generate random positions for the minima
% Global minimum at origin (with some randomness)
global_x = 0.5 * 2 - 1;  % Between -1 and 1
global_y = -0.2 * 2 - 1;

% Two local minima at random positions (but not too close to global)
angle1 = 0.3 * 2 * pi;
angle2 = angle1 + pi * (0.6 + 0.5 * 0.8);  % Roughly opposite side
radius1 = 2 +  1.5;  % Distance from origin
radius2 = 2 + 1.5;

local1_x = global_x + radius1 * cos(angle1);
local1_y = global_y + radius1 * sin(angle1);
local2_x = global_x + radius2 * cos(angle2);
local2_y = global_y + radius2 * sin(angle2);

% Random depths for the minima
global_depth = -5 - rand() * 3;  % Deepest (most negative)
local1_depth = -2 - rand() * 1.5;  % Shallower
local2_depth = -2 - rand() * 1.5;

% Random widths for the wells
global_width = 0.5 + rand() * 0.5;
local1_width = 0.4 + rand() * 0.4;
local2_width = 0.4 + rand() * 0.4;

% Define the function to calculate Z for given (x,y) pairs
calcZ = @(x, y) -(global_depth * exp(-((x - global_x).^2 + (y - global_y).^2) / global_width) + ...
                local1_depth * exp(-((x - local1_x).^2 + (y - local1_y).^2) / local1_width) + ...
                local2_depth * exp(-((x - local2_x).^2 + (y - local2_y).^2) / local2_width) + ...
                0.15 * (x.^2 + y.^2));

% Define the grid
[X, Y] = meshgrid(-5:0.1:5, -5:0.1:5);

% Create the surface using the function
Z = calcZ(X, Y);

% Create the 3D surface plot
fig = figure('Position', [100, 100, 800, 600]);
surf(X, Y, Z, 'EdgeColor', 'none', 'FaceAlpha', 1);

% Customize the plot
colormap(jet);
colorbar;
shading interp;
lighting gouraud;
camlight('headlight');

% Labels and title
xlabel('X-axis', 'FontSize', 12);
ylabel('Y-axis', 'FontSize', 12);
zlabel('Z = f(X,Y)', 'FontSize', 12);
title('2D Optimierungsproblem', 'FontSize', 14, 'FontWeight', 'bold');

% Set view angle
view(viewAngle);

% Add grid
grid on;
axis tight;

% Add a group of red points around x=-1.5, y=3.3
hold on;
%pointCount = 20;  % Number of points in the group
%scatter_radius = 0.3;  % Radius of the point cloud

% Generate random points around the specified location
points_x = position(1) + scatterRadius * (rand(pointCount, 1) * 2 - 1);
points_y = position(2) + scatterRadius * (rand(pointCount, 1) * 2 - 1);

% Calculate Z values for these points using the function
points_z = calcZ(points_x, points_y);

% Plot the red points
scatter3(points_x, points_y, points_z, 50, 'r', 'filled', 'MarkerEdgeColor', 'k', 'LineWidth', 1);
hold off;

% Display information
fprintf('Generated a 3D shape with 1 global minimum and 2 local minima\n');
fprintf('Global minimum at: (%.2f, %.2f) with depth %.2f\n', global_x, global_y, global_depth);
fprintf('Local minimum 1 at: (%.2f, %.2f) with depth %.2f\n', local1_x, local1_y, local1_depth);
fprintf('Local minimum 2 at: (%.2f, %.2f) with depth %.2f\n', local2_x, local2_y, local2_depth);
fprintf('Z range: [%.2f, %.2f]\n', min(Z(:)), max(Z(:)));

end