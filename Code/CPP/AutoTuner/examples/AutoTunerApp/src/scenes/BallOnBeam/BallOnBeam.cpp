#include "scenes/BallOnBeam/BallOnBeam.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>

BallOnBeam::BallOnBeam()
	: Experiment("Ball On Beam")
{
	MatlabAPI::StateSpaceModel::setDefaultIntegrationSolver(MatlabAPI::StateSpaceModel::Rk4);
	setScopePointCount(1000);
	setScopeCaptureTickInterval(1);
}

BallOnBeam::~BallOnBeam()
{

}

QWidget* BallOnBeam::createPropertyWidget()
{
	double valueScale = 1.0;
	QWidget* widget = new QWidget();
	QLayout* layout = new QVBoxLayout();

	m_controllerTFLabel = new QLabel("Controller TF: N/A");
	m_controllerTFLabel->setFont(QFont("Consolas", 10));
	m_controllerTFLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
	m_controllerNumeratorEdit = new QLineEdit("");
	m_controllerDenominatorEdit = new QLineEdit("");

	QLayout* tfNumeratorLayout = new QHBoxLayout();
	tfNumeratorLayout->addWidget(new QLabel("Numerator:"));
	tfNumeratorLayout->addWidget(m_controllerNumeratorEdit);
	layout->addItem(tfNumeratorLayout);

	QLayout* tfDenominatorLayout = new QHBoxLayout();
	tfDenominatorLayout->addWidget(new QLabel("Denominator:"));
	tfDenominatorLayout->addWidget(m_controllerDenominatorEdit);
	layout->addItem(tfDenominatorLayout);

	QPushButton* applyButton = new QPushButton("Apply");
	connect(applyButton, &QPushButton::clicked, this, &BallOnBeam::onControllerInputTFChanged);
	layout->addWidget(m_controllerTFLabel);
	layout->addWidget(applyButton);

	QPushButton* loadControllerSystemFromMatlabButton = new QPushButton("Load Controller from Matlab \"controllerSys\"");
	connect(loadControllerSystemFromMatlabButton, &QPushButton::clicked, this, &BallOnBeam::onLoadControllerFromMatlabClicked);
	layout->addWidget(loadControllerSystemFromMatlabButton);

	QPushButton* loadPlantSystemFromMatlabButton = new QPushButton("Load Plant from Matlab \"plantSys\"");
	connect(loadPlantSystemFromMatlabButton, &QPushButton::clicked, this, &BallOnBeam::onLoadSystemFromMatlabClicked);
	layout->addWidget(loadPlantSystemFromMatlabButton);


	registerSignal(m_positionData, "State", "Position", {0, 1, 0, 1}, 1);
	registerSignal(m_velocityData, "State", "Speed", { 1,0.5,0, 1 }, 1);

	registerSignal(m_torqueData, "Controller", "Output");
	registerSignal(m_errorData, "Controller", "Input");

	QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	layout->addItem(spacer);
	widget->setLayout(layout);
	return widget;
	
	//return new QPushButton("Test");
}
void BallOnBeam::onSetup(QSFML::Scene* scene)
{
	m_ball = new Ball(10.f, sf::Color::Green);
	m_beam = new Beam(1000.f);

	scene->addObject(m_beam);
	scene->addObject(m_ball);
	
	// Setup model
	double m_ball = 0.01;
	double r_ball = 0.01;
	double j_ball = (2.0 / 5.0) * m_ball * r_ball * r_ball;

	double m_beam = 0.2;
	double l_beam = 1;
	double j_beam = (1.0 / 3) * m_beam * l_beam * l_beam;

	double g = 9.81;
	double d = 0.03; // Distance from beam pivot to motor axis (m)

	double b_ball = 0.001; // Ball friction coeff
	double b_beam = 0.01; // Beam friction coeff

	double alpha = 5 / 7.0; // Factor from rolling constraint



	MatlabAPI::Matrix A({
		{0, 1, 0, 0},
		{0,   -b_ball / (m_ball * alpha),  -m_ball * g / (m_ball * alpha),  0},
		{0, 0, 0, 1},
		{0,    0,   -m_ball * g * l_beam / (2 * j_beam),  -b_beam / j_beam}
		});

	MatlabAPI::Matrix B({
		{0},
		{0},
		{0},
		{d / j_beam},
		});
	MatlabAPI::Matrix C({
		{1, 0, 0, 0},
		});

	MatlabAPI::Matrix D({
		{0},
		});

	x0 = MatlabAPI::Matrix({
		{0},	// Ball position
		{0.0},	// Ball velocity
		{0.0},	// Beam angle
		{0},	// Beam angular velocity
		});

	m_model = new MatlabAPI::StateSpaceModel(A, B, C, D, x0, getPhysicsDeltaT(), m_c2dMethod);
	//setControllerTF({ -4, -26, -58.04, -54.18, -18.18 }, { 1, 16, 69, 256, 256 });
	//setControllerTF({ -1460, -8760, -18980, -17520, -5840 }, { 1, 40, 600, 4000, 10000 });
	setControllerTF({ -48427.3, -2.48402e+07, -1.36894e+08, -4.86427e+08,  -8.31864e+08 }, { 1, 564.148, 31160.2, 869001,  1.54157e+07 });
}
void BallOnBeam::onStart()
{

}
void BallOnBeam::onStop()
{

}
void BallOnBeam::onReset()
{
	m_model->setState(x0);
	auto controllerState = m_controllerModel->getState();
	for (size_t i = 0; i < controllerState.getRows(); i++)
		controllerState(i, 0) = 0;
	m_controllerModel->setState(controllerState);
}
void BallOnBeam::onHit()
{
	MatlabAPI::Matrix x = m_model->getState();
	x(1, 0) = 0.5; // Add some velocity to the ball
	m_model->setState(x);
}
void BallOnBeam::onDisturbance()
{
	m_disturbance = 10 * getPhysicsDeltaT(); // Add some velocity to the ball
}

