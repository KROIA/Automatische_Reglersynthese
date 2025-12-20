%% PI Controller Design using Root Locus Method
% This example demonstrates how to design a PI controller for system 
% stabilization using root locus analysis
%
% Initialwerte:
% K_selected = 1;   % Definiert das gewählte K welches durch die WOK bestimmt wird.
%
% How To:
% 1) System definieren: G
% 2) Define the prototype controller C_base
% 3) Script laufen lassen
% 4) Mit Hilfe der Wurzelortskurve ein passenden Gain (K_selected) auswählen.
%    Wenn man nicht zu fruieden mit der WOK ist, kann man versuchen die Nullstelle der PI-Reglers an eine andere
%    Position zu verschieben, z.B. -5
% 
if matlab.engine.isEngineShared == 0
    matlab.engine.shareEngine('Experiment') % Used to be able to connect from the C++ application to this matlab session
end
clc;
maxPlotY = 2;
S=tf('s'); 
plotLayout = [4,2];


%% Define the Plant Transfer Function G(s)
G = ballOnBeam();
%G = ss(1/(S-1));
Cdim = size(G.C);
GX0 = zeros(Cdim(2), 1);


%% Design a controller prototype using Root Locus
% The controller is defined as K * C_base;
% The C_base needs to be defined below:

%C_base = tf([1, 2, 2], [1, 8, 8]);
N1 = (S - 0i + 1) * (S + 0i + 1);
N2 = (S - 0i + 2) * (S + 0i + 2);
N3 = (S - 0.1i + 2) * (S + 0.1i + 2);
%
C_base = N1*N2/((S+10)*(S+10)*(S+10)*(S+10)); %N1*N2*N3/( (S+4) * (S+4) * (S+4)*(S+4) * (S+4) * (S+4));

try 
    Cb_NUM;
    Cb_DEN;
catch
    Cb_NUM = [1];
    Cb_DEN = [1];
end


%C_base = tf(Cb_NUM,Cb_DEN);
%C_base = PID_P + PID_I*1/S + PID_D*n/(1+PID_N*1/S);

%% Choose K Parameter
K_selected = 2;


%% Step Response of Original System
figure(1);
subplot(plotLayout(2),plotLayout(1),1);
step(G);
ylim([0 maxPlotY]);
title('Step Response of Original System G(s)');
grid on;

%% Open Loop Transfer Function for Root Locus
% Open loop: 
L = C_base * G;


%% Final Controller
C_final = K_selected * C_base;
fprintf('C_final = \n');
C_final
[C_NUM, C_DEN] = tfdata(C_final);
C_NUM = C_NUM{1};
C_DEN = C_DEN{1};
controllerSys = C_final;
plantSys = G;

%% Closed-Loop System Analysis
% Closed-loop transfer function: 
T_closed = feedback(C_final * G, 1);

fprintf('\nClosed-Loop Transfer Function T(s):\n');
display(T_closed);
fprintf('DC Gain=%.3f\n', dcgain(T_closed));



%% ---------------------------------------------------------
%%                Plotting
%% ---------------------------------------------------------
% Add design specifications (example: damping ratio = 0.7)
zeta = 0.7;  % Desired damping ratio

%% Nyquist
figure(1);
subplot(plotLayout(2),plotLayout(1),2);
nyquist(L);
title('Nyquist of L(s)=C_base(s)*G(s)');

%% Root Locus Analysis
figure(1);
subplot(plotLayout(2),plotLayout(1),3);
rlocus(L);
title('Root Locus of L(s)=C(s)*G(s)');
grid on;
sgrid(zeta, []);  % Add damping ratio lines to root locus


% Negative K root locus
figure(1);
subplot(plotLayout(2),plotLayout(1),4);
hold on;
rlocus(-L);
title('Root Locus of -L(s)=-C(s)*G(s)');
grid on;
sgrid(zeta, []);  % Add damping ratio lines to root locus

%% Performance Analysis


figure(1);

% Step response comparison
subplot(plotLayout(2),plotLayout(1),5);
step(G, 'b--', T_closed, 'r-', 30);
ylim([0 maxPlotY]);
legend('Original System', 'With Controller', 'Location', 'best');
title('Step Response Comparison');
grid on;

% Nyquist plot
figure(1);
subplot(plotLayout(2),plotLayout(1),6);
nyquist(C_final * G);
title('Nyquist of L(s)=C(s)*G(s)');



% Root locus with selected point
poles_final = pole(T_closed);
subplot(plotLayout(2),plotLayout(1),7);
rlocus(L);
sgrid(zeta, []);
title('Root Locus with Selected Poles');
grid on;
if(K_selected >= 0)
    hold on;
    for i = 1:length(poles_final)
        plot(real(poles_final(i)), imag(poles_final(i)), 'ro', 'MarkerSize', 8, 'LineWidth', 2);
        plot(real(poles_final(i)), -imag(poles_final(i)), 'ro', 'MarkerSize', 8, 'LineWidth', 2);
    end
    hold off;
end
legend('Root Locus', 'Selected Poles', 'Location', 'best');

subplot(plotLayout(2),plotLayout(1),8);
rlocus(-L);
sgrid(zeta, []);
title('Negative Root Locus with Selected Poles');
grid on;
if(K_selected < 0)
    hold on;
    for i = 1:length(poles_final)
        plot(real(poles_final(i)), imag(poles_final(i)), 'ro', 'MarkerSize', 8, 'LineWidth', 2);
        plot(real(poles_final(i)), -imag(poles_final(i)), 'ro', 'MarkerSize', 8, 'LineWidth', 2);
    end
    hold off;
end
legend('Negative Root Locus', 'Selected Poles', 'Location', 'best');