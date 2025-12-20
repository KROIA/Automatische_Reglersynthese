#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <iostream>
#include <QCloseEvent>
#include <QDebug>
#include <QDir>
#include <QColorDialog>
#include <windows.h>
//#include "MatlabEngine.hpp"
//#define ENABLE_SCREEN_CAPTURE



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
	// Init resources
	Q_INIT_RESOURCE(AutoTunerIcons);
    //MatlabApiLoader::instance(); // ensures DLLs are loaded
    ui->setupUi(this);
	
	//MatlabAPI::MatlabEngine::instantiate(u"MySession");

	

	m_updateTimer = new QTimer();
	connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::onUpdateTimerFinished);
	m_updateTimer->start(500); // Update every 500 ms


    setupScene();
	

	ui->autoTestBestPID_checkBox->setChecked(true);
	on_autoTestBestPID_checkBox_stateChanged(1);

	

#ifdef USE_DIFFERENTIAL_EVOLUTION_SOLVER
	m_multiRunColor1 = sf::Color(232, 235, 35); // Yellow
	m_multiRunColor2 = sf::Color(232, 134, 21); // Orange
#endif

	ui->multiRunColor1_pushButton->setStyleSheet(QString("background-color: %1").arg(QColor::fromRgba(m_multiRunColor1.toInteger()).name()));
	ui->multiRunColor2_pushButton->setStyleSheet(QString("background-color: %1").arg(QColor::fromRgba(m_multiRunColor2.toInteger()).name()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_reset_pushButton_clicked()
{
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{
		problem->resetPopulation();
		bool ok;
		double learningAmount = ui->learningAmount_lineEdit->text().toDouble(&ok);
		if (ok)
		{
			problem->setLearningRate(learningAmount);
		}
	}
	MotorWithMassIdentification* motorWithMassProblem = m_scene->getMotorWithMassIdentification();
	if (motorWithMassProblem)
	{
		motorWithMassProblem->stopOptimization();
		motorWithMassProblem->startOptimization();
	}
}
void MainWindow::on_generateStepSequence_pushButton_clicked()
{
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{
		problem->createNewStepSequence();
	}
}
void MainWindow::on_learningAmount_lineEdit_editingFinished()
{
	bool ok;
	double learningAmount = ui->learningAmount_lineEdit->text().toDouble(&ok);
	if (ok)
	{
		PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
		if (problem)
		{
			problem->setLearningRate(learningAmount);
		}
		MotorWithMassIdentification* motorWithMassProblem = m_scene->getMotorWithMassIdentification();
		if (motorWithMassProblem)
		{
			motorWithMassProblem->setLearningRate(learningAmount);
		}
	}
	else
	{
		qDebug() << "Invalid learning amount input";
	}
}
void MainWindow::on_learningRateDecay_checkBox_stateChanged(int arg1)
{
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{
		problem->enableLearningRateDecay(ui->learningRateDecay_checkBox->isChecked());
	}
}
void MainWindow::on_autoTestBestPID_checkBox_stateChanged(int arg1)
{
	if (ui->autoTestBestPID_checkBox->isChecked())
	{
		if (!m_autoTestTimer)
		{
			m_autoTestTimer = new QTimer(this);
			connect(m_autoTestTimer, &QTimer::timeout, this, &MainWindow::onAutoTestTimerFinished);
			m_autoTestTimer->start(100); // Adjust the interval as needed
		}
	}
	else
	{
		m_autoTestTimer->stop();
		delete m_autoTestTimer;
		m_autoTestTimer = nullptr;
	}
}
void MainWindow::on_parametersColor_pushButton_clicked()
{
	QColor color = QColorDialog::getColor(Qt::white, this, "Choose Color");
	if (color.isValid()) {
		// use the chosen color
		sf::Color col(
			static_cast<sf::Uint8>(color.red()),
			static_cast<sf::Uint8>(color.green()),
			static_cast<sf::Uint8>(color.blue()),
			static_cast<sf::Uint8>(color.alpha())
		);
		PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
		if (problem)
		{
			problem->setResultsParameterColors(col);
		}

		// Visuzalize the color to the label ui->parametersColor_label
		ui->colorView_label->setStyleSheet(QString("background-color: %1").arg(color.name()));

	}
}
void MainWindow::on_learnCurveExport_pushButton_clicked()
{
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{
		std::string fileName = ui->learnCurveFile_lineEdit->text().toStdString();
		problem->saveResultsToFile(fileName);
	}
}
void MainWindow::on_multiRun_pushButton_clicked()
{
	if (m_isMultiRunning)
		return;
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{
		m_isMultiRunning = true;
		m_multiRunCount = ui->multiRunCount_spinBox->value();
		m_outputFolder = ui->learnCurveFile_lineEdit->text().toStdString();
		//#ifdef USE_DIFFERENTIAL_EVOLUTION_SOLVER
		//	problem->setTargetEpoch(200);
		//#else
		//	problem->setTargetEpoch(1000);
		//#endif
			/*QDir dir(QString::fromStdString(m_outputFolder));
			if (!dir.exists())
			{
				dir.mkpath(".");
			}*/
		problem->setResultsLearningHistoryEnableColumn(false, true, false);



		on_reset_pushButton_clicked();

		//size_t stepResponseSignalCount = problem->getResultsStepResponseSingalsCount();
		//for (size_t i = 0; i < stepResponseSignalCount; ++i)
		//{
		//	sf::Color colorGradient = QSFML::Color::lerpLinear(m_multiRunColor1, m_multiRunColor2,
		//		static_cast<float>(i) / static_cast<float>(stepResponseSignalCount - 1));
		//	problem->setResultsStepResponseSignalColor(i, colorGradient);
		//}

		problem->setResultsStepResponseSignalEnabled(0, false);
		problem->setResultsStepResponseSignalEnabled(1, false);
		problem->setResultsStepResponseSignalEnabled(2, false);
		problem->setResultsStepResponseSignalEnabled(3, false);


		problem->setResultsLearningHistoryColors(m_multiRunColor1);
		problem->setResultsParameterColors(m_multiRunColor1);

		qDebug() << "Starting Multi-Run " << m_currentMultiRun + 1;
	}
}
void MainWindow::on_multiRunColor1_pushButton_clicked()
{
	QColor color = QColorDialog::getColor(Qt::white, this, "Choose Color");
	if (color.isValid()) {
		// use the chosen color
		sf::Color col(
			static_cast<sf::Uint8>(color.red()),
			static_cast<sf::Uint8>(color.green()),
			static_cast<sf::Uint8>(color.blue()),
			static_cast<sf::Uint8>(color.alpha())
		);

		m_multiRunColor1 = col;

		// Visuzalize the color to the label ui->parametersColor_label
		ui->multiRunColor1_pushButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
	}
}
void MainWindow::on_multiRunColor2_pushButton_clicked()
{
	QColor color = QColorDialog::getColor(Qt::white, this, "Choose Color");
	if (color.isValid()) {
		// use the chosen color
		sf::Color col(
			static_cast<sf::Uint8>(color.red()),
			static_cast<sf::Uint8>(color.green()),
			static_cast<sf::Uint8>(color.blue()),
			static_cast<sf::Uint8>(color.alpha())
		);

		m_multiRunColor2 = col;

		// Visuzalize the color to the label ui->parametersColor_label
		ui->multiRunColor2_pushButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
	}
}
void MainWindow::onEpochReached(size_t epoch)
{
	if (!m_isMultiRunning)
		return;
	m_currentMultiRun++;
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{

		sf::Color colorGradient = m_multiRunColor1;

		if (m_multiRunCount > 1)
		{
			colorGradient = QSFML::Color::lerpLinear(m_multiRunColor1, m_multiRunColor2,
				static_cast<float>(m_currentMultiRun - 1) / static_cast<float>(m_multiRunCount - 1));
		}

		problem->setResultsLearningHistoryColors(colorGradient);
		problem->setResultsParameterColors(colorGradient);
		problem->setResultsStepResponseSignalColor(4, colorGradient);



		problem->saveResultsToFile(m_outputFolder + "/Result_" + std::to_string(m_currentMultiRun));
		if (m_multiRunCount <= m_currentMultiRun)
		{
			m_isMultiRunning = false;
			m_currentMultiRun = 0;
			qDebug() << "Multi-Run finished.";
			return;
		}
		qDebug() << "Starting Multi-Run " << m_currentMultiRun + 1;

		//if (m_currentMultiRun % 5 == 0)
		//{
		//	problem->setupPopulation(problem->getPopulationSize()+10,
		//		0,0,0,0,0);
		//	sf::Color colorGradient = QSFML::Color::lerpLinear(m_multiRunColor1, m_multiRunColor2,
		//		static_cast<float>(m_currentMultiRun - 1) / static_cast<float>(m_multiRunCount - 1));
		//	problem->setResultsLearningHistoryColors(colorGradient);
		//	problem->setResultsParameterColors(colorGradient);
		//}
		on_reset_pushButton_clicked();

	}

}
void MainWindow::on_testBestPID_pushButton_clicked()
{
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{
		problem->testBestAgent();
	}
}

void MainWindow::on_errorIntegral_verticalSlider_valueChanged(int value)
{
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{
		double factor = static_cast<double>(value) / 10.0;
		problem->setTuningGoalParameter_ErrorIntegralWeight(factor);
		ui->errorIntegral_label->setText(QString::number(factor));
		if(ui->restartOnChange_checkBox->isChecked())
		{
			on_reset_pushButton_clicked();
		}
	}
}
void MainWindow::on_actuatorAction_verticalSlider_valueChanged(int value)
{
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{
		double factor = static_cast<double>(value) / 1000.0;
		problem->setTuningGoalParameter_ActuatorEffortWeight(factor);
		ui->actuatorAction_label->setText(QString::number(factor));
		if (ui->restartOnChange_checkBox->isChecked())
		{
			on_reset_pushButton_clicked();
		}
	}
}
void MainWindow::on_overshoot_verticalSlider_valueChanged(int value)
{
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{
		double factor = static_cast<double>(value);
		problem->setTuningGoalParameter_OvershootWeight(factor);
		ui->overshoot_label->setText(QString::number(factor));
		if (ui->restartOnChange_checkBox->isChecked())
		{
			on_reset_pushButton_clicked();
		}
	}
}
void MainWindow::on_gainMargin_verticalSlider_valueChanged(int value)
{
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{
		double factor = static_cast<double>(value);
		problem->setTuningGoalParameter_GainMarginWeight(factor);
		ui->gainMargin_label->setText(QString::number(factor));
		if (ui->restartOnChange_checkBox->isChecked())
		{
			on_reset_pushButton_clicked();
		}
	}
}
void MainWindow::on_phaseMargin_verticalSlider_valueChanged(int value)
{
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{
		double factor = static_cast<double>(value);
		problem->setTuningGoalParameter_PhaseMarginWeight(factor);
		ui->phaseMargin_label->setText(QString::number(factor));
		if (ui->restartOnChange_checkBox->isChecked())
		{
			on_reset_pushButton_clicked();
		}
	}
}
void MainWindow::onAutoTestTimerFinished()
{
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{
		problem->testBestAgent();
	}
}
void MainWindow::onUpdateTimerFinished()
{
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{
		auto parameters = problem->getBestParameters();
		if (parameters.size() >= 3)
		{
			ui->kp_label->setText(QString::number(parameters[0]));
			ui->ki_label->setText(QString::number(parameters[1]));
			ui->kd_label->setText(QString::number(parameters[2]));
		}

		if (m_scrollAreaLabels.size() < parameters.size())
		{
			// Create labels for additional parameters
			for (size_t i = m_scrollAreaLabels.size(); i < parameters.size(); ++i)
			{
				QLabel* label = new QLabel(this);
				label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
				label->setText(QString("Param %1: %2").arg(i).arg(parameters[i]));
				// enable selectabel by mouse
				label->setTextInteractionFlags(Qt::TextSelectableByMouse);
				ui->scrollAreaWidgetContents->layout()->addWidget(label);
				m_scrollAreaLabels.push_back(label);
			}

			// Set size of scroll area contents
			ui->scrollArea->setMinimumHeight(static_cast<int>(parameters.size() * 25));
		}
		// Update labels with current parameter values§
		for (size_t i = 0; i < parameters.size(); ++i)
		{
			m_scrollAreaLabels[i]->setText(QString("Param %1: %2").arg(i).arg(parameters[i]));
		}
		ui->learningAmount_label->setText(QString::number(problem->getLearningRate()));
	}
}

void MainWindow::setupScene()
{
	m_scene = new Scene();
	m_scene->setupScene(ui->scene_frame);
	PIDTuningProblem* problem = m_scene->getPIDTuningProblem();
	if (problem)
	{
		connect(problem, &PIDTuningProblem::targetEpochReached, this, &MainWindow::onEpochReached);

		ui->errorIntegral_label->setText(QString::number(problem->getTuningGoalParameter_ErrorIntegralWeight()));
		ui->errorIntegral_verticalSlider->setValue(static_cast<int>(problem->getTuningGoalParameter_ErrorIntegralWeight() * 10.0));
		ui->actuatorAction_label->setText(QString::number(problem->getTuningGoalParameter_ActuatorEffortWeight()));
		ui->actuatorAction_verticalSlider->setValue(static_cast<int>(problem->getTuningGoalParameter_ActuatorEffortWeight() * 1000.0));
		ui->overshoot_label->setText(QString::number(problem->getTuningGoalParameter_OvershootWeight()));
		ui->overshoot_verticalSlider->setValue(static_cast<int>(problem->getTuningGoalParameter_OvershootWeight()));
		ui->gainMargin_label->setText(QString::number(problem->getTuningGoalParameter_GainMarginWeight()));
		ui->gainMargin_verticalSlider->setValue(static_cast<int>(problem->getTuningGoalParameter_GainMarginWeight()));
		ui->phaseMargin_label->setText(QString::number(problem->getTuningGoalParameter_PhaseMarginWeight()));
		ui->phaseMargin_verticalSlider->setValue(static_cast<int>(problem->getTuningGoalParameter_PhaseMarginWeight()));

		ui->learningRateDecay_checkBox->setChecked(problem->isLearningRateDecayEnabled());
		ui->learningAmount_lineEdit->setText(QString::number(problem->getLearningAmount()));
	}
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	QMainWindow::closeEvent(event);
}