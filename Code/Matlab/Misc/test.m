% Define the piecewise function F(x)
% Section 1: x in [0, 1], F(x) = 0.43*x^2 + 0.21*x
% Section 2: x in (1, inf), F(x) = 1.07*x - 0.43

% Create x values for first section [0, 1]
x1 = linspace(0, 1, 100);
F1 = 0.43*x1.^2 + 0.21*x1;

% Create x values for second section (1, 10]
x2 = linspace(1, 3, 100);
F2 = 1.07*x2 - 0.43;

% Create the plot
figure;
hold on;

% Plot first section in blue
plot(x1, F1, 'b', 'LineWidth', 2);

% Plot second section in orange
plot(x2, F2, 'Color', [1 0.5 0], 'LineWidth', 2);

% Add labels to each section
text(0.5, 0.43*0.5^2 + 0.21*0.5 + 0.2, 'f_1', 'FontSize', 12, 'FontWeight', 'bold');
text(2, 1.07*2 - 0.43 + 0.3, 'f_2', 'FontSize', 12, 'FontWeight', 'bold');

% Add labels and title
xlabel('x');
ylabel('DC-Verstärkung(x)');
title('Approximation der DC-Verstärkung');
grid on;
hold off;