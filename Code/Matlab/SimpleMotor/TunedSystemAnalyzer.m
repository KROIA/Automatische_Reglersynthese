


motorSys = tf(1, [0.14, 1]);
pidSys = tunablePID('pid', 'PID');
pidSys.Kp.Value = 5.1756;
pidSys.Ki.Value = 41.6351;
pidSys.Kd.Value = -0.0015;

forwardSystem = pidSys * motorSys;


figure;
nyquist(motorSys);
hold on;

% Draw unit circle centered at (-1, 0)
theta = linspace(0, 2*pi, 100);
x_circle = cos(theta) - 1;
y_circle = sin(theta);
plot(x_circle, y_circle, 'r--', 'LineWidth', 1.5);

% Add legend
legend('Nyquist Plot', 'Unit Circle at (-1,0)');
hold off;
grid on;
