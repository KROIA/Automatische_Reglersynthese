% Constrained Polynomial Fit with 4 conditions
% F(0) = 0
% F(500) = 0.316
% dF/dx(500) = 0.003636
% F(x) > 0 for all x > 0
% dF/dx > 0 for all x > 0 (monotonically increasing)

clear; clc;

% Define symbolic variables
syms a b c x

% Define polynomial F(x) = a*x^2 + b*x + c
% Since F(0) = 0, this means c = 0
% So F(x) = a*x^2 + b*x
F = a*(exp(b*x) - 1);

% Derivative dF/dx = 2*a*x + b
dF = diff(F, x);

fprintf('Function: F(x) = a*x^2 + b*x\n');
fprintf('Derivative: dF/dx = %s\n\n', char(dF));

% Set up the system of equations
% Condition 1: F(0) = 0 (automatically satisfied with c = 0)
% Condition 2: F(500) = 0.316
eq1 = subs(F, x, 500) == 0.316;

% Condition 3: dF/dx(500) = 0.003636
eq2 = subs(dF, x, 500) == 0.003636;

% Solve the system
solution = solve([eq1, eq2], [a, b]);

% Extract solutions
a_sol = double(solution.a);
b_sol = double(solution.b);

% Display results
fprintf('Solution:\n');
fprintf('a = %.10e\n', a_sol);
fprintf('b = %.10e\n', b_sol);
fprintf('\nFinal Formula:\n');
fprintf('F(x) = %.10e*x^2 + %.10e*x\n\n', a_sol, b_sol);

% Verify the solution
F_0 = a_sol * 0^2 + b_sol * 0;
F_500 = a_sol * 500^2 + b_sol * 500;
dF_500 = 2 * a_sol * 500 + b_sol;

fprintf('Verification:\n');
fprintf('F(0) = %.10f (should be 0)\n', F_0);
fprintf('F(500) = %.10f (should be 0.316)\n', F_500);
fprintf('dF/dx(500) = %.10f (should be 0.003636)\n\n', dF_500);

% Check positivity for x > 0
fprintf('Constraint Checks:\n');
fprintf('================\n');

% Check 1: F(x) > 0 for all x > 0
% For F(x) = ax^2 + bx = x(ax + b) > 0 for x > 0
% This requires: ax + b > 0 for all x > 0
if a_sol > 0 && b_sol >= 0
    fprintf('✓ F(x) > 0 for all x > 0 (a > 0 and b >= 0)\n');
elseif a_sol > 0
    x_min = -b_sol / (2 * a_sol);
    if x_min <= 0
        fprintf('✓ F(x) > 0 for all x > 0 (minimum at x = %.2f <= 0)\n', x_min);
    else
        F_min = a_sol * x_min^2 + b_sol * x_min;
        if F_min > 0
            fprintf('✓ F(x) > 0 for all x > 0 (minimum value = %.6f > 0)\n', F_min);
        else
            fprintf('✗ WARNING: F(x) has negative values for x > 0\n');
            fprintf('  Minimum at x = %.2f with F(x) = %.6f\n', x_min, F_min);
        end
    end
else
    fprintf('✗ WARNING: F(x) not positive for all x > 0 (a <= 0)\n');
end

% Check 2: dF/dx > 0 for all x > 0
% dF/dx = 2*a*x + b
% For this to be positive for all x > 0:
% - If a >= 0 and b > 0, then dF/dx > 0 for all x > 0
% - If a < 0, then minimum of dF/dx occurs as x → ∞ (decreasing)
fprintf('\n');
if a_sol >= 0 && b_sol > 0
    fprintf('✓ dF/dx > 0 for all x > 0 (a >= 0 and b > 0)\n');
    fprintf('  Derivative is monotonically increasing\n');
    dF_min = b_sol;
    fprintf('  Minimum derivative at x = 0+: dF/dx = %.6f\n', dF_min);
elseif a_sol >= 0 && b_sol == 0
    fprintf('✗ WARNING: dF/dx = 0 at x = 0 (b = 0)\n');
    fprintf('  Function is not strictly increasing at x = 0+\n');
elseif a_sol < 0
    fprintf('✗ WARNING: dF/dx not positive for all x > 0 (a < 0)\n');
    fprintf('  Derivative is decreasing, will eventually become negative\n');
    x_zero_slope = -b_sol / (2 * a_sol);
    if x_zero_slope > 0
        fprintf('  dF/dx = 0 at x = %.2f\n', x_zero_slope);
    end
else
    % b < 0 case
    x_zero_slope = -b_sol / (2 * a_sol);
    if x_zero_slope > 0
        fprintf('✗ WARNING: dF/dx = 0 at x = %.2f\n', x_zero_slope);
        fprintf('  Function is not strictly increasing for all x > 0\n');
    else
        fprintf('✓ dF/dx > 0 for all x > 0 (zero at x = %.2f < 0)\n', x_zero_slope);
    end
end

% Plot the function and its derivative
x_plot = linspace(0, 600, 1000);
F_plot = a_sol * x_plot.^2 + b_sol * x_plot;
dF_plot = 2 * a_sol * x_plot + b_sol;

figure;
subplot(2,1,1);
plot(x_plot, F_plot, 'b-', 'LineWidth', 2);
hold on;
plot(0, 0, 'ro', 'MarkerSize', 10, 'MarkerFaceColor', 'r');
plot(500, 0.316, 'ro', 'MarkerSize', 10, 'MarkerFaceColor', 'r');
yline(0, 'k--', 'Alpha', 0.3);
grid on;
xlabel('x');
ylabel('F(x)');
title(sprintf('F(x) = %.4e*x^2 + %.4e*x', a_sol, b_sol));
legend('F(x)', 'Constraints', 'Location', 'northwest');
hold off;

subplot(2,1,2);
plot(x_plot, dF_plot, 'r-', 'LineWidth', 2);
hold on;
plot(500, 0.003636, 'ro', 'MarkerSize', 10, 'MarkerFaceColor', 'r');
yline(0, 'k--', 'Alpha', 0.3);
grid on;
xlabel('x');
ylabel('dF/dx');
title(sprintf('dF/dx = %.4e*x + %.4e', 2*a_sol, b_sol));
legend('dF/dx', 'Constraint at x=500', 'Location', 'northwest');
hold off;