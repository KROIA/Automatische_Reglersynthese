%% Example: Using looptune to tune a feedback controller
clear; clc; 
% Requires: Robust Control Toolbox

% Define plant G(s) = 1 / (s^2 + 3s + 2)
s = tf('s');
%% Motor Parameters
R = 0.5;        % Armature resistance (Ohms)
L = 0.5;        % Armature inductance (H)
Kt = 0.01;      % Torque constant (Nm/A)
Ke = 0.01;      % Back-EMF constant (V·s/rad)
J = 0.01;       % Moment of inertia (kg·m²)
b = 0.01;        % Viscous friction coefficient (N·m·s)

plantSys = tf(Kt, [L*J, (R*J + L*b), (R*b + Kt*Ke)]);
%G = 1 / (s^2 + 3*s + 2);
G = plantSys;
G.InputName  = 'u';
G.OutputName = 'y';

% Create tunable controller structure (PI controller)
C0 = tunablePID('C','PI');
C0.InputName  = 'e';
C0.OutputName = 'u';

% Create an open-loop model with controller and plant
% 'sum' is used to define the feedback connection
% The plant input is u, and output is y.
% Controller input is the error (r - y)
sum1 = sumblk('e = r - y');
C0 = connect(C0, sum1, {'r', 'y'}, {'u'});




%% Create tuning goal: Step response with max 20% overshoot
Req = TuningGoal.StepTracking('r', 'y', 1);

% Requirement 2: Overshoot - limit overshoot to 10%
TR2 = TuningGoal.Overshoot('r', 'y', 1);
disp('2. Max overshoot: 10%');

% Perform loop shaping using looptune
wc = [1 20];
[~,C_tuned,~,Info] = looptune(G, C0, wc, TR2);

% Display tuned controller
C_tuned = -C_tuned;
disp('Tuned PI controller:')
C_tuned

% Analyze results
figure(1);
C_noRInput = C_tuned(:,'y');
bode(G, 'b', feedback(G*C_noRInput, 1), 'r--');
legend('Open-loop G(s)', 'Closed-loop T(s)');
title('Bode Plot: Open-loop and Closed-loop Response');

figure(2);
step(feedback(G*C_noRInput, 1));
title('Step Response with Tuned Controller');


% View detailed looptune results
figure(3);
loopview(G, C_tuned);

