%% MATLAB systune DC-Motor Example
% This example shows how to tune a PID-Controller for a DC-Motor using systune.
addpath('../Utilities');
addpath('../Utilities/NyquistLog');

%clear; clc; 
close all;
PlantsInit; % Create motor systems

%% Step 1: Define the Plant Model
plantSys = motorMitSchwungmasseStateSpace; % Ist im "MotorInit.m" zu finden

%% Step 2: Create a Tunable Controller
% We'll tune a PID controller
controllerSysInitial = tunablePID('controllerSys', 'PID');
% Set initial values (optional)
controllerSysInitial.Kp.Value = -1;
controllerSysInitial.Ki.Value = -1;

% Name the controller inputs and outputs
controllerSysInitial.InputName = 'e';
controllerSysInitial.OutputName = 'u1';


disp('Initial Controller:');
disp(controllerSysInitial);

%% Step 3: Connect Controller, Plant, AnalysisPoint and feedback together
% Create an analysis point for usage in the tuning processstepFinalValue
G_ap = AnalysisPoint('uAP');
G_ap.InputName = 'y2';
G_ap.OutputName = 'y2out';

%G_ap2 = AnalysisPoint('uAP2');
%G_ap2.InputName = 'uOut';
%G_ap2.OutputName = 'u1';

% Create summing junction
sum_block = sumblk('e = r - y2out');

% Connect the blocks with analysis point
sys0 = connect(plantSys, G_ap, controllerSysInitial, sum_block, {'r'}, {'y2', 'u1', 'e'});
sys0.Name = 'Untuned system';

%% Step 4: Define Tuning Requirements
disp('Tuning Requirements:');

% Requirement 1: Tracking - step response should track reference
pt2T = 0.2;
pt2D  = 1;
pt2Step = tf([1],[pt2T*pt2T 2*pt2D*pt2T 1]);
TR1 = TuningGoal.StepTracking('r', 'y2', pt2Step);  
disp('1. StepTracking');

% Requirement 2: Overshoot - limit overshoot to 10%
TR2 = TuningGoal.Overshoot('r', 'y2', 10);
disp('2. Max overshoot: 10%');

% Add margins requirement using the analysis point
TR4 = TuningGoal.Margins('uAP', 6, 45);
disp('4. Gain margin: 6 dB, Phase margin: 45°');

% Define the tuning goal
actuatorInputBound =  10;
TR5 = TuningGoal.Gain('r', 'u1', actuatorInputBound);

%TR6 = TuningGoal.Variance('r','u1', 10);

targetSoftGoals = [TR1  TR2];
targetHardGoals = [TR4];







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
plantSysInp1ToOut2 = plantSys(2,1);
CL_initial = feedback(plantSysInp1ToOut2*controllerSysInitial, 1);
CL_tuned = feedback(plantSysInp1ToOut2*controllerSysTuned, 1);

% Plot step responses
figure(1);

subplot(2,2,1);
step(feedback(plantSysInp1ToOut2, 1), CL_initial, CL_tuned, 10);
title('Closed-loop Step Response Comparison');
legend('No controller','Initial Controller', 'Tuned Controller');
grid on;

subplot(2,2,2);
%bode(plantSys, plantSys*controllerSysInitial, plantSys*controllerSysTuned);
hold on;
margin(plantSysInp1ToOut2);
margin(plantSysInp1ToOut2*controllerSysInitial);
margin(plantSysInp1ToOut2*controllerSysTuned);
title('Open-loop Transfer Function');
legend('No controller', 'Initial', 'Tuned');
grid on;
hold off;

subplot(2,2,3);
nyquist(plantSysInp1ToOut2, plantSysInp1ToOut2*controllerSysInitial, plantSysInp1ToOut2*controllerSysTuned);
hold on;
% Plot the unit circle
theta = linspace(0, 2*pi, 100);
plot(cos(theta), sin(theta), 'r--', 'LineWidth', 1.5);
title('Nyquist comparison')
legend('No controller (plantSys)', 'Initial (plantSys*controllerSysInitial)', 'Tuned (plantSys*controllerSysTuned)');


subplot(2,2,4);
pzmap(feedback(plantSysInp1ToOut2,1), feedback(plantSysInp1ToOut2*controllerSysInitial,1), feedback(plantSysInp1ToOut2*controllerSysTuned,1));
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
[Gm_i, Pm_i] = margin(plantSysInp1ToOut2*controllerSysInitial);
[Gm_t, Pm_t] = margin(plantSysInp1ToOut2*controllerSysTuned);

fprintf('\nGain Margin:      %.2f dB   %.2f dB\n', 20*log10(Gm_i), 20*log10(Gm_t));
fprintf('Phase Margin:     %.2f °    %.2f °\n', Pm_i, Pm_t);

%% Additional Visualization: View Requirements
figure(2);
viewGoal(targetSoftGoals, sysTuned);
if ~exist('tuningGoalPlotWidthRatio', 'var')
    tuningGoalPlotWidthRatio = 1;
end
if exist('motorMitSchwungmasseImagesOutputPath','var') & endsWith(pwd, ['\Code\Matlab'], 'IgnoreCase', true)
    fig = figure(4);
    for i = 1:length(targetSoftGoals)
        viewGoal(targetSoftGoals(i), sysTuned);
        fig.Position(3) = tuningGoalPlotWidthRatio * fig.Position(4);
        figureToPDF(fig, [motorMitSchwungmasseImagesOutputPath '/SystuneTuningGoalSoft_' char(string(i)) '.pdf']);
    end
end

if isempty(targetHardGoals)==0
    figure(3);
    viewGoal(targetHardGoals, sysTuned);

    if exist('motorMitSchwungmasseImagesOutputPath','var') & endsWith(pwd, ['\Code\Matlab'], 'IgnoreCase', true)
        fig = figure(4);
        for i = 1:length(targetHardGoals)
            viewGoal(targetHardGoals(i), sysTuned);
            fig.Position(3) = tuningGoalPlotWidthRatio * fig.Position(4);
            figureToPDF(fig, [motorMitSchwungmasseImagesOutputPath '/SystuneTuningGoalHard_' char(string(i)) '.pdf']);
        end
    end
end

NyquistLogArray(tf(controllerSysTuned * plantSysInp1ToOut2))
%writetable(table(out.simout),'test.csv','Delimiter',';');
%motorNoiseData = load('noise_filter.mat');