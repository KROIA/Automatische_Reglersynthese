
close all;
recordedSignals1 = loadCSVAsTimeseries('SystemIdentifikation/simoutData1_filtered.csv');
recordedSignals2 = loadCSVAsTimeseries('SystemIdentifikation/simoutData2_filtered.csv');
recordedSignals3 = loadCSVAsTimeseries('SystemIdentifikation/simoutData3_filtered.csv');
recordedSignals4 = loadCSVAsTimeseries('SystemIdentifikation/simoutData4_filtered.csv');
recordedSignals5 = loadCSVAsTimeseries('SystemIdentifikation/simoutData5_filtered.csv');
recordedSignals6 = loadCSVAsTimeseries('SystemIdentifikation/simoutData6_filtered.csv');

recordedRampSignals = loadCSVAsTimeseries('SystemIdentifikation/ramp.csv');



% Specify custom model order
%[sys, fit] = identify_transfer_function('SystemIdentifikation/simoutData4_filtered.csv', 'NumPoles', 4, 'NumZeros', 3);
% Basic usage
%[sys_mimo, sys_siso, fit] = identify_mimo_transfer_function('SystemIdentifikation/simoutData4_simple2.csv', 'NumPoles', 5, 'NumZeros', 2);
% Access individual transfer functions
%G11 = sys_siso.u1_y1;  % u1 -> y1
%G12 = sys_siso.u2_y1;  % u2 -> y1
%G21 = sys_siso.u1_y2;  % u1 -> y2
%G22 = sys_siso.u2_y2;  % u2 -> y2


%[sys_mimo, sys_siso, fit_percent] = identify_mimo_SS('SystemIdentifikation/simoutData4_simple2.csv', 'NumPoles', 5, 'NumZeros', 2);









function tsData = loadCSVAsTimeseries(csvPath)
    % loadCSVAsTimeseries - Load CSV file as timeseries for Simulink
    %
    % Syntax: tsData = loadCSVAsTimeseries(csvPath)
    %
    % Input:
    %   csvPath - Path to CSV file (first column = time, rest = signals)
    %
    % Output:
    %   tsData - Timeseries object that can be used in Simulink
    %
    % Example:
    %   mySignal = loadCSVAsTimeseries('data.csv');
    %   % Then use 'From Workspace' block in Simulink with variable 'mySignal'
    
    % Read the CSV file
    data = readtable(csvPath);
    
    % Extract time column (first column)
    timeVector = data{:, 1};
    
    % Extract signal data (all other columns)
    signalData = data{:, 2:end};
    
    % Get signal names from column headers
    signalNames = data.Properties.VariableNames(2:end);
    
    % Create timeseries object
    tsData = timeseries(signalData, timeVector);
    
    % Set time units (adjust as needed)
    tsData.TimeInfo.Units = 'seconds';
    
    % Set data info
    tsData.Name = 'CSV_Signal_Data';
    
    % Display information
    fprintf('Timeseries created successfully!\n');
    fprintf('Time range: %.3f to %.3f %s\n', timeVector(1), timeVector(end), tsData.TimeInfo.Units);
    fprintf('Number of signals: %d\n', size(signalData, 2));
    fprintf('Signal names: %s\n', strjoin(signalNames, ', '));
    fprintf('Total samples: %d\n', length(timeVector));
    fprintf('\nTo use in Simulink:\n');
    fprintf('1. Use "From Workspace" block\n');
    fprintf('2. Set the variable name to the output of this function\n');
    fprintf('3. For multiple signals, outputs will be in columns\n');
end

