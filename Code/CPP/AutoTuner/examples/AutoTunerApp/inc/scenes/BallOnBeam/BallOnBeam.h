#pragma once

#include "AutoTuner.h"
#include "Ball.h"
#include "Beam.h"

#include <QLabel>
#include <QLineEdit>

class BallOnBeam : public AutoTuner::Experiment
{
	Q_OBJECT
public:

	BallOnBeam();
	~BallOnBeam() override;


protected:
	QWidget* createPropertyWidget() override;
	void onSetup(QSFML::Scene* scene) override;
	void onStart() override;
	void onStop() override;
	void onReset() override;
	void onHit() override;
	void onDisturbance() override;

	void onSceneUpdate(QSFML::Scene* scene) override;
	void onScopeCapture() override;

	void setCurrentState(float ballXPos, float ballAngle, float beamAngle);
	void setControllerTF(const std::vector<double> &num, const std::vector<double>& den);

private slots:
	void onControllerInputTFChanged();
	void onLoadControllerFromMatlabClicked();
	void onLoadSystemFromMatlabClicked();

private:

	Ball* m_ball = nullptr;
	Beam* m_beam = nullptr;

	MatlabAPI::StateSpaceModel* m_model = nullptr;
	MatlabAPI::StateSpaceModel* m_controllerModel = nullptr;

	MatlabAPI::Matrix x0;

	QLabel* m_controllerTFLabel = nullptr;
	QLineEdit* m_controllerNumeratorEdit = nullptr;
	QLineEdit* m_controllerDenominatorEdit = nullptr;


	double m_lastTorque = 0.0;
	double m_lastError = 0.0;
	double m_disturbance = 0;
	QVector<double> m_positionData;
	QVector<double> m_velocityData;
	
	QVector<double> m_torqueData;
	QVector<double> m_errorData;


	//QVector<double> m_timeData;

	MatlabAPI::StateSpaceModel::C2DMethod m_c2dMethod = MatlabAPI::StateSpaceModel::C2DMethod::MatchedPoleZero;
};
