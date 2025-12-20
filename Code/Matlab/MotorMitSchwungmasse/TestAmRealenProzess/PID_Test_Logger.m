





simout(:,4) = simout(:,4) - simout(1,4); % Remove offset of encoder 1
simout(:,5) = simout(:,5) - simout(1,5); % Remove offset of encoder 2


labels = {'Zeit [s]', 'r [rad]', 'u1','d', 'y1 [rad]','y2 [rad]'};
lineStyles    = {'', '-', '-', '-', '-', '-',};
lineThickness = {0,4,2,1,1,1};
colors        = {0,'ebbd34','8888FF','e0162e','00AA00','0000AA'};


result = [
    labels;
    lineStyles;
    lineThickness;
    colors;
    num2cell(simout)
];



writecell(result(:,[1 2 3 4 6]), 'Measurement.csv', 'Delimiter', ';');