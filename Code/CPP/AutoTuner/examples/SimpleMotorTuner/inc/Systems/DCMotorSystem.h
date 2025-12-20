#pragma once

#include "AutoTuner.h"

#define DCMOTOR_USE_SIMPLIFIED_MODEL

class DCMotorSystem : public AutoTuner::TimeBasedSystem
{
public:
	DCMotorSystem()
	{
		// Parameters from RegT 1 - 2 Script, Page 57.
		double T = 0.14; // s
		double K1 = 1; // unitless
		double R = 1.9; // Ohm
		double J = 2.5e-4; // kg* m ^ 2
		double Psi = sqrt(R * J / T); // Wb
		double Kw = 2.4e-2; // Vs
		double Kp = K1 / Kw * Psi; // unitless


		//double K1 = Kp * Kw / Psi;
		m_k2 = R * Kw / (Psi * Psi);
		m_k1 = K1;

		MatlabAPI::Matrix A({{-1 / T}});
		MatlabAPI::Matrix B({ { K1 / T, -m_k2 / T } });
		MatlabAPI::Matrix C({ { 1 } });
		MatlabAPI::Matrix D({ { 0, 0 } });

		m_k3 = 0.005;

		//setStateSpaceMatrices(A, B, C, D);



		// System parameters for a oscilating model
		/*const double omega_n = 10;  // Natural frequency (rad/s) - makes it fast
		const double zeta = 0.2;     // Damping ratio (0.1-0.3 gives nice oscillation)

		MatlabAPI::Matrix A({ { 0.0, 1.0 },
			{ -omega_n * omega_n,    -2.0 * zeta * omega_n } });
		MatlabAPI::Matrix B({ { 0.0 },
			{ omega_n * omega_n } });
		MatlabAPI::Matrix C({ {1.0, 0.0} });
		MatlabAPI::Matrix D({ { 0 } });
		setStateSpaceMatrices(A, B, C, D); */

		m_inputVoltage = 0;
		m_disturbance = 0;
	}
	DCMotorSystem(const DCMotorSystem& other)
		: TimeBasedSystem(other)
	{
		m_inputVoltage = other.m_inputVoltage;
		m_disturbance = other.m_disturbance;
		m_integratorOutput = other.m_integratorOutput;
		m_outputAngularVelocity = other.m_outputAngularVelocity;
		m_lastPreIntegratorSignal = other.m_lastPreIntegratorSignal;
		m_invTimeConstant = other.m_invTimeConstant;
		m_k1 = other.m_k1;
		m_k2 = other.m_k2;
		m_k3 = other.m_k3;
	}
	TimeBasedSystem* clone() override
	{
		return new DCMotorSystem(*this);
	}
	//virtual void update(double deltaTime) override
	//{
	//
	//}

	void reset() override 
	{
		m_inputVoltage = 0;
		m_disturbance = 0;
		m_integratorOutput = 0;
		m_outputAngularVelocity = 0;
		m_lastPreIntegratorSignal = 0;

	}

	void setInputVoltage(double voltage)
	{
		m_inputVoltage = voltage;
	}
	void setDisturbance(double disturbance)
	{
		m_disturbance = disturbance;
	}
	void setInputs(double voltage, double disturbance)
	{
		m_inputVoltage = voltage;
		m_disturbance = disturbance;
	}
	void setInputSignal(size_t input, double value) override
	{
		if (input == 0)
			m_inputVoltage = value;
		else if (input == 1)
			m_disturbance = value;
	}
	double getAngularVelocity() const
	{
		return m_outputAngularVelocity;
	}

	void setInputSignals(double u) override
	{
		m_inputVoltage = u;
		m_disturbance = 0;
	}
	void setInputSignals(const std::vector<double>& u) override
	{
		if(u.size() >= 2)
		{
			m_inputVoltage = u[0];
			m_disturbance = u[1];
		}
	}

	double getOutput(size_t index) const override
	{
		if(index == 0)
			return m_outputAngularVelocity;
		return 0.0;
	}
	double getInput(size_t index) const override
	{
		if(index == 0)
			return m_inputVoltage;
		else if(index == 1)
			return m_disturbance;
		return 0.0;		
	}

	/**
	 * @brief
	 * Advances the system by the given time delta.
	 */
	void update(double deltaTime) override
	{
#ifdef DCMOTOR_USE_SIMPLIFIED_MODEL
		double& y = m_outputAngularVelocity;
		double& u = m_inputVoltage;
		double& l = m_disturbance; // l wie Last
		double& k1 = m_k1;
		double& k2 = m_k2;
		double& k3 = m_k3;
		double& invT = m_invTimeConstant;
		double preIntegratorSignal = (k1 * u - y * (1.0 + k2 * k3 * l)) * invT;
#else
		double preIntegratorSignal = m_invTimeConstant * (m_inputVoltage - m_outputAngularVelocity) - (m_outputAngularVelocity * 1.0 * m_disturbance);
#endif
		switch (getIntegrationSolver())
		{
			case IntegrationSolver::ForwardEuler:
			{
				m_integratorOutput = getIntegrated_forwardEuler(m_lastPreIntegratorSignal, m_integratorOutput, deltaTime);
				break;
			}
			case IntegrationSolver::BackwardEuler:
			{
				m_integratorOutput = getIntegrated_backwardEuler(preIntegratorSignal, m_integratorOutput, deltaTime);
				break;
			}
			case IntegrationSolver::Bilinear:
			{
				m_integratorOutput = getIntegrated_Bilinear(m_lastPreIntegratorSignal, preIntegratorSignal, m_integratorOutput, deltaTime);
				break;
			}
		}
#ifdef DCMOTOR_USE_SIMPLIFIED_MODEL
		m_outputAngularVelocity = m_integratorOutput;
#else
		if (m_integratorOutput < 1)
		{
			m_outputAngularVelocity = (m_integratorOutput * 0.43 + 0.21) * m_integratorOutput;
		}
		else
		{
			m_outputAngularVelocity = (m_integratorOutput * 1.07) - 0.43;
		}
#endif
		m_lastPreIntegratorSignal = preIntegratorSignal;
	} 

	std::vector<double> getInputs() const override
	{
		return { m_inputVoltage, m_disturbance };
	}
	std::vector<double> getOutputs() const override
	{
		return { m_outputAngularVelocity };
	}

private:
	double m_inputVoltage = 0;
	double m_disturbance = 0;
	double m_integratorOutput = 0;
	double m_outputAngularVelocity = 0;
	double m_lastPreIntegratorSignal = 0;

	//double m_disturbanceScale = 1;
	double m_invTimeConstant = 1.0 / 0.14; // 1/s

	double m_k1;
	double m_k2;
	double m_k3;
};