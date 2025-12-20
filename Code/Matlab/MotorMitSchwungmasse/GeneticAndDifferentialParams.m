
[geneticPIDc,      geneticPIDd]      = discretePIDtoContinuous('GeneticPID', -0.771251, -0.0000579791, -0.60542, 3.98857);
[differentialPIDc, differentialPIDd] = discretePIDtoContinuous('DifferentialPID', -0.87958, -0.0000947773, -0.63515, 4.28537);





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