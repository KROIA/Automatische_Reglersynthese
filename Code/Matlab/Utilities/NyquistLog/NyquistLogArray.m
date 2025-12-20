function fig = NyquistLogArray(sys,sys_colors,NgPlot,tol)

% NYQUISTLOG makes a polar plot of the open loop transfer function sys: the
% amplitude M=|sys| is converted to a logarithmic scale according to the
% following function:
%         _
%        |   M^(log10(2))       if M < 1
% L(M)=|
%        |_  2-M^(-log10(2))    if M > 1
%
% The optional boolean parameter NGPLOT specifies if also the diagram for
% negative frequencies should be plotted.
% The optional parameter TOL specifies the tolerance for quasi zero-pole
% cancelation by the MINREAL function.
% Default values are NGPLOT = TRUE and TOL = SQRT(EPS).
% Param sys is a cellarray of multiple systems: {sys1, sys2, ...}
%       The names of the system is defined in each system
%       and can be defined using: sys1.Name = 'name';
% Param sys_colors is a cellarray of colors for each system

% May 2021
% Luca Ballotta
% Department of Information Engineering,
% University of Padova,
% Padova, Italy.
%
% Modified version of the function Closed_Logarithmic_Nyquist by
% Roberto Zanasi and Federica Grossi.
%
% May be distributed freely for non-commercial use,
% but please leave the above info unchanged, for
% credit and feedback purposes.


%%%%%%%%%
% Input check
% Handle array of systems
if ~iscell(sys)
    if length(sys) > 1
        sys_array = cell(1, length(sys));
        for i = 1:length(sys)
            sys_array{i} = sys(i);
        end
        sys = sys_array;
    else
        sys = {sys};
    end
else
    % Already a cell array
end

num_systems = length(sys);

% Checking optional arguments
if nargin<2 || isempty(sys_colors)
    % Default color array
    sys_colors = {'r', 'b', 'g', 'm', 'c', 'k', [0.8500 0.3250 0.0980], [0.4940 0.1840 0.5560]};
else
    % Convert character array to cell array if needed
    if ischar(sys_colors) || isstring(sys_colors)
        sys_colors_cell = cell(1, length(sys_colors));
        for i = 1:length(sys_colors)
            sys_colors_cell{i} = sys_colors(i);
        end
        sys_colors = sys_colors_cell;
    end
    if length(sys_colors) < num_systems
        error('NyquistlLog:InputError',...
            'Number of colors must be at least equal to number of systems.');
    end
    % Convert hex colors to RGB triplets
    for i = 1:length(sys_colors)
        if ischar(sys_colors{i}) || isstring(sys_colors{i})
            color_str = char(sys_colors{i});
            if length(color_str) > 1 && color_str(1) == '#'
                % Convert hex to RGB
                sys_colors{i} = sscanf(color_str(2:end), '%2x%2x%2x', [1 3])/255;
            end
        end
    end
end

% Ensure sys_colors is a cell array with enough colors
if ~iscell(sys_colors)
    error('NyquistlLog:InputError',...
        'sys_colors must be a cell array or character array.');
end

if nargin<3
    NgPlot = true;
