[motorMitSchwungmasseStateSpace, blockschaltbildKoeffizienten] = motorMitSchwungmassePlant();
simpleMotorSystem = simpleMotorPlant();





%%
function plantSys = simpleMotorPlant()
    % Motor Parameters
    % Parameters from RegT 1-2 Script, Page 57.
    T = 0.14; % s
    K1 = 1; 
    
    % Transfer function for the DC-Motor
    plantSys = tf(K1, [T, 1]);
    plantSys.InputName = 'u';
    plantSys.OutputName = 'y';
    plantSys.Name = 'plantSys';
end

%%
function [plantSys, blockschaltbildKoeffizienten]= motorMitSchwungmassePlant()
    % Step 1: Define the Plant Model
    % Aus den initial ermittelte Koeffizienten erstelltes SS
    %A = [0 0 -1.14106781843957 0;
    %    0 0 0 -0.195612222183072;
    %    38.2968282439173 38.2968282439173 0.227949698362888 0;
    %    3408.35608456442 3408.35608456442 0 -20.2871563264034];
    %
    %B = [0 0;
    %     0 0;
    %     -0.0108630006922152 0;
    %     0 -0.453744048854055];
    %
    %C = [1 0 0 0;
    %     0 1 0 0];
    %
    %D = [0 0;
    %     0 0];

    % Aus den mit PID ermittelte Koeffizienten erstelltes SS
    A = [
      0 0 -0.809018 0 ;
      0 0 0 -0.228862 ;
      39.2124 39.2124 0.207233 0 ;
      3572.61 3572.61 0 -18.8808 ;
    ];
    
    B = [
      0 0 ;
      0 0 ;
      -0.0132184 0 ;
      0 -0.439267 ;
    ];
    
    C = [
      1 0 0 0 ;
      0 1 0 0 ;
    ];
    
    D = [
      0 0 ;
      0 0 ;
    ];
    
    B = B * 300; % Normierung der Eingangssignale auf +-10
    D = D * 300;
    
    % Transfer function for the DC-Motor
    plantSys = ss(A,B,C,D);
    plantSys.InputName = {'u1', 'u2'};
    plantSys.OutputName = {'y1', 'y2'};


    %% Initial ermittelte Koeffizienten
    %blockschaltbildKoeffizienten = [...
    %    0.047655253638114625...
    %    -0.022366074453890505...
    %    168.00561053145208...
    %    -0.22794969836288803...
    %    20.287156326403448...
    %    -1.1410678184395731...
    %    -0.19561222218307175...
    %    ];

    % Mit integriertem PID-Regler ermittelte Koeffizienten
    % Normierung der Eingangssignale ist im Simulink Blockdiagram manuell umgesetzt
    blockschaltbildKoeffizienten = [0.0637851 -0.0232653 189.219 -0.207233 18.8808 -0.809018 -0.228862 ];
    
    % Display plant
    %disp('Plant Transfer Function:');
    %disp(plantSys);
end