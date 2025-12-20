#include "Utilities/PID.h"

namespace AutoTuner
{
	PID::PID()
	{
		setParameters(0, 0, 0, 0);
		setIntegrationSolver(getDefaultIntegrationSolver());
		setDifferentiationSolver(getDefaultDifferentiationSolver());
	}
	PID::PID(double kp, double ki, double kd)
	{
		setParameters(kp, ki, kd);
		setIntegrationSolver(getDefaultIntegrationSolver());
		setDifferentiationSolver(getDefaultDifferentiationSolver());
	}
	PID::PID(double kp, double ki, double kd, double kn)
	{
		setParameters(kp, ki, kd, kn);
		setIntegrationSolver(getDefaultIntegrationSolver());
		setDifferentiationSolver(getDefaultDifferentiationSolver());
	}
	PID::PID(const PID& other)
		: TimeBasedSystem(other)
		, m_kp(other.m_kp)
		, m_ki(other.m_ki)
		, m_kd(other.m_kd)
		, m_kn(other.m_kn)
		, m_backCalculationConstant(other.m_backCalculationConstant)
		, m_iSaturationLimit(other.m_iSaturationLimit)
		, m_outputSaturationLimitLower(other.m_outputSaturationLimitLower)
		, m_outputSaturationLimitUpper(other.m_outputSaturationLimitUpper)
		, m_integral(other.m_integral)
		, m_inputValue(other.m_inputValue)
		, m_lastInputValue(other.m_lastInputValue)
		, m_lastDerivativeValue(other.m_lastDerivativeValue)
		, m_outputValue(other.m_outputValue)
		, m_outputValueBeforeSaturation(other.m_outputValueBeforeSaturation)
		, m_outputPositiveSaturated(other.m_outputPositiveSaturated)
		, m_outputNegativeSaturated(other.m_outputNegativeSaturated)		
	{

	}
	PID::~PID()
	{
		//if (m_statespaceRepresentation)
		//{
		//	delete m_statespaceRepresentation;
		//	m_statespaceRepresentation = nullptr;
		//}
	}

	void PID::setIntegrationSolver(IntegrationSolver solver)
	{
		TimeBasedSystem::setIntegrationSolver(solver);
		//if (m_statespaceRepresentation)
		//{
		//	m_statespaceRepresentation->setIntegrationSolver(solver);
		//}
	}

	void PID::setParameters(double kp, double ki, double kd)
	{
		m_ki = ki;
		m_kp = kp;
		m_kd = kd;
		m_kn = 0.0;
		//if (m_statespaceRepresentation)
		//{
		//	delete m_statespaceRepresentation;
		//	m_statespaceRepresentation = nullptr;
		//}
	}
	void PID::setParameters(double kp, double ki, double kd, double kn)
	{
		//if (!m_statespaceRepresentation)
		//	m_statespaceRepresentation = new StatespaceSystem();

		m_kp = kp;
		m_ki = ki;
		m_kd = kd;
		m_kn = kn;
		//double s1 = kp + kd * kn;
		//double s2 = ki + kn * kp;
		//double s3 = ki * kn;
		//
		//
		//MatlabAPI::Matrix A({ 
		//	{ 0,   0,   0 },
		//	{ 1,   0,   0},
		//	{ -s2, -s3, -1}
		//	});
		//MatlabAPI::Matrix B({ 
		//	{ 1 },
		//	{ 0 },
		//	{ -s1 }
		//	});
		//MatlabAPI::Matrix C({ { s2, s3, -1 } });
		//MatlabAPI::Matrix D({ { s1 } });
		//
		//
		//m_statespaceRepresentation->setStateSpaceMatrices(A, B, C, D);
	}
	void PID::setKp(double kp)
	{
		//if (m_statespaceRepresentation)
		//	setParameters(kp, m_ki, m_kd, m_kn);
		//else
		//	setParameters(kp, m_ki, m_kd);
		m_kp = kp;
	}
	void PID::setKi(double ki)
	{
		//if (m_statespaceRepresentation)
		//	setParameters(m_kp, ki, m_kd, m_kn);
		//else
		//	setParameters(m_kp, ki, m_kd);
		m_ki = ki;
	}
	void PID::setKd(double kd)
	{
		//if (m_statespaceRepresentation)
		//	setParameters(m_kp, m_ki, kd, m_kn);
		//else
		//	setParameters(m_kp, m_ki, kd);
		m_kd = kd;
	}
	void PID::setKn(double kn)
	{
		//if (m_statespaceRepresentation)
		//	setParameters(m_kp, m_ki, m_kd, kn);
		//else
		//	setParameters(m_kp, m_ki, m_kd);
		m_kn = kn;
	}

