function sys = ballOnBeam()
%% Ball-on-Beam (Kugel auf Wippe) State-Space Representation
% This script defines the state-space model for a ball rolling on a beam
% 
% States:
% x1 = ball position (m)
% x2 = ball velocity (m/s) 
% x3 = beam angle (rad)
% x4 = beam angular velocity (rad/s)
%
% Input: u = motor torque (N*m)
% Output: y = ball position (m)
%
% Returns the statespace system for the ball on beam problem.


%% Physical Parameters
% Ball parameters
m_ball = 0.01;      % Ball mass (kg)
R_ball = 0.01;      % Ball radius (m)
J_ball = (2/5) * m_ball * R_ball^2; % Ball moment of inertia (kg*m²)

% Beam parameters
m_beam = 0.2;       % Beam mass (kg)
L_beam = 1.0;       % Beam length (m)
J_beam = (1/3) * m_beam * L_beam^2; % Beam moment of inertia about pivot (kg*m²)

% System parameters
g = 9.81;           % Gravitational acceleration (m/s²)
d = 0.03;           % Distance from beam pivot to motor axis (m)

% Friction coefficients
b_ball = 0.001;      % Ball friction coefficient
b_beam = 0.01;       % Beam friction coefficient

%% State-Space Matrices
% For the linearized system around equilibrium (ball at center, beam horizontal)

% State vector: x = [ball_position; ball_velocity; beam_angle; beam_angular_velocity]
% Input: u = motor_torque
% Output: y = ball_position

% Calculate coefficients for the linearized model
% From the nonlinear equations of motion, linearized around equilibrium

% Ball dynamics: considering rolling constraint
I_total = J_ball + m_ball * R_ball^2; % Total ball inertia
alpha = (5/7); % Factor from rolling constraint

% A matrix (4x4)
A = [0,    1,    0,    0;
     0,   -b_ball/(m_ball*alpha),  -m_ball*g/(m_ball*alpha),  0;
     0,    0,    0,    1;
     0,    0,   -m_ball*g*L_beam/(2*J_beam),  -b_beam/J_beam];

% B matrix (4x1) 
B = [0;
     0;
     0;
     d/J_beam];

% C matrix (1x4) - output is ball position
C = [1, 0, 0, 0];

% D matrix (1x1)
D = 0;

%% Create State-Space Object
ballOnBeamSys = ss(A, B, C, D);
ballOnBeamSys.StateName = {'Ball Position (m)', 'Ball Velocity (m/s)', 'Beam Angle (rad)', 'Beam Angular Velocity (rad/s)'};
ballOnBeamSys.InputName = {'Motor Torque (N*m)'};
ballOnBeamSys.OutputName = {'Ball Position (m)'};

sys = ballOnBeamSys;

%% Display System Information
fprintf('Ball-on-Beam State-Space Model\n');
fprintf('===============================\n\n');

fprintf('Physical Parameters:\n');
fprintf('Ball mass: %.3f kg\n', m_ball);
fprintf('Ball radius: %.3f m\n', R_ball);
fprintf('Beam mass: %.3f kg\n', m_beam);
fprintf('Beam length: %.3f m\n', L_beam);
fprintf('Motor distance: %.3f m\n\n', d);

fprintf('State-Space Matrices:\n');
fprintf('A matrix:\n');
disp(A);
fprintf('B matrix:\n');
disp(B);
fprintf('C matrix:\n');
disp(C);
fprintf('D matrix:\n');
disp(D);

%% System Analysis
%{
fprintf('\nSystem Analysis:\n');
fprintf('================\n');

% Eigenvalues (poles)
poles = eig(A);
fprintf('System poles: ');
for i = 1:length(poles)
    if imag(poles(i)) == 0
        fprintf('%.4f ', real(poles(i)));
    else
        fprintf('%.4f±%.4fj ', real(poles(i)), abs(imag(poles(i))));
    end
end
fprintf('\n');

% Check stability
if all(real(poles) < 0)
    fprintf('System is stable (all poles in left half-plane)\n');
elseif any(real(poles) > 0)
    fprintf('System is unstable (poles in right half-plane)\n');
else
    fprintf('System is marginally stable (poles on imaginary axis)\n');
end

% Check controllability
Co = ctrb(A, B);
rank_Co = rank(Co);
fprintf('Controllability matrix rank: %d (full rank = %d)\n', rank_Co, size(A,1));
if rank_Co == size(A,1)
    fprintf('System is controllable\n');
else
    fprintf('System is not controllable\n');
end

% Check observability
Ob = obsv(A, C);
rank_Ob = rank(Ob);
fprintf('Observability matrix rank: %d (full rank = %d)\n', rank_Ob, size(A,1));
if rank_Ob == size(A,1)
    fprintf('System is observable\n');
else
    fprintf('System is not observable\n');
end

%% Step Response
figure(1);
step(ballOnBeamSys);
title('Step Response: Ball Position vs Motor Torque');
grid on;

%% Impulse Response
figure(2);
impulse(ballOnBeamSys);
title('Impulse Response: Ball Position vs Motor Torque');
grid on;

%% Simulation Example
fprintf('\nSimulation Example:\n');
fprintf('==================\n');

% Initial conditions: ball at 0.1m from center, beam tilted 0.05 rad
x0 = [0.1; 0; 0.05; 0];

% Time vector
t = 0:0.01:10;

% Input: step input of 0.1 N*m
u = 0.1 * ones(size(t));

% Simulate system response
[y, t_sim, x] = lsim(ballOnBeamSys, u, t, x0);

% Plot results
figure(3);
subplot(2,2,1);
plot(t_sim, x(:,1));
xlabel('Time (s)'); ylabel('Ball Position (m)');
title('Ball Position vs Time'); grid on;

subplot(2,2,2);
plot(t_sim, x(:,2));
xlabel('Time (s)'); ylabel('Ball Velocity (m/s)');
title('Ball Velocity vs Time'); grid on;

subplot(2,2,3);
plot(t_sim, x(:,3));
xlabel('Time (s)'); ylabel('Beam Angle (rad)');
title('Beam Angle vs Time'); grid on;

subplot(2,2,4);
plot(t_sim, x(:,4));
xlabel('Time (s)'); ylabel('Beam Angular Velocity (rad/s)');
title('Beam Angular Velocity vs Time'); grid on;

sgtitle('Ball-on-Beam System Response (Initial Conditions + Step Input)');

%% Transfer Function
fprintf('\nTransfer Function (Ball Position / Motor Torque):\n');
tf_ballOnBeamSys = tf(ballOnBeamSys);
tf_ballOnBeamSys

%% Save workspace
% save('ball_beam_model.mat', 'sys', 'A', 'B', 'C', 'D', 'poles');
fprintf('\nModel saved to workspace. Use "sys" for further analysis.\n');
%}
end