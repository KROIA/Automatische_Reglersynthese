s = tf('s');

% Example: 2 inputs, 2 outputs
G11 = 1/(s+1);         % From input 1 → output 1
G12 = 2/(s+3);         % From input 2 → output 1
G21 = (s+2)/(s^2+2*s+5); % From input 1 → output 2
G22 = 3/(s+4);         % From input 2 → output 2

% Combine into a MIMO transfer function matrix
sys = [ G11  G12 ;
        G21  G22 ];

Ts = 0.1; 

sysd = c2d(sys,Ts,'zoh'); 
S = ss(sysd); 
[Ad,Bd,Cd,Dd, dt] = ssdata(S);

% ensure column vectors/matrices: if A is scalar convert to matrix form
A = double(Ad); 
B = double(Bd); 
C = double(Cd); 
D = double(Dd);


%% --- Simulation settings ---
%dt      = 0.01;      % integration timestep (s)
Tfinal  = 2.0;        % total simulation time (s)
t       = 0:dt:Tfinal;
N       = numel(t);

% input: shape (nu x N)
nu = size(B, 2);
u = zeros(nu, N);

% Example input choices (uncomment one)
% Input 1: step at t=0
u(1,:) = ((t >= 0.0) - ((t>=1)/2));

% Input 2: step at t=2
u(2,:) = (t >= 0.0);
% u(1, :) = 0.5 * sin(2*pi*1*t);     % sine
% u(1, :) = randn(1,N)*0.1;          % noise

% initial state
nx = size(A,1);
x0 = zeros(nx,1);

% choose integrator: 'euler' or 'rk4'
integrator = 'euler';

%% --- Preallocate and initialize ---
x = zeros(nx, N);
ny = size(C,1);
y = zeros(ny, N);

x(:,1) = x0;
y(:,1) = C * x(:,1) + D * u(:,1);

%% --- Step-by-step integration loop ---
for k = 1:(N-1)
    uk = u(:,k);            % input during this step (piecewise-constant)
    switch lower(integrator)
        case 'euler'
            %dx = A * x(:,k) + B * uk;
            %x(:,k+1) = x(:,k) + dx*dt;
            x(:,k+1) = A * x(:,k) + B * uk;

        case 'rk4'
            % classical RK4 with input held constant over the step
            k1 = A * x(:,k) + B * uk;
            k2 = A * (x(:,k) + 0.5 * dt * k1) + B * uk;
            k3 = A * (x(:,k) + 0.5 * dt * k2) + B * uk;
            k4 = A * (x(:,k) + dt * k3) + B * uk;
            x(:,k+1) = x(:,k) + (dt/6) * (k1 + 2*k2 + 2*k3 + k4);

        otherwise
            error('Unknown integrator: choose ''euler'' or ''rk4''.');
    end

    % compute output at next time-step (you can also compute y at current time)
    y(:,k+1) = C * x(:,k+1) + D * u(:,k+1);
end

%% --- Plot results ---
figure('Name','State & Output vs Time','NumberTitle','off');
subplot(2,1,1);
plot(t, x.');
xlabel('Time (s)');
ylabel('State x');
grid on;
title(sprintf('States (integrator = %s)', integrator));

subplot(2,1,2);
plot(t, y.');
hold on;
plot(t, u.');
xlabel('Time (s)');
ylabel('Output y');
grid on;
title('Output');

%% --- (optional) animation / step-wise visualization ---
% Example: animate output value as a moving marker
figure('Name','Realtime-ish visualization','NumberTitle','off');
h1 = plot(t(1), y(1,1), 'o-');
hold on;
h2 = plot(t(1), y(2,1), 'o-');
xlim([t(1) t(end)]);
ylim([min(y(:)) - 0.1, max(y(:)) + 0.1]);
xlabel('Time (s)');
ylabel('y');
grid on;
for k = 1:N
    set(h1,'XData1',t(1:k),'YData',y(1,1:k));
    set(h2,'XData2',t(1:k),'YData',y(2,1:k));
    drawnow limitrate;
    % In a real app you would push the current x(:,k) / y(:,k) to your renderer
end