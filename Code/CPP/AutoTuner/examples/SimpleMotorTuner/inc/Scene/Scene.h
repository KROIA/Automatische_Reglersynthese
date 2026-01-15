#pragma once


#include "AutoTuner.h"
#include "Scene/Objects/DCMotorProblem.h"
#include "Scene/Objects/DCMotorWithMassProblem.h"
#include "Scene/Objects/IntegrationTest.h"
#include "Scene/Objects/MotorWithMassIdentification.h"

class Scene
{
public:
	Scene();
	~Scene();

	void setupScene(QWidget* parent);
	void createDCMotorWithMassProblem();
	void createDCMotorWithMassProblem(PIDTuningProblem::SetupSettings settings);
	PIDTuningProblem::SetupSettings getDefaultDCMotorWithMassSettings() const;

	void createDCMotorProblem();
	void createDCMotorProblem(PIDTuningProblem::SetupSettings settings);
	PIDTuningProblem::SetupSettings getDefaultDCMotorSettings() const;

	void destroyDCMotorProblem();
	void destroyDCMotorWithMassProblem();

	PIDTuningProblem *getPIDTuningProblem() const
	{
		// Return either DCMotorProblem or DCMotorWithMassProblem based on which one is initialized
		if (m_dcMotorProblem)
			return m_dcMotorProblem;
		else
			return m_dcMotorWithMassProblem;
	}
	DCMotorProblem* getDCMotorProblem() const { return m_dcMotorProblem; }
	DCMotorWithMassProblem* getDCMotorWithMassProblem() const { return m_dcMotorWithMassProblem; }
	MotorWithMassIdentification* getMotorWithMassIdentification() const { return m_motorWithMassIdentification; }

	void stop()
	{
		if(m_scene)
			m_scene->stop();
	}
	void start()
	{
		if (m_scene)
			m_scene->start();
	}
private:
	QSFML::Scene* m_scene;

	DCMotorProblem* m_dcMotorProblem = nullptr;
	DCMotorWithMassProblem* m_dcMotorWithMassProblem = nullptr; // corrected variable name for consistency
	MotorWithMassIdentification* m_motorWithMassIdentification = nullptr; // corrected variable name for consistency

};