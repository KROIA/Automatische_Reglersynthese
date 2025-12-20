#pragma once

#include "AutoTuner.h"

#define DCMOTOR_USE_SIMPLIFIED_MODEL

class DCMotorWithMassSystem : public AutoTuner::TunableTimeBasedSystem
{
public:
	DCMotorWithMassSystem()
	{
		// Identified with openloop impulse responses at input gain of 2000
		//m_parameters = { 
		//	0.047655253638114625,
		//	-0.022366074453890505, 
		//	168.00561053145208, 
		//	-0.22794969836288803, 
		//	20.287156326403448, 
		//	-1.1410678184395731, 
		//	-0.19561222218307175 };


		// Refined system, identified with closed loop step signals and PID controller
		m_parameters = { 
			0.0637851, 
			-0.0232653, 
			189.219, 
			-0.207233, 
			18.8808, 
			-0.809018, 
			-0.228862 };

		m_inputs = { 0.0, 0.0 };
		m_outputs = { 0.0, 0.0 };
	}
	DCMotorWithMassSystem(const DCMotorWithMassSystem& other)
		: AutoTuner::TunableTimeBasedSystem(other)
	{
		m_parameters = other.m_parameters;
		m_inputs = other.m_inputs;
		m_outputs = other.m_outputs;
		m_disk1 = other.m_disk1;
		m_disk2 = other.m_disk2;
	}
	virtual TimeBasedSystem* clone() override
	{
		return new DCMotorWithMassSystem(*this);
	}

	void reset() override
	{
		for (size_t i = 0; i < m_inputs.size(); ++i)
			m_inputs[i] = 0;
		for (size_t i = 0; i < m_outputs.size(); ++i)
			m_outputs[i] = 0;
		m_disk1 = Disk();
		m_disk2 = Disk();
	}

	void setParameters(const std::vector<double>& params) override { m_parameters = params; }
	std::vector<double> getParameters() const override { return m_parameters; }

	void setInputs(double u1, double u2)
	{
		m_inputs[0] = u1;
		m_inputs[1] = u2;
	}


	void setInputSignals(double u) override
	{
		m_inputs[0] = u;
		m_inputs[1] = 0;
	}
	void setInputSignal(size_t input, double value) override
	{
		m_inputs[input] = value;
	}
	void setInputSignals(const std::vector<double>& u) override
	{
		if (u.size() >= 2)
		{
			m_inputs[0] = u[0];
			m_inputs[1] = u[1];
		}
	}

	double getOutput(size_t index) const override
	{
		return m_outputs[index];
	}
	double getInput(size_t index) const override
	{
		return m_inputs[index];
	}

	/**
	 * @brief
	 * Advances the system by the given time delta.
	 */
	void update(double deltaTime) override
	{
		double input1Scaled = m_inputs[0] * 300 * m_parameters[0];
		double input2Scaled = m_inputs[1] * 300 * m_parameters[1];

		//if (std::abs(m_disk1.angularVelocity) < 0.001 && std::abs(m_inputs[0]) < 400)
		//	input1Scaled = 0;
		//
		//if (std::abs(m_disk2.angularVelocity) < 0.001 && std::abs(m_inputs[1]) < 525)
		//	input2Scaled = 0;


		double angleDifferenceTorque = (m_disk1.angle + m_disk2.angle) * m_parameters[2];

		double torque1 = (input1Scaled - m_disk1.angularVelocity - angleDifferenceTorque) * m_parameters[3];
		double torque2 = (input2Scaled - m_disk2.angularVelocity + angleDifferenceTorque) * m_parameters[4];

		double lastAngularVelocity1 = m_disk1.angularVelocity * m_parameters[5];
		double lastAngularVelocity2 = m_disk2.angularVelocity * m_parameters[6];
		m_disk1.angularVelocity = TimeBasedSystem::getIntegrated_Bilinear(m_disk1.lastPreIntegrationTorque, torque1, m_disk1.angularVelocity, deltaTime);
		m_disk2.angularVelocity = TimeBasedSystem::getIntegrated_Bilinear(m_disk2.lastPreIntegrationTorque, torque2, m_disk2.angularVelocity, deltaTime);


		m_disk1.angle = TimeBasedSystem::getIntegrated_Bilinear(lastAngularVelocity1, m_disk1.angularVelocity * m_parameters[5], m_disk1.angle, deltaTime);
		m_disk2.angle = TimeBasedSystem::getIntegrated_Bilinear(lastAngularVelocity2, m_disk2.angularVelocity * m_parameters[6], m_disk2.angle, deltaTime);
		m_outputs[0] = m_disk1.angle;
		m_outputs[1] = m_disk2.angle;


		m_disk1.lastPreIntegrationTorque = torque1;
		m_disk2.lastPreIntegrationTorque = torque2;
	}

	std::vector<double> getInputs() const override { return m_inputs; }
	std::vector<double> getOutputs() const override { return m_outputs; }

private:
	std::vector<double>	m_parameters;
	std::vector<double> m_inputs;
	std::vector<double> m_outputs;

	struct Disk
	{
		double angle = 0;
		double angularVelocity = 0;
		double lastPreIntegrationTorque = 0;
	};

	Disk m_disk1;
	Disk m_disk2;

};