function [sys_mimo, sys_siso, fit_percent] = identify_mimo_transfer_function(csv_path, varargin)
% IDENTIFY_MIMO_TRANSFER_FUNCTION Identifies MIMO transfer functions from CSV data
%
% Inputs:
%   csv_path - Path to CSV file with columns: [timestamp, u1, u2, y1, y2]
%   varargin - Optional name-value pairs:
%              'NumPoles', n - Number of poles (default: 2)
%              'NumZeros', m - Number of zeros (default: 1)
%              'PlotResults', true/false - Show plots (default: true)
%
% Outputs:
%   sys_mimo - 2x2 MIMO transfer function matrix
%   sys_siso - Structure with fields: u1_y1, u2_y1, u1_y2, u2_y2
%   fit_percent - Structure with fit percentages for each transfer function
%
% Example:
%   [sys_mimo, sys_siso, fit] = identify_mimo_transfer_function('data.csv');
%   [sys_mimo, sys_siso, fit] = identify_mimo_transfer_function('data.csv', 'NumPoles', 3);

    % Parse optional inputs
    p = inputParser;
    addParameter(p, 'NumPoles', 2, @isnumeric);
    addParameter(p, 'NumZeros', 1, @isnumeric);
    addParameter(p, 'PlotResults', true, @islogical);
    addParameter(p, 'SystemDefinitionMatlabScript', 'identified_transfer_functions.m', @string);
    parse(p, varargin{:});
    
    np = p.Results.NumPoles;
    nz = p.Results.NumZeros;
    plot_results = p.Results.PlotResults;
    systemDefinitionMatlabScript = p.Results.SystemDefinitionMatlabScript;
    
    % Read CSV file
    data = readmatrix(csv_path);
    t = data(:, 1);
    u1 = data(:, 2);
    u2 = data(:, 3);
    y1 = data(:, 4);
    y2 = data(:, 5);
    
    % Calculate sampling time
    dt = mean(diff(t));
    
    % Create MIMO iddata object
    U = [u1, u2];
    Y = [y1, y2];
    data_obj = iddata(Y, U, dt);
    
    % Split data into estimation and validation (70/30 split)
    n_total = length(y1);
    n_est = floor(0.7 * n_total);
    
    data_est = data_obj(1:n_est);
    data_val = data_obj(n_est+1:end);
    
    % Initialize structures
    sys_siso = struct();
    fit_percent = struct();
    
    fprintf('\n=== MIMO Transfer Function Identification ===\n');
    fprintf('Number of poles per TF: %d\n', np);
    fprintf('Number of zeros per TF: %d\n', nz);
    fprintf('Sampling time: %.4f s\n\n', dt);
    
    % Identify individual SISO transfer functions
    % TF 1: u1 -> y1
    fprintf('Identifying u1 -> y1...\n');
    data_u1y1_est = iddata(data_est.y(:,1), data_est.u(:,1), dt);
    data_u1y1_val = iddata(data_val.y(:,1), data_val.u(:,1), dt);
    sys_siso.u1_y1 = tfest(data_u1y1_est, np, nz);
    y_pred = lsim(sys_siso.u1_y1, data_u1y1_val.u, data_u1y1_val.SamplingInstants);
    fit_percent.u1_y1 = 100 * (1 - norm(data_u1y1_val.y - y_pred) / norm(data_u1y1_val.y - mean(data_u1y1_val.y)));
    fprintf('  Fit: %.2f%%\n', fit_percent.u1_y1);
    
    % TF 2: u2 -> y1
    fprintf('Identifying u2 -> y1...\n');
    data_u2y1_est = iddata(data_est.y(:,1), data_est.u(:,2), dt);
    data_u2y1_val = iddata(data_val.y(:,1), data_val.u(:,2), dt);
    sys_siso.u2_y1 = tfest(data_u2y1_est, np, nz);
    y_pred = lsim(sys_siso.u2_y1, data_u2y1_val.u, data_u2y1_val.SamplingInstants);
    fit_percent.u2_y1 = 100 * (1 - norm(data_u2y1_val.y - y_pred) / norm(data_u2y1_val.y - mean(data_u2y1_val.y)));
    fprintf('  Fit: %.2f%%\n', fit_percent.u2_y1);
    
    % TF 3: u1 -> y2
    fprintf('Identifying u1 -> y2...\n');
    data_u1y2_est = iddata(data_est.y(:,2), data_est.u(:,1), dt);
    data_u1y2_val = iddata(data_val.y(:,2), data_val.u(:,1), dt);
    sys_siso.u1_y2 = tfest(data_u1y2_est, np, nz);
    y_pred = lsim(sys_siso.u1_y2, data_u1y2_val.u, data_u1y2_val.SamplingInstants);
    fit_percent.u1_y2 = 100 * (1 - norm(data_u1y2_val.y - y_pred) / norm(data_u1y2_val.y - mean(data_u1y2_val.y)));
    fprintf('  Fit: %.2f%%\n', fit_percent.u1_y2);
    
    % TF 4: u2 -> y2
    fprintf('Identifying u2 -> y2...\n');
    data_u2y2_est = iddata(data_est.y(:,2), data_est.u(:,2), dt);
    data_u2y2_val = iddata(data_val.y(:,2), data_val.u(:,2), dt);
    sys_siso.u2_y2 = tfest(data_u2y2_est, np, nz);
    y_pred = lsim(sys_siso.u2_y2, data_u2y2_val.u, data_u2y2_val.SamplingInstants);
    fit_percent.u2_y2 = 100 * (1 - norm(data_u2y2_val.y - y_pred) / norm(data_u2y2_val.y - mean(data_u2y2_val.y)));
    fprintf('  Fit: %.2f%%\n\n', fit_percent.u2_y2);
    
    % Construct MIMO transfer function matrix
    % sys_mimo = [G11, G12;
    %             G21, G22]
    % where Gij is the transfer function from uj to yi
    % Convert to tf objects without initial conditions to avoid concatenation issues
    tf1 = tf(sys_siso.u1_y1);
    tf2 = tf(sys_siso.u2_y1);
    tf3 = tf(sys_siso.u1_y2);
    tf4 = tf(sys_siso.u2_y2);
    
    sys_mimo = [tf1, tf2;
                tf3, tf4];
    
    % Display transfer functions
    fprintf('=== Individual Transfer Functions ===\n\n');
    fprintf('u1 -> y1:\n');
    disp(sys_siso.u1_y1);
    fprintf('\nu2 -> y1:\n');
    disp(sys_siso.u2_y1);
    fprintf('\nu1 -> y2:\n');
    disp(sys_siso.u1_y2);
    fprintf('\nu2 -> y2:\n');
    disp(sys_siso.u2_y2);
    
    % Plot results if requested
    if plot_results
        % Figure 1: Time domain comparisons for all 4 transfer functions
        figure('Position', [50 50 1400 900]);
        
        % u1 -> y1
        subplot(2, 2, 1);
        compare(data_u1y1_val, sys_siso.u1_y1);
        title(sprintf('u1 → y1 (Fit: %.2f%%)', fit_percent.u1_y1));
        grid on;
        
        % u2 -> y1
        subplot(2, 2, 2);
        compare(data_u2y1_val, sys_siso.u2_y1);
        title(sprintf('u2 → y1 (Fit: %.2f%%)', fit_percent.u2_y1));
        grid on;
        
        % u1 -> y2
        subplot(2, 2, 3);
        compare(data_u1y2_val, sys_siso.u1_y2);
        title(sprintf('u1 → y2 (Fit: %.2f%%)', fit_percent.u1_y2));
        grid on;
        
        % u2 -> y2
        subplot(2, 2, 4);
        compare(data_u2y2_val, sys_siso.u2_y2);
        title(sprintf('u2 → y2 (Fit: %.2f%%)', fit_percent.u2_y2));
        grid on;
        
        % Figure 2: Bode plots for all transfer functions
        figure('Position', [100 100 1400 900]);
        
        subplot(2, 2, 1);
        bode(sys_siso.u1_y1);
        title('Bode: u1 → y1');
        grid on;
        
        subplot(2, 2, 2);
        bode(sys_siso.u2_y1);
        title('Bode: u2 → y1');
        grid on;
        
        subplot(2, 2, 3);
        bode(sys_siso.u1_y2);
        title('Bode: u1 → y2');
        grid on;
        
        subplot(2, 2, 4);
        bode(sys_siso.u2_y2);
        title('Bode: u2 → y2');
        grid on;
        
        % Figure 3: Step responses
        figure('Position', [150 150 1400 900]);
        
        subplot(2, 2, 1);
        step(sys_siso.u1_y1);
        title('Step Response: u1 → y1');
        grid on;
        
        subplot(2, 2, 2);
        step(sys_siso.u2_y1);
        title('Step Response: u2 → y1');
        grid on;
        
        subplot(2, 2, 3);
        step(sys_siso.u1_y2);
        title('Step Response: u1 → y2');
        grid on;
        
        subplot(2, 2, 4);
        step(sys_siso.u2_y2);
        title('Step Response: u2 → y2');
        grid on;
        
        % Figure 4: MIMO validation - simulate and plot manually
        figure('Position', [200 200 1200 600]);
        
        % Simulate MIMO system
        t_val = data_val.SamplingInstants;
        u_val = data_val.u;
        y_val = data_val.y;
        
        % Simulate each output
        y1_sim = lsim(tf1, u_val(:,1), t_val) + lsim(tf2, u_val(:,2), t_val);
        y2_sim = lsim(tf3, u_val(:,1), t_val) + lsim(tf4, u_val(:,2), t_val);
        
        % Plot y1
        subplot(2, 1, 1);
        plot(t_val, y_val(:,1), 'b-', 'LineWidth', 1.5);
        hold on;
        plot(t_val, y1_sim, 'r--', 'LineWidth', 1.5);
        hold off;
        legend('Measured y1', 'Simulated y1');
        xlabel('Time');
        ylabel('y1');
        title('MIMO Validation: Output y1');
        grid on;
        
        % Plot y2
        subplot(2, 1, 2);
        plot(t_val, y_val(:,2), 'b-', 'LineWidth', 1.5);
        hold on;
        plot(t_val, y2_sim, 'r--', 'LineWidth', 1.5);
        hold off;
        legend('Measured y2', 'Simulated y2');
        xlabel('Time');
        ylabel('y2');
        title('MIMO Validation: Output y2');
        grid on;
        
        sgtitle('MIMO System Validation');
    end
    
    % Summary statistics
    fprintf('\n=== Summary Statistics ===\n');
    fprintf('Average fit: %.2f%%\n', mean([fit_percent.u1_y1, fit_percent.u2_y1, fit_percent.u1_y2, fit_percent.u2_y2]));
    fprintf('Best fit: %.2f%% ', max([fit_percent.u1_y1, fit_percent.u2_y1, fit_percent.u1_y2, fit_percent.u2_y2]));
    [~, idx] = max([fit_percent.u1_y1, fit_percent.u2_y1, fit_percent.u1_y2, fit_percent.u2_y2]);
    tf_names = {'u1→y1', 'u2→y1', 'u1→y2', 'u2→y2'};
    fprintf('(%s)\n', tf_names{idx});
    fprintf('Worst fit: %.2f%% ', min([fit_percent.u1_y1, fit_percent.u2_y1, fit_percent.u1_y2, fit_percent.u2_y2]));
    [~, idx] = min([fit_percent.u1_y1, fit_percent.u2_y1, fit_percent.u1_y2, fit_percent.u2_y2]);
    fprintf('(%s)\n', tf_names{idx});
    
    % Create MATLAB script with transfer functions
    script_name = systemDefinitionMatlabScript;
    fid = fopen(script_name, 'w');
    
    fprintf(fid, '%% Identified Transfer Functions\n');
    fprintf(fid, '%% Generated on: %s\n', datetime('now'));
    fprintf(fid, '%% Source data: %s\n\n', csv_path);
    
    fprintf(fid, '%% Model parameters:\n');
    fprintf(fid, '%% Number of poles: %d\n', np);
    fprintf(fid, '%% Number of zeros: %d\n\n', nz);
    
    fprintf(fid, '%% Fit quality (validation data):\n');
    fprintf(fid, '%% u1->y1: %.2f%%\n', fit_percent.u1_y1);
    fprintf(fid, '%% u2->y1: %.2f%%\n', fit_percent.u2_y1);
    fprintf(fid, '%% u1->y2: %.2f%%\n', fit_percent.u1_y2);
    fprintf(fid, '%% u2->y2: %.2f%%\n\n', fit_percent.u2_y2);
    
    fprintf(fid, '%% Define transfer functions\n');
    fprintf(fid, 's = tf(''s'');\n\n');
    
    % Write G11 (u1 -> y1)
    [num, den] = tfdata(tf1, 'v');
    fprintf(fid, '%% G11: u1 -> y1\n');
    fprintf(fid, 'G11_num = %s;\n', mat2str(num, 6));
    fprintf(fid, 'G11_den = %s;\n', mat2str(den, 6));
    fprintf(fid, 'G11 = tf(G11_num, G11_den);\n\n');
    
    % Write G12 (u2 -> y1)
    [num, den] = tfdata(tf2, 'v');
    fprintf(fid, '%% G12: u2 -> y1\n');
    fprintf(fid, 'G12_num = %s;\n', mat2str(num, 6));
    fprintf(fid, 'G12_den = %s;\n', mat2str(den, 6));
    fprintf(fid, 'G12 = tf(G12_num, G12_den);\n\n');
    
    % Write G21 (u1 -> y2)
    [num, den] = tfdata(tf3, 'v');
    fprintf(fid, '%% G21: u1 -> y2\n');
    fprintf(fid, 'G21_num = %s;\n', mat2str(num, 6));
    fprintf(fid, 'G21_den = %s;\n', mat2str(den, 6));
    fprintf(fid, 'G21 = tf(G21_num, G21_den);\n\n');
    
    % Write G22 (u2 -> y2)
    [num, den] = tfdata(tf4, 'v');
    fprintf(fid, '%% G22: u2 -> y2\n');
    fprintf(fid, 'G22_num = %s;\n', mat2str(num, 6));
    fprintf(fid, 'G22_den = %s;\n', mat2str(den, 6));
    fprintf(fid, 'G22 = tf(G22_num, G22_den);\n\n');
    
    % Write MIMO system
    fprintf(fid, '%% Construct MIMO system matrix\n');
    fprintf(fid, '%% sys_mimo = [G11, G12;\n');
    fprintf(fid, '%%             G21, G22]\n');
    fprintf(fid, 'sys_mimo = [G11, G12; G21, G22];\n\n');
    
    fprintf(fid, '%% Display transfer functions\n');
    fprintf(fid, 'fprintf(''\\nG11 (u1 -> y1):\\n'');\n');
    fprintf(fid, 'G11\n\n');
    fprintf(fid, 'fprintf(''\\nG12 (u2 -> y1):\\n'');\n');
    fprintf(fid, 'G12\n\n');
    fprintf(fid, 'fprintf(''\\nG21 (u1 -> y2):\\n'');\n');
    fprintf(fid, 'G21\n\n');
    fprintf(fid, 'fprintf(''\\nG22 (u2 -> y2):\\n'');\n');
    fprintf(fid, 'G22\n\n');
    fprintf(fid, 'fprintf(''\\nMIMO System:\\n'');\n');
    fprintf(fid, 'sys_mimo\n');
    
    fclose(fid);
    
    fprintf('\n=== Transfer functions saved to: %s ===\n', script_name);
    
