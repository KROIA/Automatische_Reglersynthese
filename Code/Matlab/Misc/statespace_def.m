statespace = struct();

%% Simple
% Define the matrices
A = [0 1; -2 -3];
B = [0; 1];
C = [1 0];
D = [0];


% Create a struct to hold them
statespace.A = A;
statespace.B = B;
statespace.C = C;
statespace.D = D;


%% Pendulum
% Parameters
m = 1;      % mass (kg)
l = 1;      % length (m)
b = 0.1;    % damping coefficient
g = 9.81;   % gravity (m/s^2)

% State-space matrices
A = [0 1;
     -g/l -b/(m*l^2)];
B = [0;
     1/(m*l^2)];
C = [1 0];   % output is theta
D = [0];

statespace.A = A;
statespace.B = B;
statespace.C = C;
statespace.D = D;



%% Analyzer
% Optionally, create the state-space object from the struct
sys = ss(statespace.A, statespace.B, statespace.C, statespace.D);


controlSystemDesigner(sys)