elseif (NgPlot~=true && NgPlot~=false)
    error('NyquistlLog:InputError',...
        '''NgPlot'' must be boolean valued.');
end
if nargin<4
    tol = sqrt(eps);
elseif (not(isscalar(tol)) && tol<=0)
    error('NyquistlLog:InputError',...
        '''tol'' must be positive valued.');
end

% Get system names from .Name property or use default
sys_names = cell(1, num_systems);
for i = 1:num_systems
    if length(sys{i}.Name) == 0
        sys_names{i} = sprintf('System %d', i);
    else
        sys_names{i} = sys{i}.Name;
    end
end

% Validate each system
for idx = 1:num_systems
    current_sys = sys{idx};
    
    % Checking system variable:
    [hlp,den]=tfdata(current_sys,'v');
    k=1;
    while (hlp(k) == 0) k=k+1; end
    num=(hlp(k:end));
    hlp=size(size(den));
    if (hlp(2)> 2)
        error('NyquistlLog:InputError',...
            'Only monovariable systems allowed.');
    end
    % Checking delay:
    dly=get(current_sys,'ioDelay');
    if (dly>0)
        error('NyquistlLog:InputError',...
            'Delay not allowed.');
    end
    % Checking continuous-time:
    sorz=get(current_sys,'variable');
    if (sorz ~= 's')
        error('NyquistlLog:InputError',...
            'Only continuous systems allowed.');
    end
    % Checking system order:
    k=1;
    while (den(k) == 0) k=k+1; end
    den=(den(k:end));
    sysdim=size(den); sysdim=sysdim(2)-1;
    if (sysdim == 0)
        error('NyquistlLog:InputError',...
            'Denominator order of zero not allowed.');
    end
    numdim=size(num); numdim = numdim(2)-1;
    if (numdim > sysdim)
        error('NyquistlLog:InputError',...
            'Only causal systems allowed.');
    end
    % Checking open-loop stability
    sys{idx}=minreal(current_sys,tol);
    sys_poles = pole(sys{idx});
    %if (any(real(sys_poles)>0))
    %    error('NyquistlLog:InputError',...
    %        'Only stable systems allowed.');
    %end
end

%%%%%%%%%
% Plot params
n=2;             % Base of the L function
Lw=1.2;          % Linewidth
Nd=4;            % Number of level lines

%%%%%%%%%%%%%%%%%%%%%%%%
fig = figure('Position', [0, 100, 600, 700]); clf
plot(2*exp(1i*(0:0.01:1)*2*pi),'-')
hold on

ax = gca; % or if you have multiple axes, specify which one
% Remove grid
grid off
% Remove tick labels (the numbers) but keep the axis lines
ax.XAxis.TickValues = [];
ax.YAxis.TickValues = [];

plot(exp(1i*(0:0.01:1)*2*pi),'-')
plot(exp(1i*pi),'*r')
text(-1.05,-0.05,'-1')
xx=1*exp(1i*pi/4);
text(real(xx),imag(xx),[num2str(0) 'dB'])
for ii=1:Nd
    plot(1/(n^ii)*exp(1i*(0:0.01:1)*2*pi),':')
    plot((2-1/(n^ii))*exp(1i*(0:0.01:1)*2*pi),':')
    if ii<3
        xx=1/(n^ii)*exp(1i*pi/4);
        text(real(xx),imag(xx),[num2str(-ii*20) 'dB'])
        xx=(2-n^(-ii))*exp(1i*pi/4);
        text(real(xx),imag(xx),[num2str(ii*20) 'dB'])
    end
end
for ii=1:12 % plot of sectors of pi/6
    ps=2*exp(1i*ii*pi/6);
    plot([0 real(ps)],[0 imag(ps)],':')
end
plot([-1 1]*2,[-1 1]*0,':')
plot([-1 1]*0,[-1 1]*2,':')
axis equal
axis([-1 1 -1 1]*2.2)

% Store handles for legend
legend_handles = [];
crossing_freqs = cell(num_systems, 1);

% Plot each system
for sys_idx = 1:num_systems
    current_sys = sys{sys_idx};
    current_color = sys_colors{sys_idx};
    
    XPlot.c = current_color;
    XPlot.Lw = Lw;
    XPlot.NgPlot = NgPlot;
    XPlot.sys = current_sys;
    
    sys_poles = pole(current_sys);
    [mult,freqs_inf] = groupcounts(imag(sys_poles(real(sys_poles)==0 & imag(sys_poles)>0)));
    n_freqs_inf = length(freqs_inf);
    
    if isempty(freqs_inf)
        [RE,IM] = nyquist(XPlot.sys,logspace(-Nd,Nd,1000));
    else
        n_points = 1000/(n_freqs_inf+1);
        freq_inf_th = .01;
        while max(abs(freqresp(current_sys,freqs_inf-freq_inf_th))) <= 1
            freq_inf_th = freq_inf_th / 2;
        end
        [RE,IM] = nyquist(XPlot.sys,logspace(-Nd,log10(freqs_inf(1)-freq_inf_th),n_points));
        for ii = 1:n_freqs_inf-1
            [RE_i,IM_i] = nyquist(XPlot.sys,logspace(log10(freqs_inf(ii)+freq_inf_th),log10(freqs_inf(ii+1)-freq_inf_th),n_points));
            RE = cat(3,RE,RE_i);
            IM = cat(3,IM,IM_i);
        end
        [RE_f,IM_f] = nyquist(XPlot.sys,logspace(log10(freqs_inf(end)+freq_inf_th),Nd,n_points));
        RE = cat(3,RE,RE_f);
        IM = cat(3,IM,IM_f);
    end
    NY=(RE(:,:)+1i*IM(:,:));
    
    %%%%%%%%%%%%%%%%%%%%%
    NY_L2=PlotLogNyq(NY,n,freqs_inf,mult);
    h_main = plot(NY_L2{1,1},'Color',current_color,'LineWidth',Lw);
    legend_handles = [legend_handles, h_main];
    
    % Find where the plot enters the unit circle (MD2 crosses 1)
    NY_L2_first = NY_L2{1,1};
    MD2_vals = abs(NY_L2_first);
    % Find first point where magnitude crosses below 1 (enters unit circle)
    crossing_idx = find(MD2_vals < 1, 1, 'first');
    if ~isempty(crossing_idx) && crossing_idx > 1
        % Get the frequency at this point
        if isempty(freqs_inf)
            freq_vector = logspace(-Nd,Nd,1000);
        else
            n_points = 1000/(n_freqs_inf+1);
            freq_vector = logspace(-Nd,log10(freqs_inf(1)-0.01),n_points);
        end
        crossing_freq = freq_vector(crossing_idx);
        crossing_freqs{sys_idx} = sprintf('%s: %.2f rad/s', sys_names{sys_idx}, crossing_freq);
    else
        crossing_freqs{sys_idx} = sprintf('%s: N/A', sys_names{sys_idx});
    end
    
    for ii = 1:n_freqs_inf
        plot(NY_L2{1,2*ii},'Color',current_color,'LineWidth',Lw,'Linestyle',':')
        plot([real(NY_L2{1,2*ii}(end)) real(NY_L2{1,2*ii+1}(1))],[imag(NY_L2{1,2*ii}(end)) imag(NY_L2{1,2*ii+1}(1))],'Color',current_color,'Linewidth',Lw)
        plot(NY_L2{1,2*ii+1},'Color',current_color,'LineWidth',Lw)
    end
    
    % plot for w<0 and complete
    compr_fact = .98;
    NY_1=NY_L2{1,1};
    circ_inf=CompleteNyqPlot(NY_1(1),XPlot,compr_fact);
    if XPlot.NgPlot
        plot(compr_fact*conj(NY_L2{1,1}),'Color',current_color,'LineWidth',Lw,'Linestyle','--')
        for ii = 1:n_freqs_inf
            plot(compr_fact*conj(NY_L2{1,2*ii}),'Color',current_color,'LineWidth',Lw,'Linestyle',':')
            plot(compr_fact*[real(NY_L2{1,2*ii}(end)) real(NY_L2{1,2*ii+1}(1))],-compr_fact*[imag(NY_L2{1,2*ii}(end)) imag(NY_L2{1,2*ii+1}(1))],...
                'Color',current_color,'Linewidth',Lw,'Linestyle','--')
            plot(compr_fact*conj(NY_L2{1,2*ii+1}),'Color',current_color,'LineWidth',Lw,'Linestyle','--')
        end
    end
    
    % Arrows
    pp=[150 480 600 700 850];
    NY_L2_mat=transpose(cell2mat(NY_L2));
    for jj=pp % arrows for w>0
        plotArrow(real(NY_L2_mat(jj)),imag(NY_L2_mat(jj)),real(NY_L2_mat(jj+1)),imag(NY_L2_mat(jj+1)),0.1,0.1,current_color)
    end
    if XPlot.NgPlot
        pp=[150 480 600 700];
        for jj=pp % arrows for w<0
            plotArrow(compr_fact*real(NY_L2_mat(jj)),-compr_fact*imag(NY_L2_mat(jj)),compr_fact*real(NY_L2_mat(jj-1)),-compr_fact*imag(NY_L2_mat(jj-1)),0.1,0.1,current_color)
        end
    end
    
    % Print stability information
    fprintf(1,'\n=== %s ===\n', sys_names{sys_idx});
    openloop_rhp_poles = sum(real(sys_poles)>0);
    fprintf(1,'Number of open-loop poles in RHP: %i\n',openloop_rhp_poles);
    NY_L2_mat=[NY_L2_mat;transpose(circ_inf(round(end/2):end))];
    [ncirc,npoles_on_im_axis] = CountEncirc(conj(NY_L2_mat),NY_L2_mat);
    if npoles_on_im_axis > 0
        fprintf(1,'%d closed-loop pole(s) on the im-axis.\n',npoles_on_im_axis);
        fprintf(1,'Graph goes through the -1 point,\n');
        fprintf(1,'encirclement counting infeasible.\n');
    else
        fprintf(1,'Number of net encirclements around the -1 point:   %i\n',ncirc);
        closedloop_rhp_poles=ncirc+openloop_rhp_poles;
        fprintf(1,'=> Number of closed-loop poles in RHP:   %i\n',...
            closedloop_rhp_poles);
        if closedloop_rhp_poles > 0
            fprintf(1,'=> Closed-loop system is unstable\n');
        else
            fprintf(1,'and no closed-loop poles on Im-axis\n=> Closed-loop system is asymptotically stable\n');
        end
    end
end

title('Logarithmisches Nyquist Diagramm')
xlabel('Reale Achse')
ylabel('Imaginäre Achse')
legend(legend_handles, sys_names, 'Location', 'eastoutside')

% Add text box with crossing frequencies
crossing_text = sprintf('Crossover Frequenzen:\n%s', strjoin(crossing_freqs, '\n'));

legendObj = fig.Children(1);
legendObj.Position = [0.25 0.1 0.1 0.05];

annotation('textbox', [0.6 0.112 0.1 0.05], ...
    'String', crossing_text, ...
    'FitBoxToText', 'on', ...
    'BackgroundColor', 'white', ...
    'EdgeColor', 'black', ...
    'LineWidth', 1, ...
    'FontSize', 10, ...
    'Interpreter', 'none');

%**************************************************************************
function h=CompleteNyqPlot(NY1,XPlot,compr_fact)
Np_origin=length(find(pole(XPlot.sys)==0)); % Number of poles at the origin
vv=(angle(conj(NY1)):-0.02:angle(conj(NY1))-Np_origin*pi);
delta_spiral=0.003*(Np_origin>2);
xx=max(2,abs(NY1))*(1-delta_spiral*vv).*exp(1i*vv);
if XPlot.NgPlot
    plot(xx,'Color',XPlot.c,'Linewidth',XPlot.Lw,'Linestyle',':')
    plot([real(xx(end)) real(NY1)],[imag(xx(end)) imag(NY1)],'Color',XPlot.c,'Linewidth',XPlot.Lw,'Linestyle','-')
    plot([real(xx(1)) compr_fact*real(NY1)],[imag(xx(1)) -compr_fact*imag(NY1)],'Color',XPlot.c,'Linewidth',XPlot.Lw,'Linestyle','--')
    %Nr_arrow=2*Np_origin; % Number of arrows
    Nr_arrow=2; % Number of arrows
    if Nr_arrow>0
        try
        randVal = rand();
        pp=round(size(xx,2)*randVal/(Nr_arrow));
        plotArrow(real(xx(pp)),imag(xx(pp)),real(xx(pp+1)),imag(xx(pp+1)),0.1,0.1,XPlot.c)
        pp = size(xx,2) - pp;
        plotArrow(real(xx(pp)),imag(xx(pp)),real(xx(pp+1)),imag(xx(pp+1)),0.1,0.1,XPlot.c)
        catch

        end
        %for jj=[1:Nr_arrow]
        %    pp=jj*round(size(xx,2)*randVal/(Nr_arrow));
        %    randVal = 1-randVal;
        %    plotArrow(real(xx(pp)),imag(xx(pp)),real(xx(pp+1)),imag(xx(pp+1)),0.1,0.1,XPlot.c)
        %end
    end
end
h=xx;
return

%**************************************************************************
function circ=ImaginaryPoleCircle(init_point,end_point_abs,mult)
vv=(angle(init_point):-0.02:angle(init_point)-mult*pi);
delta_spiral=0.003*(mult>2);
circ=linspace(abs(init_point),end_point_abs,length(vv)).*(1-delta_spiral*vv).*exp(1i*vv);
return

%**************************************************************************
function plotArrow(x0,y0,x,y,rx,ry,c)
% Plots an arrow at (x,y) along vector dx=x-x0, dy=y-y0. The length of the arrow
% is rx and ry in the directions x and y. The arrow color is c.
Lw=1.2;         % Linewidth
dx=(x-x0)/rx;
dy=(y-y0)/ry;
dxy=sqrt(dx^2+dy^2);
fx=-dx/dxy;
fy=-dy/dxy;
rotpiu=[rx,0;0,ry]*[cos(pi/6), -sin(pi/6); sin(pi/6), cos(pi/6) ]*[fx,fy]';
rotmeno=[rx,0;0,ry]*[cos(pi/6), sin(pi/6); -sin(pi/6), cos(pi/6) ]*[fx,fy]';
plot([x0,x],[y0,y],'-')
plot([x,x+rotpiu(1)],[y,y+rotpiu(2)],'Color', c,'LineWidth',Lw)
plot([x,x+rotmeno(1)],[y,y+rotmeno(2)],'Color', c,'LineWidth',Lw)
return

%**************************************************************************
function Ln=PlotLogNyq(NY,n,freqs_inf,mult)
RE=real(NY);
IM=imag(NY);
MD=sqrt(RE.^2+IM.^2);
PH=atan2(IM,RE);
MD2=MD.^(log10(n));

ind=find(MD2>1);
MD2(ind)=2-1./MD2(ind);

n_freqs_inf = length(freqs_inf);
n_points = 1000/(n_freqs_inf+1);
Ln = cell(1,2*n_freqs_inf+1);

for ii = 0:n_freqs_inf-1
    Ln{1,1+2*ii}=MD2(1+ii*n_points:(ii+1)*n_points).*exp(1i*PH(1+ii*n_points:(ii+1)*n_points));
    circ_i=ImaginaryPoleCircle(Ln{1,1+2*ii}(end),MD2(1+(ii+1)*n_points),mult(ii+1));
    Ln{1,2+2*ii}=circ_i;
end
Ln{1,end}=MD2(1+n_freqs_inf*n_points:end).*exp(1i*PH(1+n_freqs_inf*n_points:end));
return

%**************************************************************************
function [ncirc,npoles_on_im_axis] = CountEncirc(zmirr,zmain)

% Counts net encirclements around the point -1.
% An encirclement is counted as positive if the direction
% is clockwise. This follows Belanger (1995):
% "Control Engeering", Saunders College Publishing,
% pp 206 - 208.

eps=1e-6;
zmirr(1:end)=zmirr(end:-1:1);
zmirr=zmirr(2:end-1);
zall=[zmirr;zmain;zmirr(1)];
if abs(imag(zall(1))) < eps
    zall=[zall;zall(2)];
end
ncirc=0;
npoles_on_im_axis=0;
z3=zall(end);
for k=3:length(zall)
    z4=z3;
    z1=zall(k);z2=zall(k-1); z3=zall(k-2);
    abz1=abs(z1+1);abz2=abs(z2+1); abz3=abs(z3+1);
    zre1=real(z1); zre2=real(z2);
    %   Checking if graph is too close to -1:
    dl1= fromline2minusone(z1,z2);
    dl2= fromline2minusone(z2,z3);
    dl3= fromline2minusone(z3,z4);
    closest_now = abz1 > abz2 && abz3 > abz2;
    if closest_now && min([dl1 dl2 dl3]) < 1e-5 ...
            && min([abz1 abz2 abz3])< 0.001
        npoles_on_im_axis=npoles_on_im_axis+1;
    end
    %   end checking if graph is too close to -1.
    
    % Only checking for Re axis crossings to the left
    % of minus 0.9 to avoid unnecessary work:
    if zre1 < -0.9
        zim1=imag(z1); zim2=imag(z2); zim3=imag(z3);
        if zim1*zim2 < 0
            % Interpolation to find real z value at crossing:
            delta12=(real(z1)-real(z2))*abs(imag(z2))/(abs(imag(z1))+abs(imag(z2)));
            realcross=real(z2)+delta12;
        end
        if zim1 > eps && zim2 < -eps
            if realcross < -1
                ncirc=ncirc+1;
            end
        elseif zim1 < -eps && zim2 > eps
            if realcross < -1
                ncirc=ncirc-1;
            end
        elseif abs(zim2) < eps && zim1 > 0 && zim3 < 0
            if real(z2) < -1
                ncirc=ncirc+1;
            end
        elseif abs(zim2) < eps && zim3 > 0 && zim1 < 0
            if real(z2) < -1
                ncirc=ncirc-1;
            end
        else
        end
    end % real(z1) < -0.99
end
return

% ********************************
function [distline_to_minus1] = fromline2minusone(z1,z2)
% Calcucates the min. distance from the point -1 to the line z1,z2
v=z2-z1;
v=imag(v)-1i*real(v);
v=v/abs(z2-z1);
r=z1+1;
d=dot([real(v) imag(v)],[real(r) imag(r)]');
distline_to_minus1=abs(d);
return