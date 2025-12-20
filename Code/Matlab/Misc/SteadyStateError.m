%% Control System Definition
% Testversuch aus dem Script Abbildung 60, Seite 82, RegT3/4.
% Ich wollte folgende Aussage prüfen:
%  - Seite 82, RegT3/4:
%  " 
%   Um zusätzliche Integratoren in G0 zu vermeiden, kann eine Vorverstärkung L
%   benutzt werden, so dass die DC-Verstärkung Y(s)/R(s) = L*G0(0)/(1+G0(0)) = 1 wird.
%   Der Wert der Vorverstärkung ist allerdings von den Parametern der Strecke abhängig. 
%   Ändern sich diese, so muss die Vorverstärkung nachgeführt werden.
%  "
%
% Block diagram: G(s):
%     
%	R(S) +---+   +   E(S)  +-------+      Y(S)
%	---->| L |--->( )----->| G0(S) |---*--->
%	     +---+   - ^       +-------+   |
%		           |                   |
%				   +-------------------+
%
% L wird so abgestimmt, sodass:
%   Y(s)        G0(0)
%  ----- = L * ------- = 1
%   R(s)       1+G0(0) 
%
%   
%     1+G0(0)            1
% L = -------  =  ---------------
%      G0(0)      feedback(G0(0))
%

% Clear workspace
clear; clc; close all;

%% Define Transfer Functions
% Plant transfer function G0(s) - example: x/(0.5s^3 + s^2 + 3s + 2)
G0_num = [0.8];               % numerator coefficients  
G0_den = [1 2 3 4];       % denominator coefficients
G0 = tf(G0_num, G0_den);

%         G0(s)
% G0f = ---------
%        1+G0(s)

G0f = feedback(G0, 1);


%        1
% L = -------
%      G0f(0)

L = 1/dcgain(G0f);

G1 = L * G0f;


% G2 ist das System ohne abgeglichenes L, sondern mit einem Integrator
%  G2(s):
%
%	    
%	R(S)   +   E(S)  +-------+   +-------+   Y(S)
%	------->( )----->|  1/s  |-->| G0(S) |---*--->
%	       - ^       +-------+   +-------+   |
%		     |                               |
%			 +-------------------------------+
%
G2 = feedback(tf([1],[1,0]) * G0, 1);

%% System Response Analysis
% Step response
figure(1);
subplot(2, 1, 1);
step(G1);
hold on;
step(G0f);
step(G2);
%step(feedback(G1,1))
%step(feedback(G2,1))
title('Closed-Loop Step Response');
legend('G1(s)', 'G0f(s)', 'G2(s)', 'feedback(G1)', 'feedback(G2)');
grid on;







subplot(2, 1, 2);
nyquist(G1, G0f, G2);
title('Multiple Systems Nyquist Plot');
legend('G1(s)', 'G0f(s)', 'G2(s)', 'Location', 'best');
grid on;


%{
%% System Analysis
% Forward path transfer function
G_forward = L * G0;

% Closed-loop transfer function from R(s) to Y(s)
T = feedback(G_forward, 1);  % Unity negative feedback

% Error transfer function from R(s) to E(s)
E_R = feedback(1, G_forward);

%% Display Transfer Functions
fprintf('Lead Compensator L(s):\n');
display(L);

fprintf('\nPlant G0(s):\n');
display(G0);

fprintf('\nForward Path G(s) = L(s) * G0(s):\n');
display(G_forward);

fprintf('\nClosed-Loop Transfer Function T(s) = Y(s)/R(s):\n');
display(T);

fprintf('\nError Transfer Function E(s)/R(s):\n');
display(E_R);

%% System Response Analysis
% Step response
figure(1);
step(T);
title('Closed-Loop Step Response');
grid on;

% Bode plot of open-loop system
figure(2);
bode(G_forward);
title('Open-Loop Bode Plot (L(s) * G0(s))');
grid on;

% Root locus
figure(3);
rlocus(G_forward);
title('Root Locus of Open-Loop System');
grid on;

%% System Properties
% Stability margins
[Gm, Pm, Wcg, Wcp] = margin(G_forward);
fprintf('\n=== Stability Margins ===\n');
fprintf('Gain Margin: %.2f dB (at %.2f rad/s)\n', 20*log10(Gm), Wcg);
fprintf('Phase Margin: %.2f degrees (at %.2f rad/s)\n', Pm, Wcp);

% Step response characteristics
S = stepinfo(T);
fprintf('\n=== Step Response Characteristics ===\n');
fprintf('Rise Time: %.3f s\n', S.RiseTime);
fprintf('Settling Time: %.3f s\n', S.SettlingTime);
fprintf('Overshoot: %.2f %%\n', S.Overshoot);
fprintf('Peak Time: %.3f s\n', S.PeakTime);

%% Sensitivity Analysis
% Sensitivity function S(s) = 1/(1 + L(s)*G0(s))
S_sens = feedback(1, G_forward);

% Complementary sensitivity function T(s) = L(s)*G0(s)/(1 + L(s)*G0(s))
T_comp = feedback(G_forward, 1);

figure(4);
bodemag(S_sens, T_comp);
legend('Sensitivity S(s)', 'Complementary Sensitivity T(s)');
title('Sensitivity Functions');
grid on;

%% Simulate system response to different inputs
t = 0:0.01:10;  % time vector

% Step input response
figure(5);
subplot(3,1,1);
[y_step, t_step] = step(T, t);
plot(t_step, y_step, 'b', 'LineWidth', 2);
title('Step Response');
xlabel('Time (s)'); ylabel('Output Y(s)');
grid on;

% Ramp input response  
subplot(3,1,2);
[y_ramp, t_ramp] = lsim(T, t, t);
plot(t_ramp, y_ramp, 'r', 'LineWidth', 2);
hold on; plot(t, t, 'k--', 'LineWidth', 1);
title('Ramp Response');
xlabel('Time (s)'); ylabel('Output Y(s)');
legend('System Response', 'Input Ramp', 'Location', 'best');
grid on;

% Impulse response
subplot(3,1,3);
[y_imp, t_imp] = impulse(T, t);
plot(t_imp, y_imp, 'g', 'LineWidth', 2);
title('Impulse Response');
xlabel('Time (s)'); ylabel('Output Y(s)');
grid on;

%}