	void PID::setInputSignals(double u)
	{
		m_inputValue = u;
		//if (m_statespaceRepresentation)
		//{
		//	m_statespaceRepresentation->setInputSignals(u);
		//}
	}
	void PID::setInputSignals(const std::vector<double>& u)
	{
		if (u.size() > 0)
			m_inputValue = u[0];
		//if (m_statespaceRepresentation)
		//{
		//	m_statespaceRepresentation->setInputSignals(u);
		//}
	}
	std::vector<double> PID::getInputs() const
	{
		return { m_inputValue };
	}
	std::vector<double> PID::getOutputs() const
	{
		return { m_outputValue };
	}

	void PID::update(double deltaTime)
	{
		double proportional = m_kp * m_inputValue;
		double derivative = 0;

		switch (getDerivativeType())
		{
			case DerivativeType::Unfiltered:
			{
				switch (getDifferentiationSolver())
				{
					case DifferentiationSolver::BackwardEuler:
					{
						derivative = TimeBasedSystem::getDifferentiated_backwardEuler(m_lastInputValue, m_inputValue, deltaTime) * m_kd;
						break;
					}
				}
				break;
			}
			case DerivativeType::Filtered:
			{
				derivative = m_kd * m_kn * (m_inputValue - m_lastInputValue) - m_lastDerivativeValue * (m_kn * deltaTime - 1);
				break;
			}
		}

		double toIntegrateSignal     = m_inputValue     * m_ki;
		double toIntegrateLastSignal = m_lastInputValue * m_ki;

		switch (getAntiWindupMethod())
		{
			case AntiWindupMethod::None:
				break;
			case AntiWindupMethod::Clamping:
			{
				bool integralSignEqual = (m_integral > 0 && toIntegrateSignal > 0) || (m_integral < 0 && toIntegrateSignal < 0);
				if ((m_outputPositiveSaturated || m_outputNegativeSaturated) && integralSignEqual)
				{
					// Skip integration when output is saturated and integral has same sign as toIntegrateSignal
					goto afterIntegration;
				}
				break;
			}
			case AntiWindupMethod::BackCalculation:
			{
				double antiWindupSignal = (m_outputValue - m_outputValueBeforeSaturation) * m_backCalculationConstant;
				toIntegrateSignal += antiWindupSignal;
				toIntegrateLastSignal += antiWindupSignal;
				break;
			}
		}


		switch (getIntegrationSolver())
		{
			case IntegrationSolver::ForwardEuler:
			{
				m_integral = TimeBasedSystem::getIntegrated_forwardEuler(toIntegrateSignal, m_integral, deltaTime);
				break;
			}
			case IntegrationSolver::BackwardEuler:
			{
				m_integral = TimeBasedSystem::getIntegrated_backwardEuler(toIntegrateLastSignal, m_integral, deltaTime);
				break;
			}
			case IntegrationSolver::Bilinear:
			{
				m_integral = TimeBasedSystem::getIntegrated_Bilinear(toIntegrateLastSignal, toIntegrateSignal, m_integral, deltaTime);
				break;
			}
		}
		afterIntegration:
		

		// Apply integral saturation
		if (m_integral < -m_iSaturationLimit)
		{
			m_integral = -m_iSaturationLimit;
		}
		else if (m_integral > m_iSaturationLimit)
		{
			m_integral = m_iSaturationLimit;
		}


		m_outputValueBeforeSaturation = proportional + m_integral + derivative;

		// Apply output saturation
		m_outputPositiveSaturated = false;
		m_outputNegativeSaturated = false;

		if (m_outputValueBeforeSaturation < m_outputSaturationLimitLower)
		{
			m_outputValue = m_outputSaturationLimitLower;
			m_outputNegativeSaturated = true;
		}
		else if (m_outputValueBeforeSaturation > m_outputSaturationLimitUpper)
		{
			m_outputValue = m_outputSaturationLimitUpper;
			m_outputPositiveSaturated = true;
		}
		else
		{
			m_outputValue = m_outputValueBeforeSaturation;
		}

		m_lastInputValue = m_inputValue;
		m_lastDerivativeValue = derivative;
	}
}