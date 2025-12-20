#pragma once

#include "AutoTuner_base.h"
#include "Utilities/TimeBasedSystem.h"
#include <complex>

namespace AutoTuner
{
	class AUTO_TUNER_API FrequencyResponse
	{
	public:
		struct FrequencyResponsePoint {
			double frequency;
			std::complex<double> gain;  // amplitude and phase
		};
		struct FrequencyResponseData
		{
			double crossingFrequency = 0;
			double gainCrossoverFrequency = 0;

			double gainMargin = 0;  // 
			double phaseMargin = 0; // in radians
			std::vector<FrequencyResponsePoint> responsePoints;
		};
		

		FrequencyResponse();
		~FrequencyResponse();

		void setSettelingTime(double time) { m_settelingTime = time; }
		double getSettelingTime() const { return m_settelingTime; }
		//void setDeltaTime(double deltaTime) { m_deltaTime = deltaTime; }
		//double getDeltaTime() const { return m_deltaTime; }
		void setPointsPerDecade(size_t points) { m_pointsPerDecade = points; }
		size_t getPointsPerDecade() const { return m_pointsPerDecade; }
		void setSignalGain(double gain) { m_signalGain = gain; }
		double getSignalGain() const { return m_signalGain; }

		void setInputIndex(size_t index) { m_inputIndex = index; }
		size_t getInputIndex() const { return m_inputIndex; }
		void setOutputIndex(size_t index) { m_outputIndex = index; }
		size_t getOutputIndex() const { return m_outputIndex; }
		
		FrequencyResponseData getResponse(TimeBasedSystem& system, double startFrequency, double endFrequency) const;



		//static void drawNyquistPlot(const std::vector<FrequencyResponsePoint>& response, const char* title = "Nyquist Plot");
	private:
		static std::complex<double> computePhasor(const std::vector<double>& signal, double freq, double dt);
		static std::vector<double> logspace(double start, double end, int n);


		double m_signalGain = 1;
		double m_settelingTime		= 10;
		//double m_deltaTime			= 0.005;
		size_t m_pointsPerDecade	= 10;

		size_t m_inputIndex = 0;
		size_t m_outputIndex = 0;
	};
}