void BallOnBeam::onSceneUpdate(QSFML::Scene* scene)
{
	//double sliderPercent = m_slider->value() / 100.0;
	//MatlabAPI::Matrix u({ {sliderPercent} });
	//m_model->processTimeStep(u);
	double lastPos = 0;
	MatlabAPI::Matrix u({ {0 - lastPos} });
	double error = u(0, 0);

	for (int substep = 0; substep < 100; ++substep)
	{
		const MatlabAPI::Matrix& lastX = m_model->getState();
		lastPos = lastX(0, 0);
		error = 0 - lastPos;
		u(0,0) = error;
		
		if (m_controllerModel)
		{
			m_controllerModel->processTimeStep(u);
			//m_controllerModel->processTimeStepApproxContinuesRk4(u);
			u = m_controllerModel->getOutput();
		}
		if (m_disturbance != 0)
		{
			MatlabAPI::Matrix x = m_model->getState();
			x(1, 0) += m_disturbance;
			m_model->setState(x);
		}
		m_model->processTimeStep(u);
		//m_model->processTimeStepApproxContinuesRk4(u);
	}
	m_disturbance = 0;
	MatlabAPI::Matrix x = m_model->getState();

	// Check bounds
	if (x(0, 0) < -0.5)
	{
		x(0, 0) = -0.5;
		x(1, 0) = 0;
		m_model->setState(x);
	}
	else if (x(0, 0) > 0.5)
	{
		x(0, 0) = 0.5;
		x(1, 0) = 0;
		m_model->setState(x);
	}

	float ballPos = static_cast<float>(x(0, 0));
	float ballVel = static_cast<float>(x(1, 0));
	float beamAngle = static_cast<float>(x(2, 0));
	m_lastTorque = static_cast<float>(u(0, 0));
	m_lastError = error;

	setCurrentState(ballPos, 0, -beamAngle);
}
void BallOnBeam::onScopeCapture()
{
	const MatlabAPI::Matrix &x = m_model->getState();
	double ballPos = x(0, 0);
	double ballVel =  x(1, 0);
	double beamAngle = x(2, 0);

	m_positionData.append(ballPos);
	m_velocityData.append(ballVel);
	m_torqueData.append(m_lastTorque);
	m_errorData.append(m_lastError);
}

