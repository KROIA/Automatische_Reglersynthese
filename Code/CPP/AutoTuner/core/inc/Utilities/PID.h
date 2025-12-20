#pragma once

#include "AutoTuner_base.h"
#include "Utilities/TimeBasedSystem.h"
#include "Utilities/StatespaceSystem.h"

namespace AutoTuner
{
	class AUTO_TUNER_API PID : public TimeBasedSystem
	{
	public:
		/*enum class PIDType
		{
			P = 1,
			I = 2,
			D = 4,
			N = 8,
			PI = P | I,
			PD = P | D,
			ID = I | D,
			PID = P | I | D,
			PIDN = P | I | D | N
		};*/
		enum class AntiWindupMethod
		{
			None,
			Clamping,
			BackCalculation
		};
		enum class DerivativeType
		{
			Unfiltered,
			Filtered
		};

		PID();
		PID(double kp, double ki, double kd);
		PID(double kp, double ki, double kd, double kn);
		PID(const PID& other);
		TimeBasedSystem* clone() override
		{
			return new PID(*this);
		}
		~PID();

		void reset() override
		{
			m_inputValue = 0.0;
			m_lastInputValue = 0.0;
			m_integral = 0.0;
			m_lastDerivativeValue = 0.0;
			m_outputValue = 0.0;
			m_outputValueBeforeSaturation = 0.0;
			m_outputPositiveSaturated = false;
			m_outputNegativeSaturated = false;

			//if (m_statespaceRepresentation)
			//{
			//	m_statespaceRepresentation->reset();
			//}
		}

		void setIntegrationSolver(IntegrationSolver solver) override;
		void setAntiWindupMethod(AntiWindupMethod method)
		{
			m_antiWindupMethod = method;
		}
		AntiWindupMethod getAntiWindupMethod() const
		{
			return m_antiWindupMethod;
		}
		void setDerivativeType(DerivativeType type)
		{
			m_derivativeType = type;
		}
		DerivativeType getDerivativeType() const
		{
			return m_derivativeType;
		}

		void setParameters(double kp, double ki, double kd);
		void setParameters(double kp, double ki, double kd, double kn);
		void setKp(double kp);
		void setKi(double ki);
		void setKd(double kd);
		void setKn(double kn);

		double getKp() const
		{
			return m_kp;
		}
		double getKi() const
		{
			return m_ki;
		}
		double getKd() const
		{
			return m_kd;
		}
		double getKn() const
		{
			return m_kn;
		}

		void setIntegralSatturationLimit(double limit) {
			m_iSaturationLimit = std::max(0.0, limit);
		}
		double getIntegralSatturationLimit() const{
			return m_iSaturationLimit;
		}
		void setOutputSaturationLimits(double lowerLimit, double upperLimit) {
			m_outputSaturationLimitLower = lowerLimit;
			m_outputSaturationLimitUpper = upperLimit;
		}
		std::pair<double, double> getOutputSaturationLimits() const {
			return { m_outputSaturationLimitLower,  m_outputSaturationLimitUpper };
		}

		bool isOutputPositiveSaturated() const {
			return m_outputPositiveSaturated;
		}
		bool isOutputNegativeSaturated() const {
			return m_outputNegativeSaturated;
		}
		bool isOutputSaturated() const {
			return m_outputPositiveSaturated || m_outputNegativeSaturated;
		}
		void setAntiWindupBackCalculationConstant(double constant) {
			m_backCalculationConstant = constant;
		}
		double getAntiWindupBackCalculationConstant() const {
			return m_backCalculationConstant;
		}

		void update(double deltaTime) override;

		void setInput(double u)
		{
			m_inputValue = u;
		}
		void setInputSignal(size_t input, double value) override
		{
			if (input == 0)
				m_inputValue = value;
		}
		void setInputSignals(double u) override;
		void setInputSignals(const std::vector<double>& u) override;
		std::vector<double> getInputs() const override;
		std::vector<double> getOutputs() const override;
		double getOutput() const
		{
			return m_outputValue;
		}
		double getOutput(size_t index) const override
		{
			if(index == 0)
				return m_outputValue;
			return 0.0;
		}
		double getInput(size_t index) const override
		{
			if (index == 0)
				return m_inputValue;
			return 0.0;
		}
	private:
		
		//PIDType m_pidType = PIDType::PID;
		AntiWindupMethod m_antiWindupMethod = AntiWindupMethod::None;
		DerivativeType m_derivativeType = DerivativeType::Unfiltered;


		double m_kp = 0.0; // Proportional gain
		double m_ki = 0.0; // Integral gain
		double m_kd = 0.0; // Derivative gain
		double m_kn = 0.0; // Derivative filter coefficient
		double m_backCalculationConstant = 0.0; // Back-calculation constant for anti-windup


		double m_iSaturationLimit = 1e6; // Integral saturation limit
		double m_outputSaturationLimitLower = -1e6;
		double m_outputSaturationLimitUpper = 1e6;

		double m_integral = 0.0;

		double m_inputValue = 0.0;
		double m_lastInputValue = 0.0;
		double m_lastDerivativeValue = 0.0;
		double m_outputValue = 0.0;
		double m_outputValueBeforeSaturation = 0.0;
		bool m_outputPositiveSaturated = false;
		bool m_outputNegativeSaturated = false;

		//StatespaceSystem* m_statespaceRepresentation = nullptr;
	};
}