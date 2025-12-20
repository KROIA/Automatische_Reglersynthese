#include "Utilities/FrequencyResponse.h"


namespace AutoTuner
{
	FrequencyResponse::FrequencyResponse()
	{

	}
	FrequencyResponse::~FrequencyResponse()
	{

	}

    FrequencyResponse::FrequencyResponseData FrequencyResponse::getResponse(TimeBasedSystem& system, double startFrequency, double endFrequency) const
	{
        FrequencyResponseData data;
		// Get number of decades
		size_t n_decades = static_cast<size_t>(std::log10(endFrequency) - std::log10(startFrequency));
        // Log-spaced frequencies for Nyquist plot
		size_t n_points = n_decades * m_pointsPerDecade;
        std::vector<double> frequencies = logspace(startFrequency, endFrequency, n_points);

		system.reset();

        for (double freq : frequencies) {
            // Reset system or let it settle from previous frequency
            double omega = 2.0 * M_PI * freq;
            double amplitude = m_signalGain;  // Choose appropriate amplitude

            // Simulate several periods to reach steady-state
            double dt = 1.0 / (freq * 1000);  // 100 samples per period
            dt = std::min(0.01, dt);
            int n_settle = static_cast<int>(5.0 / freq / dt);  // 5 periods settling
            int n_measure = static_cast<int>(10.0 / freq / dt); // 10 periods measuring

            // Settling phase
            for (int i = 0; i < n_settle; i++) {
                double t = i * dt;
                double input = amplitude * sin(omega * t);
				system.setInputSignal(m_inputIndex, input);
				system.update(dt);
            }

            // Measurement phase - collect input/output
            std::vector<double> /*input_samples, */ output_samples;
			output_samples.reserve(n_measure);
            for (int i = 0; i < n_measure; i++) 
            {
                double t = (n_settle + i) * dt;
                double input = amplitude * sin(omega * t);
				system.setInputSignal(m_inputIndex, input);
				system.update(dt);
				double output = system.getOutput(m_outputIndex); // Assuming single output system
               // input_samples.push_back(input);
                output_samples.push_back(output);
            }

            // Extract complex gain using DFT at fundamental frequency
            //std::complex<double> input_phasor = computePhasor(input_samples, freq, dt);
            std::complex<double> input_phasor(0.0, -1.0); // For x(t) = sin(2πft), the phasor at frequency f is -j
            std::complex<double> output_phasor = computePhasor(output_samples, freq, dt);

            data.responsePoints.push_back({ freq, output_phasor / input_phasor });
        }

		// Find crossing frequency, gain margin, and phase margin
        bool crossedOver = false;
        double lastGain = 0;
        double lastArg = 0;
        double lastImag = 0;
        for (size_t i = 0; i < data.responsePoints.size(); ++i)
        {
            const auto& point = data.responsePoints[i];
            double magnitude = std::abs(point.gain);
            double phase = std::arg(point.gain);
            // Gain crossover frequency (magnitude = 1)
            if (magnitude <= 1.0 && (i == 0 || lastGain > 1.0))
            {
                data.crossingFrequency = point.frequency;
                data.phaseMargin = M_PI + phase; // Phase margin in radians
				// handle wrap-around
				data.phaseMargin = std::fmod(data.phaseMargin + M_PI, 2.0 * M_PI) - M_PI;
                crossedOver = true;
            }
            // Phase crossover frequency (phase = -180 degrees)
            if (crossedOver)
            {
                //if (phase <= M_PI && (i == 0 || lastArg > -M_PI))
                if (lastImag < 0 && point.gain.imag() > 0)
                {
                    data.gainMargin = 1.0 / magnitude; // Gain margin
                    data.gainCrossoverFrequency = point.frequency;
                    break;
                }
            }
            lastArg = phase;
            lastGain = magnitude;
			lastImag = point.gain.imag();
        }

		return data;
	}


	std::complex<double> FrequencyResponse::computePhasor(const std::vector<double>& signal, double freq, double dt) 
	{
		double omega = 2.0 * M_PI * freq;
		std::complex<double> sum(0.0, 0.0);

		for (size_t i = 0; i < signal.size(); i++) {
			double t = i * dt;
			std::complex<double> exp_term(cos(omega * t), -sin(omega * t));
			sum += signal[i] * exp_term;
		}

		return sum * (2.0 / signal.size());
	}

	std::vector<double> FrequencyResponse::logspace(double start, double end, int n) 
	{
		std::vector<double> result(n);
		double log_start = log10(start);
		double log_end = log10(end);
		for (int i = 0; i < n; i++) {
			result[i] = pow(10.0, log_start + i * (log_end - log_start) / (n - 1));
		}
		return result;
	}
}