void BallOnBeam::setCurrentState(float ballXPos, float ballAngle, float beamAngle)
{
	// Visual positioning calculations only:
	float ballYEstimate = tan(beamAngle) * ballXPos; // Estimate ball Y position based on beam angle and ball X position
	float ballRadius = m_ball->getRadius();
	float ak = ballRadius;
	float gk = tan(beamAngle) * ballRadius;
	float h = sqrt(ak * ak + gk * gk) - ballRadius;
	float ak2 = m_beam->getThickness();
	float gk2 = tan(beamAngle) * ak2;
	float h2 = sqrt(ak2 * ak2 + gk2 * gk2);
	h += h2;
	sf::Vector2f ballPosition(ballXPos * 1000, ballYEstimate * 1000 - h); // Scale and offset for visualization

	m_ball->setPosition(ballPosition);
	m_ball->setRotation(ballAngle);
	m_beam->setBeamAngle(beamAngle);
}

void BallOnBeam::setControllerTF(const std::vector<double>& num, const std::vector<double>& den)
{
	
	//MatlabAPI::TransferFunction pidTf({ -2, -2.8, -1.16 }, { 1, 10, 25 }); // Stabil
	//MatlabAPI::TransferFunction pidTf({ -40, -200, -399.2, -384, -153.5 }, { 1, 20, 150, 500, 625 }); // Stabil
	//MatlabAPI::TransferFunction pidTf({ -130, -455, -734.8, -559.6, -169.7 }, { 1, 20, 150, 500, 625 }); // Stabil
	//MatlabAPI::TransferFunction pidTf({ -4, -15.2, -21.68, -13.86, -3.434 }, { 1, 10, 37, 60, 36 }); // Stabil
	//MatlabAPI::TransferFunction pidTf({ -60, -540, -1695, -2235, -1020 }, { 1, 40, 400 }); // Stabil
	//MatlabAPI::TransferFunction pidTf({ -4, -26, -58.04, -54.18, -18.18 }, { 1, 16, 69, 256, 256 }); // Stabil
	//MatlabAPI::TransferFunction pidTf({ -5, -32.5, -72.7, -68.4, -23.4 }, { 1, 16, 69, 256, 256 }); // Stabil
	//MatlabAPI::TransferFunction pidTf({ -12.75, -81.6, -185.4, -197.3, -100,  }, { 1, 16, 69, 256, 256 }); // Stabil
	//MatlabAPI::TransferFunction pidTf({ -0.4856,  0.4921 }, { 1, 2.239 }); // Stabil
	//MatlabAPI::TransferFunction pidTf({ d, p , i }, { 1, 0 });

	if (den.size() < num.size())
	{
		m_controllerTFLabel->setText("Controller TF: Not causal controller!\n(denominator order < numerator order)");
		return;
	}

	MatlabAPI::TransferFunction transferF(num, den);
	if (m_controllerModel)
		delete m_controllerModel;
	m_controllerModel = new MatlabAPI::StateSpaceModel(transferF.toStateSpaceModel(getPhysicsDeltaT(), m_c2dMethod));

	QString numeratorStr = "";
	QString numStr = "";
	for (size_t i = 0; i < num.size(); i++)
	{
		numeratorStr += QString::number(num[i]);
		if (i < num.size() - 2)
		{
			numeratorStr += +"s^" + QString::number(num.size() - i - 1) + " + ";
		}else if (i < num.size() - 1)
		{
			numeratorStr += +"s + ";
		}
		numStr += QString::number(num[i]) + " ";
	}
	QString denominatorStr = "";
	QString denStr = "";
	for (size_t i = 0; i < den.size(); i++)
	{
		denominatorStr += QString::number(den[i]);
		if (i < den.size() - 2)
		{
			denominatorStr += +"s^" + QString::number(den.size() - i - 1) + " + ";
		}else if (i < den.size() - 1)
		{
			denominatorStr += +"s + ";
		}
		denStr += QString::number(den[i]) + " ";
	}
	QString line(std::max(numeratorStr.length(), denominatorStr.length()), '-');

	m_controllerNumeratorEdit->setText(numStr);
	m_controllerDenominatorEdit->setText(denStr);
	m_controllerTFLabel->setText("Controller TF:\n" + numeratorStr + "\n" + line + "\n" + denominatorStr);
	onReset();
}

