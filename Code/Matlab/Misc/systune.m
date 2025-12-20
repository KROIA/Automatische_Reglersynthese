%% MATLAB systune Beginner Example
% This example shows how to tune a simple feedback controller using systune
% to meet rise time and overshoot specifications

clear; clc; 
%close all;

%% Step 1: Define the Plant Model
%% Motor Parameters
R = 0.5;        % Armature resistance (Ohms)
L = 0.5;        % Armature inductance (H)
Kt = 0.01;      % Torque constant (Nm/A)
Ke = 0.01;      % Back-EMF constant (V·s/rad)
J = 0.01;       % Moment of inertia (kg·m²)
b = 0.01;        % Viscous friction coefficient (N·m·s)

plantSys = tf(Kt, [L*J, (R*J + L*b), (R*b + Kt*Ke)]);

% We'll use a simple second-order system representing a DC motor
%J = 0.01;   % Inertia
%b = 0.01;   % Damping coefficient
%K = 0.05;   % Motor constant
%
% Transfer function: theta(s)/V(s)
s = tf('s');
%plantSys = K / (J*s^2 + b*s);
%plantSys = 1/(s-1);
% Name the plant inputs and outputs
plantSys.InputName = 'uIn';
plantSys.OutputName = 'y';
plantSys.Name = 'plantSys';

% Display plant
disp('Plant Transfer Function:');
disp(plantSys);

%% Step 2: Create a Tunable Controller
% We'll tune a simple PI controller: C(s) = Kp + Ki/s
controllerSysInitial = tunablePID('controllerSys', 'PID');
% Set initial values (optional)
controllerSysInitial.Kp.Value = 3;
controllerSysInitial.Ki.Value = 0.3;


%controllerSysInitial = tunableTF('controllerSys', 2,2);

% Name the controller inputs and outputs
controllerSysInitial.InputName = 'e';
controllerSysInitial.OutputName = 'uOut';




disp('Initial Controller:');
disp(controllerSysInitial);

%% Step 3: Connect Controller, Plant, AnalysisPoint and feedback together
% Create an analysis point for measuring margins
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
TR1 = TuningGoal.Tracking('r', 'y', 1);  
disp('1. Tracking bandwidth: 1 rad/s');

% Requirement 2: Overshoot - limit overshoot to 10%
TR2 = TuningGoal.Overshoot('r', 'y', 10);
disp('2. Max overshoot: 10%');

% Requirement 3: Limit rate of change (slew rate) to 0.1/second
% We do this by limiting the gain from reference to derivative of output
% Using a high-pass filter approximation: d/dt ≈ s at frequencies of interest
targetTransientSystem = 100/(s+100);
%[delayNum,delayDen] = pade(0.5,5)
%targetTransientSystem = 0.5*(tf(delayNum,delayDen) + 1/(s+1));
TR3 = TuningGoal.Transient('r', 'y', targetTransientSystem, 'step');  % Limit gain to 0.1 in frequency range
disp('3. Max rate of change: 0.1/second');

% Add margins requirement using the analysis point
TR4 = TuningGoal.Margins('uAP', 6, 45);
disp('4. Gain margin: 6 dB, Phase margin: 45°');

% Define the tuning goal
actuatorBound =  5;
TR5 = TuningGoal.Gain('r', 'uIn', actuatorBound);

targetSoftGoals = [TR1, TR2];
targetHardGoals = [TR5];







%% Step 5: Tune the Controller using systune
% Define the system to tune (open-loop interconnection)

% Perform tuning
[sysTuned, fSoft, fHard] = systune(sys0, targetSoftGoals, targetHardGoals);
sysTuned.Name = 'Tuned system';

% Extract tuned controller
controllerSysTuned = getBlockValue(sysTuned, 'controllerSys');

% Extract model data for usage in simulink model: Experiment_TF.slx
C_NUM = tf(controllerSysTuned).Numerator{1};
C_DEN = tf(controllerSysTuned).Denominator{1};
G = ss(plantSys);
Cdim = size(G.C);
GX0 = zeros(Cdim(2), 1);

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


if isempty(targetHardGoals)==0
    figure(3);
    viewGoal(targetHardGoals, sysTuned);
end