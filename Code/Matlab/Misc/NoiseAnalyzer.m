%% ===============================================================
%  Noise Analysis Script
%  Reads CSV → extracts 3rd column → calculates FFT → plots results
% ===============================================================

clear; clc; close all;

%% === 1. Load CSV ===
filename = 'SimpleMotor/TestAmRealenProzess/SystemIdentifikation/simoutData1.csv';     % << change to your filename
data = readmatrix(filename);

% Extract 3rd column (noise signal)
noise = data(:, 3);

% If sampling frequency is known, set it here:
Fs = 1000;   % Hz   <<<<< CHANGE THIS to your sampling rate
dt = 1/Fs;
N = length(noise);

%% === 2. Remove DC offset ===
noise = noise - mean(noise);

%% === 3. FFT Analysis ===
Y = fft(noise);
P2 = abs(Y / N);              % two-sided spectrum
P1 = P2(1:N/2+1);             % one-sided spectrum
P1(2:end-1) = 2*P1(2:end-1);

f = Fs * (0:(N/2)) / N;

%% === 4. Plot Time Signal ===
figure;
plot((0:N-1)*dt, noise);
title("Noise Signal (Time Domain)");
xlabel("Time [s]");
ylabel("Amplitude");

%% === 5. Plot FFT ===
figure;
plot(f, P1);
title("Noise Spectrum (FFT)");
xlabel("Frequency [Hz]");
ylabel("Magnitude");
grid on;

%% === 6. Compute PSD (Recommended for Simulink) ===
[PSD, f_psd] = periodogram(noise, [], [], Fs);

figure;
plot(f_psd, 10*log10(PSD));
title("Noise Power Spectral Density (PSD)");
xlabel("Frequency [Hz]");
ylabel("Power/Frequency [dB/Hz]");
grid on;

%% === 7. Export PSD for Simulink usage ===
save('noise_psd.mat','f_psd','PSD');
fprintf("Saved PSD to noise_psd.mat\n");



%% ===============================================================
%  Convert PSD → FIR filter for noise generation
%  Creates a filter you can use in Simulink
% ===============================================================

load('noise_psd.mat');

Fs = 1000; % set this correctly

% Normalize frequencies
f_norm = f_psd / (Fs/2);
f_norm(f_norm > 1) = 1;

% Use amplitude (not power)
H = sqrt(PSD);

% Fix abrupt end transitions
if abs(H(end) - H(end-1)) > 0.1 * H(end-1)
    H(end) = H(end-1); % smooth edge
end

% Remove last Nyquist point (fir2 unstable there)
f_norm = f_norm(1:end-1);
H      = H(1:end-1);

% FIR Design (robust)
N = 2048;
b = firls(N, f_norm, H);

save('noise_filter.mat', 'b', 'Fs');