void BallOnBeam::onControllerInputTFChanged()
{
	try {
		// Parse numerator and denominator
		QString numText = m_controllerNumeratorEdit->text();
		QString denText = m_controllerDenominatorEdit->text();

		std::vector<double> num;
		std::vector<double> den;

		for (const QString& part : numText.split(' ', Qt::SkipEmptyParts))
		{
			bool ok;
			double val = part.toDouble(&ok);
			if (ok)
				num.push_back(val);
		}
		for (const QString& part : denText.split(' ', Qt::SkipEmptyParts))
		{
			bool ok;
			double val = part.toDouble(&ok);
			if (ok)
				den.push_back(val);
		}
		if (num.size() > 0 && den.size() > 0)
		{
			setControllerTF(num, den);
		}
		else
		{
			m_controllerTFLabel->setText("Controller TF: Invalid input");
		}
	}
	catch (const std::exception& e)
	{
		m_controllerTFLabel->setText("Controller TF: Error parsing input");
	}
}
void BallOnBeam::onLoadControllerFromMatlabClicked()
{
	try {
		MatlabAPI::MatlabArray* controllerVar = MatlabAPI::MatlabEngine::getVariable("controllerSys");
		matlab::data::CellArray numArrayCell = MatlabAPI::MatlabEngine::getProperty(controllerVar, u"Numerator").getAPIArray();
		matlab::data::CellArray denArrayCell = MatlabAPI::MatlabEngine::getProperty(controllerVar, u"Denominator").getAPIArray();

		matlab::data::TypedArray<double> numArray = numArrayCell[0];
		matlab::data::TypedArray<double> denArray = denArrayCell[0];

		std::vector<double> numVec;
		std::vector<double> denVec;
		for (double el : numArray)
			numVec.push_back(el);
		for (double el : denArray)
			denVec.push_back(el);
		if (numVec.size() > 0 && denVec.size() > 0)
		{
			//setControllerTF(numVec, denVec);
			QString numStr = "";
			for (double v : numVec)
				numStr += QString::number(v) + " ";
			QString denStr = "";
			for (double v : denVec)
				denStr += QString::number(v) + " ";
			m_controllerNumeratorEdit->setText(numStr);
			m_controllerDenominatorEdit->setText(denStr);
			onControllerInputTFChanged();
		}
		else
		{
			m_controllerTFLabel->setText("Controller TF: Invalid input in Matlab variable \"sys\"");
		}
	}
	catch (const std::exception& e)
	{
		m_controllerTFLabel->setText("Controller TF: Error loading Matlab variable \"controllerSys\"");
	}
}

void BallOnBeam::onLoadSystemFromMatlabClicked()
{
	try {
		MatlabAPI::MatlabArray* plant = MatlabAPI::MatlabEngine::getVariable("plantSys");

		MatlabAPI::MatlabArray aArray = MatlabAPI::MatlabEngine::getProperty(plant, u"A");
		MatlabAPI::MatlabArray bArray = MatlabAPI::MatlabEngine::getProperty(plant, u"B");
		MatlabAPI::MatlabArray cArray = MatlabAPI::MatlabEngine::getProperty(plant, u"C");
		MatlabAPI::MatlabArray dArray = MatlabAPI::MatlabEngine::getProperty(plant, u"D");




		MatlabAPI::Matrix A(&aArray);
		MatlabAPI::Matrix B(&bArray);
		MatlabAPI::Matrix C(&cArray);
		MatlabAPI::Matrix D(&dArray);

		MatlabAPI::StateSpaceModel* newModel = new MatlabAPI::StateSpaceModel(A, B, C, D, x0, getPhysicsDeltaT(), m_c2dMethod);
		delete m_model;
		m_model = newModel;
		info("Loaded plant model from Matlab variable \"plantSys\".");
	}
	catch (const std::exception& e)
	{
		error("Error loading Matlab variable \"plantSys\".");
	}
}