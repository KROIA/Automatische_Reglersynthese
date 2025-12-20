%% MATLAB systune DC-Motor Example
% This example shows how to tune a PID-Controller for a DC-Motor using systune.
addpath('../Utilities');
addpath('../Utilities/NyquistLog');

%clear; clc; 
PlantsInit; % Create motor systems

%% Step 1: Define the Plant Model
%% Transfer function for the DC-Motor
plantSys = simpleMotorSystem;
plantSys.InputName = 'uIn';

% Display plant
disp('Plant Transfer Function:');
disp(plantSys);

%% Step 2: Create a Tunable Controller
% We'll tune a PID controller
controllerSysInitial = tunablePID('controllerSys', 'PID');
% Set initial values (optional)
controllerSysInitial.Kp.Value = 1;
controllerSysInitial.Ki.Value = 1;

% Name the controller inputs and outputs
controllerSysInitial.InputName = 'e';
controllerSysInitial.OutputName = 'uOut';


disp('Initial Controller:');
disp(controllerSysInitial);

%% Step 3: Connect Controller, Plant, AnalysisPoint and feedback together
% Create an analysis point for usage in the tuning process
G_ap = AnalysisPoint('uAP');
G_ap.InputName = 'uOut';
G_ap.OutputName = 'uIn';

% Create summing junction
sum_block = sumblk('e = r - y');

% Connect the blocks with analysis point
sys0 = connect(plantSys, G_ap, controllerSysInitial, sum_block, {'r'}, {'y', 'uIn', 'e'});
sys0.Name = 'Untuned system';

%% Step 4: Define Tuning Requirements
disp('Tuning Requirements:');

% Requirement 1: Tracking - step response should track reference
TR1 = TuningGoal.StepTracking('r', 'y', 0.014);  
disp('1. StepTracking');

% Requirement 2: Overshoot - limit overshoot to 10%
TR2 = TuningGoal.Overshoot('r', 'y', 10);
disp('2. Max overshoot: 10%');

% Add margins requirement using the analysis point
%TR4 = TuningGoal.Margins('uAP', 6, 45);
%disp('4. Gain margin: 6 dB, Phase margin: 45°');

% Define the tuning goal
actuatorInputBound =  10;
TR5 = TuningGoal.Gain('r', 'uIn', actuatorInputBound);

%TR6 = TuningGoal.Variance('r','uIn', 10);

targetSoftGoals = [TR1];
targetHardGoals = [TR2];







%% Step 5: Tune the Controller using systune
% Perform tuning
[sysTuned, fSoft, fHard] = systune(sys0, targetSoftGoals, targetHardGoals);
sysTuned.Name = 'Tuned system';

% Extract tuned controller
controllerSysTuned = getBlockValue(sysTuned, 'controllerSys');
systunePIDc = controllerSysTuned;
systunePIDc.Name = 'SystunePID';

% Discretize the pid
systunePIDd = c2d(systunePIDc, 0.01, 'forward');


disp(' ');
disp('Tuned Controller:');
disp(controllerSysTuned);

% Display final objective value
fprintf('Final soft objective: %.4f (< 1 is good)\n', fSoft);
fprintf('Final hard objective: %.4f (< 1 is good)\n', fHard);

%% Step 6: Compare Results
% Create closed-loop systems
CL_initial = feedback(plantSys*controllerSysInitial, 1);
CL_tuned = feedback(plantSys*controllerSysTuned, 1);

% Plot step responses
figure(1);

subplot(2,2,1);
step(feedback(plantSys, 1), CL_initial, CL_tuned, 10);
title('Closed-loop Step Response Comparison');
legend('No controller','Initial Controller', 'Tuned Controller');
grid on;

subplot(2,2,2);
%bode(plantSys, plantSys*controllerSysInitial, plantSys*controllerSysTuned);
hold on;
margin(plantSys);
margin(plantSys*controllerSysInitial);
margin(plantSys*controllerSysTuned);
title('Open-loop Transfer Function');
legend('No controller', 'Initial', 'Tuned');
grid on;
hold off;

subplot(2,2,3);
nyquist(plantSys, plantSys*controllerSysInitial, plantSys*controllerSysTuned);
hold on;
% Plot the unit circle
theta = linspace(0, 2*pi, 100);
plot(cos(theta), sin(theta), 'r--', 'LineWidth', 1.5);
title('Nyquist comparison')
legend('No controller (plantSys)', 'Initial (plantSys*controllerSysInitial)', 'Tuned (plantSys*controllerSysTuned)');


subplot(2,2,4);
pzmap(feedback(plantSys,1), feedback(plantSys*controllerSysInitial,1), feedback(plantSys*controllerSysTuned,1));
title('Pole & Zeros');
legend('No controller feedback(plantSys)', 'Initial feedback(plantSys*controllerSysInitial)', 'Tuned feedback(plantSys*controllerSysTuned)');

%% Step 7: Analyze Performance
% Get step response info
info_initial = stepinfo(CL_initial);
info_tuned = stepinfo(CL_tuned);

fprintf('\n=== Performance Comparison ===\n');
fprintf('                  Initial    Tuned\n');
fprintf('Rise Time:        %.3f s    %.3f s\n', ...
    info_initial.RiseTime, info_tuned.RiseTime);
fprintf('Settling Time:    %.3f s    %.3f s\n', ...
    info_initial.SettlingTime, info_tuned.SettlingTime);
fprintf('Overshoot:        %.2f %%    %.2f %%\n', ...
    info_initial.Overshoot, info_tuned.Overshoot);

% Check margins
[Gm_i, Pm_i] = margin(plantSys*controllerSysInitial);
[Gm_t, Pm_t] = margin(plantSys*controllerSysTuned);

fprintf('\nGain Margin:      %.2f dB   %.2f dB\n', 20*log10(Gm_i), 20*log10(Gm_t));
fprintf('Phase Margin:     %.2f °    %.2f °\n', Pm_i, Pm_t);

%% Additional Visualization: View Requirements
figure(2);
viewGoal(targetSoftGoals, sysTuned);
if ~exist('tuningGoalPlotWidthRatio', 'var')
    tuningGoalPlotWidthRatio = 1;
end

if exist('simpleMotorImagesOutputPath','var') & endsWith(pwd, ['\Code\Matlab'], 'IgnoreCase', true)
    fig = figure(4);
    for i = 1:length(targetSoftGoals)
        viewGoal(targetSoftGoals(i), sysTuned);
        % ajust aspect ratio
        fig.Position(3) = tuningGoalPlotWidthRatio * fig.Position(4);
        figureToPDF(fig, [simpleMotorImagesOutputPath '/SystuneTuningGoalSoft_' char(string(i)) '.pdf']);
    end
end

if isempty(targetHardGoals)==0
    figure(3);
    viewGoal(targetHardGoals, sysTuned);

    if exist('simpleMotorImagesOutputPath','var') & endsWith(pwd, ['\Code\Matlab'], 'IgnoreCase', true)
        fig = figure(4);
        for i = 1:length(targetHardGoals)
            viewGoal(targetHardGoals(i), sysTuned);
            fig.Position(3) = tuningGoalPlotWidthRatio * fig.Position(4);
            figureToPDF(fig, [simpleMotorImagesOutputPath '/SystuneTuningGoalHard_' char(string(i)) '.pdf']);
        end
    end
end

%writetable(table(out.simout),'test.csv','Delimiter',';');
%motorNoiseData = load('noise_filter.mat');