end


function [sys_mimo, sys_siso, fit_percent] = identify_mimo_SS(csv_path, varargin)
% IDENTIFY_MIMO_TRANSFER_FUNCTION Identifies MIMO transfer functions from CSV data
%
% Inputs:
%   csv_path - Path to CSV file with columns: [timestamp, u1, u2, y1, y2]
%   varargin - Optional name-value pairs:
%              'NumPoles', n - Number of poles (default: 2)
%              'NumZeros', m - Number of zeros (default: 1)
%              'PlotResults', true/false - Show plots (default: true)
%
% Outputs:
%   sys_mimo - 2x2 MIMO transfer function matrix
%   sys_siso - Structure with fields: u1_y1, u2_y1, u1_y2, u2_y2
%   fit_percent - Structure with fit percentages for each transfer function
%
% Example:
%   [sys_mimo, sys_siso, fit] = identify_mimo_transfer_function('data.csv');
%   [sys_mimo, sys_siso, fit] = identify_mimo_transfer_function('data.csv', 'NumPoles', 3);

    % Parse optional inputs
    p = inputParser;
    addParameter(p, 'NumPoles', 2, @isnumeric);
    addParameter(p, 'NumZeros', 1, @isnumeric);
    addParameter(p, 'PlotResults', true, @islogical);
    addParameter(p, 'SystemDefinitionMatlabScript', 'identified_transfer_functions.m', @string);
    parse(p, varargin{:});
    
    np = p.Results.NumPoles;
    nz = p.Results.NumZeros;
    plot_results = p.Results.PlotResults;
    systemDefinitionMatlabScript = p.Results.SystemDefinitionMatlabScript;
    
    % Read CSV file
    data = readmatrix(csv_path);
    t = data(:, 1);
    u1 = data(:, 2);
    u2 = data(:, 3);
    y1 = data(:, 4);
    y2 = data(:, 5);
    
    % Calculate sampling time
    dt = mean(diff(t));
    
    % Create MIMO iddata object
    U = [u1, u2];
    Y = [y1, y2];
    data_obj = iddata(Y, U, dt);
    
    % Split data into estimation and validation (70/30 split)
    n_total = length(y1);
    n_est = floor(0.7 * n_total);
    
    data_est = data_obj(1:n_est);
    data_val = data_obj(n_est+1:end);
    
    % Initialize structures
    sys_siso = struct();
    fit_percent = struct();
    
    fprintf('\n=== MIMO Transfer Function Identification ===\n');
    fprintf('Number of poles per TF: %d\n', np);
    fprintf('Number of zeros per TF: %d\n', nz);
    fprintf('Sampling time: %.4f s\n\n', dt);
    
    % Identify individual SISO transfer functions
    % TF 1: u1 -> y1
    fprintf('Identifying u1 -> y1...\n');
    data_u1y1_est = iddata(data_est.y(:,1), data_est.u(:,1), dt);
    data_u1y1_val = iddata(data_val.y(:,1), data_val.u(:,1), dt);
    sys_siso.u1_y1 = tfest(data_u1y1_est, np, nz);
    y_pred = lsim(sys_siso.u1_y1, data_u1y1_val.u, data_u1y1_val.SamplingInstants);
    fit_percent.u1_y1 = 100 * (1 - norm(data_u1y1_val.y - y_pred) / norm(data_u1y1_val.y - mean(data_u1y1_val.y)));
    fprintf('  Fit: %.2f%%\n', fit_percent.u1_y1);
    
    % TF 2: u2 -> y1
    fprintf('Identifying u2 -> y1...\n');
    data_u2y1_est = iddata(data_est.y(:,1), data_est.u(:,2), dt);
    data_u2y1_val = iddata(data_val.y(:,1), data_val.u(:,2), dt);
    sys_siso.u2_y1 = tfest(data_u2y1_est, np, nz);
    y_pred = lsim(sys_siso.u2_y1, data_u2y1_val.u, data_u2y1_val.SamplingInstants);
    fit_percent.u2_y1 = 100 * (1 - norm(data_u2y1_val.y - y_pred) / norm(data_u2y1_val.y - mean(data_u2y1_val.y)));
    fprintf('  Fit: %.2f%%\n', fit_percent.u2_y1);
    
    % TF 3: u1 -> y2
    fprintf('Identifying u1 -> y2...\n');
    data_u1y2_est = iddata(data_est.y(:,2), data_est.u(:,1), dt);
    data_u1y2_val = iddata(data_val.y(:,2), data_val.u(:,1), dt);
    sys_siso.u1_y2 = tfest(data_u1y2_est, np, nz);
    y_pred = lsim(sys_siso.u1_y2, data_u1y2_val.u, data_u1y2_val.SamplingInstants);
    fit_percent.u1_y2 = 100 * (1 - norm(data_u1y2_val.y - y_pred) / norm(data_u1y2_val.y - mean(data_u1y2_val.y)));
    fprintf('  Fit: %.2f%%\n', fit_percent.u1_y2);
    
    % TF 4: u2 -> y2
    fprintf('Identifying u2 -> y2...\n');
    data_u2y2_est = iddata(data_est.y(:,2), data_est.u(:,2), dt);
    data_u2y2_val = iddata(data_val.y(:,2), data_val.u(:,2), dt);
    sys_siso.u2_y2 = tfest(data_u2y2_est, np, nz);
    y_pred = lsim(sys_siso.u2_y2, data_u2y2_val.u, data_u2y2_val.SamplingInstants);
    fit_percent.u2_y2 = 100 * (1 - norm(data_u2y2_val.y - y_pred) / norm(data_u2y2_val.y - mean(data_u2y2_val.y)));
    fprintf('  Fit: %.2f%%\n\n', fit_percent.u2_y2);
    
    % Construct MIMO transfer function matrix
    % sys_mimo = [G11, G12;
    %             G21, G22]
    % where Gij is the transfer function from uj to yi
    % Convert to tf objects without initial conditions to avoid concatenation issues
    tf1 = tf(sys_siso.u1_y1);
    tf2 = tf(sys_siso.u2_y1);
    tf3 = tf(sys_siso.u1_y2);
    tf4 = tf(sys_siso.u2_y2);
    
    sys_mimo = [tf1, tf2;
                tf3, tf4];
    
    % Display transfer functions
    fprintf('=== Individual Transfer Functions ===\n\n');
    fprintf('u1 -> y1:\n');
    disp(sys_siso.u1_y1);
    fprintf('\nu2 -> y1:\n');
    disp(sys_siso.u2_y1);
    fprintf('\nu1 -> y2:\n');
    disp(sys_siso.u1_y2);
    fprintf('\nu2 -> y2:\n');
    disp(sys_siso.u2_y2);
    
    % Plot results if requested
    if plot_results
        % Figure 1: Time domain comparisons for all 4 transfer functions
        figure('Position', [50 50 1400 900]);
        
        % u1 -> y1
        subplot(2, 2, 1);
        compare(data_u1y1_val, sys_siso.u1_y1);
        title(sprintf('u1 → y1 (Fit: %.2f%%)', fit_percent.u1_y1));
        grid on;
        
        % u2 -> y1
        subplot(2, 2, 2);
        compare(data_u2y1_val, sys_siso.u2_y1);
        title(sprintf('u2 → y1 (Fit: %.2f%%)', fit_percent.u2_y1));
        grid on;
        
        % u1 -> y2
        subplot(2, 2, 3);
        compare(data_u1y2_val, sys_siso.u1_y2);
        title(sprintf('u1 → y2 (Fit: %.2f%%)', fit_percent.u1_y2));
        grid on;
        
        % u2 -> y2
        subplot(2, 2, 4);
        compare(data_u2y2_val, sys_siso.u2_y2);
        title(sprintf('u2 → y2 (Fit: %.2f%%)', fit_percent.u2_y2));
        grid on;
        
        % Figure 2: Bode plots for all transfer functions
        figure('Position', [100 100 1400 900]);
        
        subplot(2, 2, 1);
        bode(sys_siso.u1_y1);
        title('Bode: u1 → y1');
        grid on;
        
        subplot(2, 2, 2);
        bode(sys_siso.u2_y1);
        title('Bode: u2 → y1');
        grid on;
        
        subplot(2, 2, 3);
        bode(sys_siso.u1_y2);
        title('Bode: u1 → y2');
        grid on;
        
        subplot(2, 2, 4);
        bode(sys_siso.u2_y2);
        title('Bode: u2 → y2');
        grid on;
        
        % Figure 3: Step responses
        figure('Position', [150 150 1400 900]);
        
        subplot(2, 2, 1);
        step(sys_siso.u1_y1);
        title('Step Response: u1 → y1');
        grid on;
        
        subplot(2, 2, 2);
        step(sys_siso.u2_y1);
        title('Step Response: u2 → y1');
        grid on;
        
        subplot(2, 2, 3);
        step(sys_siso.u1_y2);
        title('Step Response: u1 → y2');
        grid on;
        
        subplot(2, 2, 4);
        step(sys_siso.u2_y2);
        title('Step Response: u2 → y2');
        grid on;
        
        % Figure 4: MIMO validation - simulate and plot manually
        figure('Position', [200 200 1200 600]);
        
        % Simulate MIMO system
        t_val = data_val.SamplingInstants;
        u_val = data_val.u;
        y_val = data_val.y;
        
        % Simulate each output
        y1_sim = lsim(tf1, u_val(:,1), t_val) + lsim(tf2, u_val(:,2), t_val);
        y2_sim = lsim(tf3, u_val(:,1), t_val) + lsim(tf4, u_val(:,2), t_val);
        
        % Plot y1
        subplot(2, 1, 1);
        plot(t_val, y_val(:,1), 'b-', 'LineWidth', 1.5);
        hold on;
        plot(t_val, y1_sim, 'r--', 'LineWidth', 1.5);
        hold off;
        legend('Measured y1', 'Simulated y1');
        xlabel('Time');
        ylabel('y1');
        title('MIMO Validation: Output y1');
        grid on;
        
        % Plot y2
        subplot(2, 1, 2);
        plot(t_val, y_val(:,2), 'b-', 'LineWidth', 1.5);
        hold on;
        plot(t_val, y2_sim, 'r--', 'LineWidth', 1.5);
        hold off;
        legend('Measured y2', 'Simulated y2');
        xlabel('Time');
        ylabel('y2');
        title('MIMO Validation: Output y2');
        grid on;
        
        sgtitle('MIMO System Validation');
    end
    
    % Summary statistics
    fprintf('\n=== Summary Statistics ===\n');
    fprintf('Average fit: %.2f%%\n', mean([fit_percent.u1_y1, fit_percent.u2_y1, fit_percent.u1_y2, fit_percent.u2_y2]));
    fprintf('Best fit: %.2f%% ', max([fit_percent.u1_y1, fit_percent.u2_y1, fit_percent.u1_y2, fit_percent.u2_y2]));
    [~, idx] = max([fit_percent.u1_y1, fit_percent.u2_y1, fit_percent.u1_y2, fit_percent.u2_y2]);
    tf_names = {'u1→y1', 'u2→y1', 'u1→y2', 'u2→y2'};
    fprintf('(%s)\n', tf_names{idx});
    fprintf('Worst fit: %.2f%% ', min([fit_percent.u1_y1, fit_percent.u2_y1, fit_percent.u1_y2, fit_percent.u2_y2]));
    [~, idx] = min([fit_percent.u1_y1, fit_percent.u2_y1, fit_percent.u1_y2, fit_percent.u2_y2]);
    fprintf('(%s)\n', tf_names{idx});
    
    % Create MATLAB script with state-space representation
    script_name = systemDefinitionMatlabScript;
    fid = fopen(script_name, 'w');
    
    fprintf(fid, '%% Identified MIMO State-Space System\n');
    fprintf(fid, '%% Generated on: %s\n', datetime('now'));
    fprintf(fid, '%% Source data: %s\n\n', csv_path);
    
    fprintf(fid, '%% Model parameters:\n');
    fprintf(fid, '%% Number of poles per TF: %d\n', np);
    fprintf(fid, '%% Number of zeros per TF: %d\n\n', nz);
    
    fprintf(fid, '%% Fit quality (validation data):\n');
    fprintf(fid, '%% u1->y1: %.2f%%\n', fit_percent.u1_y1);
    fprintf(fid, '%% u2->y1: %.2f%%\n', fit_percent.u2_y1);
    fprintf(fid, '%% u1->y2: %.2f%%\n', fit_percent.u1_y2);
    fprintf(fid, '%% u2->y2: %.2f%%\n\n', fit_percent.u2_y2);
    
    % Convert MIMO system to state-space
    ss_mimo = ss(sys_mimo);
    [A, B, C, D] = ssdata(ss_mimo);
    
    fprintf(fid, '%% State-Space Representation: dx/dt = A*x + B*u; y = C*x + D*u\n');
    fprintf(fid, '%% Number of states: %d\n', size(A, 1));
    fprintf(fid, '%% Number of inputs: 2 (u1, u2)\n');
    fprintf(fid, '%% Number of outputs: 2 (y1, y2)\n\n');
    
    % Write A matrix
    fprintf(fid, '%% State matrix A (%dx%d)\n', size(A, 1), size(A, 2));
    fprintf(fid, 'A = [\n');
    for i = 1:size(A, 1)
        fprintf(fid, '    ');
        for j = 1:size(A, 2)
            if j < size(A, 2)
                fprintf(fid, '%14.8e, ', A(i, j));
            else
                fprintf(fid, '%14.8e', A(i, j));
            end
        end
        if i < size(A, 1)
            fprintf(fid, ';\n');
        else
            fprintf(fid, '\n');
        end
    end
    fprintf(fid, '];\n\n');
    
    % Write B matrix
    fprintf(fid, '%% Input matrix B (%dx%d)\n', size(B, 1), size(B, 2));
    fprintf(fid, 'B = [\n');
    for i = 1:size(B, 1)
        fprintf(fid, '    ');
        for j = 1:size(B, 2)
            if j < size(B, 2)
                fprintf(fid, '%14.8e, ', B(i, j));
            else
                fprintf(fid, '%14.8e', B(i, j));
            end
        end
        if i < size(B, 1)
            fprintf(fid, ';\n');
        else
            fprintf(fid, '\n');
        end
    end
    fprintf(fid, '];\n\n');
    
    % Write C matrix
    fprintf(fid, '%% Output matrix C (%dx%d)\n', size(C, 1), size(C, 2));
    fprintf(fid, 'C = [\n');
    for i = 1:size(C, 1)
        fprintf(fid, '    ');
        for j = 1:size(C, 2)
            if j < size(C, 2)
                fprintf(fid, '%14.8e, ', C(i, j));
            else
                fprintf(fid, '%14.8e', C(i, j));
            end
        end
        if i < size(C, 1)
            fprintf(fid, ';\n');
        else
            fprintf(fid, '\n');
        end
    end
    fprintf(fid, '];\n\n');
    
    % Write D matrix
    fprintf(fid, '%% Feedthrough matrix D (%dx%d)\n', size(D, 1), size(D, 2));
    fprintf(fid, 'D = [\n');
    for i = 1:size(D, 1)
        fprintf(fid, '    ');
        for j = 1:size(D, 2)
            if j < size(D, 2)
                fprintf(fid, '%14.8e, ', D(i, j));
            else
                fprintf(fid, '%14.8e', D(i, j));
            end
        end
        if i < size(D, 1)
            fprintf(fid, ';\n');
        else
            fprintf(fid, '\n');
        end
    end
    fprintf(fid, '];\n\n');
    
    % Create state-space system
    fprintf(fid, '%% Create state-space system\n');
    fprintf(fid, 'sys_ss = ss(A, B, C, D);\n\n');
    
    % Add system properties
    fprintf(fid, '%% Set input and output names\n');
    fprintf(fid, 'sys_ss.InputName = {''u1'', ''u2''};\n');
    fprintf(fid, 'sys_ss.OutputName = {''y1'', ''y2''};\n\n');
    
    fprintf(fid, '%% Display state-space system\n');
    fprintf(fid, 'fprintf(''\\nState-Space System:\\n'');\n');
    fprintf(fid, 'sys_ss\n\n');
    
    % Add system analysis
    fprintf(fid, '%% System properties\n');
    fprintf(fid, 'fprintf(''\\nSystem Properties:\\n'');\n');
    fprintf(fid, 'fprintf(''Number of states: %%d\\n'', size(A, 1));\n');
    fprintf(fid, 'fprintf(''Eigenvalues (poles):\\n'');\n');
    fprintf(fid, 'disp(eig(A));\n\n');
    
    fprintf(fid, '%% Check stability\n');
    fprintf(fid, 'poles = eig(A);\n');
    fprintf(fid, 'if all(real(poles) < 0)\n');
    fprintf(fid, '    fprintf(''System is STABLE (all poles have negative real parts)\\n'');\n');
    fprintf(fid, 'else\n');
    fprintf(fid, '    fprintf(''System is UNSTABLE (some poles have positive real parts)\\n'');\n');
    fprintf(fid, 'end\n\n');
    
    % Add conversion to transfer function
    fprintf(fid, '%% Convert to transfer function representation\n');
    fprintf(fid, 'sys_tf = tf(sys_ss);\n');
    fprintf(fid, 'fprintf(''\\nTransfer Function Representation:\\n'');\n');
    fprintf(fid, 'sys_tf\n');
    
    fclose(fid);
    
    fprintf('\n=== State-space system saved to: %s ===\n', script_name);
    
end