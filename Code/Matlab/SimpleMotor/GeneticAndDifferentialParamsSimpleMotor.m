
[geneticPIDc,      geneticPIDd]      = discretePIDtoContinuous('GeneticPID', 7.16, 109, 0.0013, 1.27e-4); %1.27e-5
[differentialPIDc, differentialPIDd] = discretePIDtoContinuous('DifferentialPID', 6.124, 84.536, -0.487648, 0.622535);





function [continuousPID, discretePID]  = discretePIDtoContinuous(name, Kp, Ki, Kd, Kn)
    syms z
    Ts = 0.01;

    % Tustin discrete PID
    Cz = Kp + Ki*Ts/(z-1) + Kd*Kn/(1+Kn*Ts/(z-1));
    
    Cz = simplify(Cz);   % symbolic discrete PID
    [num, den] = numden(Cz);
    num = sym2poly(num);
    den = sym2poly(den);
    
    discretePID = tf(num, den, Ts);
    continuousPID = pid(d2c(discretePID,'forward'));   % discretized PID
    continuousPID.Name = name;
    discretePID = pid(discretePID);
    discretePID.Name = name;
end