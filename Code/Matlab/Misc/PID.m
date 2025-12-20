
sys = tf([0, 0.1, 0], [1, 0]);
sysd = c2d(sys, 0.01, 'zoh');
[Ad,Bd,Cd,Dd] = ssdata(ss(sysd)); 
[A,B,C,D] = ssdata(ss(sys));