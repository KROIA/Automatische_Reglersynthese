#include "scene/Objects/DCMotorProblem.h"
#include <QDir>
#include <fstream>


#ifdef USE_DIFFERENTIAL_EVOLUTION_SOLVER
static const std::vector<sf::Color> csvLineColor = {
		sf::Color(0x00B4D8FF),
		sf::Color(0x90E0EFFF),
		sf::Color(0x0077B6FF)
};
#else
static const std::vector<sf::Color> csvLineColor = {
		sf::Color(0x52B788FF),
		sf::Color(0x95D5B2FF),
		sf::Color(0x2D6A4FFF)
};
#endif

static double map(double value, double inMin, double inMax, double outMin, double outMax)
{
	return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

DCMotorProblem::DCMotorProblem(const SetupSettings& setupSettings, 
	const std::string& name,
	GameObject* parent)
	: PIDTuningProblem(name, parent)
	, m_setupSettings(setupSettings)
	, m_testSystem(setupSettings)
{

	m_zieglerNicholsComponent = new AutoTuner::ZieglerNichols("ZieglerNichols");
	addComponent(m_zieglerNicholsComponent);

	m_chartViewComponent = new AutoTuner::ChartViewComponent("DC Motor simulation");
	addComponent(m_chartViewComponent);

	m_learningRate = m_setupSettings.startLearningRate;
	switch (m_setupSettings.solverType)
	{
		case SolverType::GeneticAlgorithm:
		{
			AutoTuner::GeneticSolver* gs = new AutoTuner::GeneticSolver();
			gs->setMutationAmount(m_setupSettings.startLearningRate);
			m_solverObject = gs;
			break;
		}
		case SolverType::DifferentialEvolution:
		{
			AutoTuner::DifferentialEvolutionSolver* ds = new AutoTuner::DifferentialEvolutionSolver();
			ds->setMutationAmount(m_setupSettings.startLearningRate);
			m_solverObject = ds;
			break;
		}
		default:
		{
			break;
		}

	}
//#ifdef USE_GENTIC_SOLVER
//	AutoTuner::GeneticSolver *gs = new AutoTuner::GeneticSolver();
//	gs->setMutationAmount(m_learningRate);
//	m_solverObject = gs;
//#endif
//#ifdef USE_DIFFERENTIAL_EVOLUTION_SOLVER
//	AutoTuner::DifferentialEvolutionSolver *ds = new AutoTuner::DifferentialEvolutionSolver();
//	ds->setMutationAmount(s_startLearningRate);
//	m_solverObject = ds;
//#endif
	if(m_setupSettings.useMinimizingScore)
		m_solverObject->setOptimizingDirection(AutoTuner::Solver::OptimizingDirection::Minimize);
	else
		m_solverObject->setOptimizingDirection(AutoTuner::Solver::OptimizingDirection::Maximize);

//#ifdef GENETIC_USE_MINIMIZING_SCORE
//	m_solverObject->setOptimizingDirection(AutoTuner::Solver::OptimizingDirection::Minimize);
//#else
//	m_solverObject->setOptimizingDirection(AutoTuner::Solver::OptimizingDirection::Maximize);
//#endif
	addChild(m_solverObject);

	
	m_solverObject->setParametersToColorFunc(
		[](const std::vector<double>& parameters)
		{
			double maxValue = std::max({ std::abs(parameters[0]), std::abs(parameters[1]), std::abs(parameters[2]) });
			// Map Kp to Red, Ki to Green, Kd to Blue
			int r = static_cast<int>(map(parameters[0], -maxValue, maxValue, 0, 255));
			int g = static_cast<int>(map(parameters[1], -maxValue, maxValue, 0, 255));
			int b = static_cast<int>(map(parameters[2], -maxValue, maxValue, 0, 255));
			return sf::Color(r, g, b);
		}
	);
	m_solverObject->setParametersTestFunc(std::bind(&DCMotorProblem::agentTestFunction, this, std::placeholders::_1, std::placeholders::_2));
	m_solverObject->setScorePartsLabels({ "error", "pidOutChange", "Overshoot", "GainMargin", "PhaseMargin" });
	
	//geneticSolver->setTargetScore(AutoTuner::GeneticSolver::TargetScore::Minimize);
	
	//m_dcMotorSystem.setIntegrationSolver(AutoTuner::TimeBasedSystem::IntegrationSolver::Bilinear);
	//testPID(17.724747459, 23.814575, 0.0388);

	//createNewStepSequence();
	//m_disturbanceData = { {3.0, 0.05}, {5.0, 0.1}, { 7.0, 0.15 }, { 8.0, 0.0 } };
	

#ifndef DYNAMIC_STEP_SEQUENCE
	m_learningStepData = { {0.0,0.0}, {0.5,1.0}, {1.0,0.0}, {1.5,5.0}, {2.5,0.0}, {3.5,8.0}, {4.5,0.0}, {5.5,9.0}, {6.25,0.0}, {6.5,2.0}, {7,0.0}, {7.5,2.0}, {8,0.0}, {8.5,2.0}, {9,0.0},
		{10.0, 5.0}, {15, 7.0}, {20.5, 7.0}
	};
	m_disturbanceData = {
		{0 , 0},
		{11, 1},
		{12, 2},
		{13, 0},
		{14, 2},
		{14.5, 0},
		{15, 2},
		{15.5, 0},
		{16, 2},
		{16.5, 0},
		{17, 2},
		{17.5, 0},
		{18, 2},
		{18.5, 0},
		{19, 2},
		{19.5, 0},
	};
	m_learningDisturbanceData = m_disturbanceData;
#endif
	if (m_learningStepData.size() > 0)
		m_stepData = m_learningStepData;

	// Print time and signal values for SIMULINK copy paste in to the Repeating Sequence block
	printSignalSequenceToConsole("Step Signal", m_stepData);
	printSignalSequenceToConsole("Disturbance Signal", m_disturbanceData);

	
	
	setCSVHeader();

	m_nyquistPlotComponent = new AutoTuner::NyquistPlotComponent("NyquistPlotComponent");
	addComponent(m_nyquistPlotComponent);
	m_frequencyResponse.setSettelingTime(5.0);
	//m_frequencyResponse.setOutputIndex(0); // Position output
	//addDrawFunction(
	//	[this](const GameObject&, sf::RenderTarget& target, sf::RenderStates)
	//	{
	//		auto responseData = m_frequencyResponse.getResponse(m_testSystem, 0.1, 1000.0);
	//		m_frequencyResponse.drawNyquistPlot(responseData, "DC Motor with Spring - Nyquist Plot");
	//	}
	//);
}

void DCMotorProblem::onAwake()
{
	testCustomPID();
	precalculatePIDWithZieglerNichols();
}
void DCMotorProblem::precalculatePIDWithZieglerNichols()
{
	std::vector<float> timeData;
	std::vector<float> responseData;
	DCMotorSystem dcMotorSystem;
	dcMotorSystem.setIntegrationSolver(AutoTuner::TimeBasedSystem::IntegrationSolver::Bilinear);

	float dt = m_setupSettings.deltaTime;
	for (float x = 0; x < 1; x += dt)
	{
		timeData.push_back(x);
		responseData.push_back(static_cast<float>(dcMotorSystem.getAngularVelocity()));

		dcMotorSystem.setInputs(1.0, 0.0);
		dcMotorSystem.update(dt);
	}
	m_zieglerNicholsComponent->setStepResponse(timeData, responseData);
	auto pidParams = m_zieglerNicholsComponent->getPID_Parameters(AutoTuner::ZieglerNichols::Method::ZN_PID_controller);
	pidParams.Kd = m_setupSettings.defaultKp; // 2.38885601249613;
	pidParams.Ki = m_setupSettings.defaultKi; // 22.1216868792194;
	pidParams.Kp = m_setupSettings.defaultKd; // 0.0231853375201583;
	setupPopulation(m_setupSettings.agentCount, pidParams.Kp, pidParams.Ki, pidParams.Kd, 
		m_setupSettings.defaultPIDISaturation, m_setupSettings.startAreaRange);
	
	auto tmp = agentTestFunction({ 11.3, 0, 0, 0 },0);
	// tmp = agentTestFunction({ 7.21,111.21,0,0 });
	qDebug() << "Test PID Score Components: ";
	for (const auto& val : tmp)
	{
		qDebug() << val << ", ";
	}
}
void DCMotorProblem::setupPopulation(size_t populationSize, double kp, double ki, double kd, double integralSatturation, double areaRange)
{
	//AutoTuner::GeneticSolver* geneticSolver = dynamic_cast<AutoTuner::GeneticSolver*>(m_solverObject);
	if (m_solverObject)
	{
		m_setupSettings.agentCount = populationSize;
		std::vector<std::vector<double>> initialPopulation;
		for (size_t i = 0; i < populationSize; ++i)
		{
			std::vector<double> individual;
			if(m_setupSettings.optimizeKp)
				individual.push_back(kp + AutoTuner::Solver::getRandomDouble(-areaRange, areaRange)); // Kp

			if (m_setupSettings.optimizeKi)
				individual.push_back(ki + AutoTuner::Solver::getRandomDouble(-areaRange, areaRange)); // Ki

			if (m_setupSettings.optimizeKd)
				individual.push_back(kd + AutoTuner::Solver::getRandomDouble(-areaRange, areaRange));  // Kd
			if (m_setupSettings.optimizeKn && m_setupSettings.useKn)
				individual.push_back(m_setupSettings.defaultKn + AutoTuner::Solver::getRandomDouble(-areaRange, areaRange)); // Kn
			if (m_setupSettings.optimizeIntegralSaturation)
				individual.push_back(m_setupSettings.defaultPIDISaturation * AutoTuner::Solver::getRandomDouble(0, 2 * areaRange));
			if (m_setupSettings.optimizeAntiWindupBackCalculationConstant)
				individual.push_back(m_setupSettings.defaultPIDAntiWindupBackCalculationConstant + AutoTuner::Solver::getRandomDouble(-areaRange, areaRange));




//#ifdef PARAMETERLIST_ENABLE_KP
//			individual.push_back(kp + AutoTuner::Solver::getRandomDouble(-areaRange, areaRange)); // Kp
//#endif
//#ifdef PARAMETERLIST_ENABLE_KI
//			individual.push_back(ki + AutoTuner::Solver::getRandomDouble(-areaRange, areaRange)); // Ki
//#endif
//#ifdef PARAMETERLIST_ENABLE_KD
//			individual.push_back(kd + AutoTuner::Solver::getRandomDouble(-areaRange, areaRange));  // Kd
//#endif
//#ifdef PARAMETERLIST_ENABLE_KN
//			individual.push_back(s_defaultKn + AutoTuner::Solver::getRandomDouble(-areaRange, areaRange)); // Kn
//#endif
//#ifdef PARAMETERLIST_ENABLE_INTEGRAL_SATURATION
//			individual.push_back(s_defaultPIDISaturation * AutoTuner::Solver::getRandomDouble(0,2 * areaRange));
//#endif
//#ifdef PARAMETERLIST_ENABLE_ANTI_WINDUP_BACK_CALCULATION_CONSTANT
//			individual.push_back(s_defaultPIDAntiWindupBackCalculationConstant + AutoTuner::Solver::getRandomDouble(-areaRange, areaRange));
//#endif
			initialPopulation.push_back(individual);
		}
		AutoTuner::GeneticSolver* geneticSolver = dynamic_cast<AutoTuner::GeneticSolver*>(m_solverObject);
		if (geneticSolver)
		{
			if (geneticSolver->isThreadsBusy())
			{
				qDebug() << "Cannot reset population while solver is busy!";
				return;
			}
		}
		m_solverObject->setInitialParameters(initialPopulation);
		m_solverObject->clearAlltimeBestParameters();
		testPID(initialPopulation[0]);
		//testPID({5,35.7,0,10});
	}
}

void DCMotorProblem::update()
{
	if (m_solverObject)
	{
#ifdef DYNAMIC_STEP_SEQUENCE
		if (m_learningStepData.size() == 0)
		{
			m_learningStepData.clear();
			m_learningStepData.resize(m_testRuns);
			for (size_t i = 0; i < m_testRuns; ++i)
				m_learningStepData[i] = generateRandomStepSequence(m_testSystem.getSystemInputLimit() * 0.8, 10, 0.5, 3.0, 5);
		}
#endif

		m_solverObject->test();
		m_solverObject->iterate();

		
		logCSVData();
		++m_epochCounter;
		if(m_setupSettings.targetEpochs <= m_epochCounter)
		{
			emit targetEpochReached(m_epochCounter);
		}

		AutoTuner::GeneticSolver* geneticSolver = dynamic_cast<AutoTuner::GeneticSolver*>(m_solverObject);
		if (geneticSolver)
		{
//#if defined(USE_GENTIC_SOLVER)
			if (m_setupSettings.useGeneticMutationRateDecay)
			{
				//AutoTuner::GeneticSolver* geneticSolver = dynamic_cast<AutoTuner::GeneticSolver*>(m_solverObject);
				//if (geneticSolver)
				//{
					m_learningRate *= m_setupSettings.learningRateDecay;
					geneticSolver->setMutationAmount(m_learningRate);
				//}
			}
		}
//#endif
	}
}

void DCMotorProblem::createNewStepSequence()
{
	m_stepData = generateRandomStepSequence(m_testSystem.getSystemInputLimit() * 0.8, 10, 0.5, 3.0, 5);
	

}
void DCMotorProblem::resetPopulation()
{
	if (m_solverObject)
	{
		//createNewStepSequence();
		if (m_learningStepData.size() > 0)
			m_stepData = m_learningStepData;

		double kp = (rand() % 20) - 10;
		double ki = (rand() % 20) - 10;
		double kd = (rand() % 20) - 10;
		
		//setupPopulation(kp, ki, kd,m_defaultPIDISatturation,  1);
		//testPID(kp, ki, kd, m_defaultPIDISatturation);

		setupPopulation(m_setupSettings.agentCount, 0, 0, 0, m_setupSettings.defaultPIDISaturation, m_setupSettings.startAreaRange);
		//testPID({ 0, 0, 0, s_defaultPIDISaturation });
		m_resultData.clearData();
		//m_csvExport.clearData();
		m_epochCounter = 0;
	}
}
void DCMotorProblem::testBestAgent()
{
	if (m_solverObject)
	{
		auto parameters = m_solverObject->getBestParameters();
		testPID(parameters);
	}
}
void DCMotorProblem::setLearningAmount(double learningAmount)
{
	if (m_solverObject)
	{
		m_solverObject->setMutationAmount(learningAmount);
	}
}
double DCMotorProblem::getLearningAmount() const
{
	if(m_solverObject)
	{
		return m_solverObject->getMutationAmount();
	}
	return 0.0;
}
void DCMotorProblem::setLearningRate(double learningRate)
{
	AutoTuner::GeneticSolver* geneticSolver = dynamic_cast<AutoTuner::GeneticSolver*>(m_solverObject);
	m_learningRate = learningRate;
	if (geneticSolver)
	{
		geneticSolver->setMutationAmount(learningRate);
		return;
	}
	AutoTuner::DifferentialEvolutionSolver* deSolver = dynamic_cast<AutoTuner::DifferentialEvolutionSolver*>(m_solverObject);
	if (deSolver)
	{
		deSolver->setMutationAmount(learningRate);
	}	
}
double DCMotorProblem::getLearningRate() const
{
	return m_learningRate;
}
void DCMotorProblem::saveResultsToFile(const std::string& resultName)
{
	testPID(getBestParameters(), &m_resultData);
	m_resultData.resultName = resultName;
	saveResultDataToFile(m_resultData, "Results");
	//m_csvExport.exportToFile("Plots/"+filename);
}
void DCMotorProblem::setResultsLearningHistoryColors(const sf::Color& color)
{
	setResultsLearningHistoryColors(color, color, color);
}
void DCMotorProblem::setResultsLearningHistoryColors(const sf::Color& averageColor, const sf::Color& bestColor, const sf::Color& worstColor)
{
	m_resultData.learningHistory.averageScore.lineColor = averageColor;
	m_resultData.learningHistory.bestScore.lineColor = bestColor;
	m_resultData.learningHistory.worstScore.lineColor = worstColor;
}
void DCMotorProblem::setResultsParameterColors(const sf::Color& color)
{
	for (auto& param : m_resultData.parameterChanges.parameters)
	{
		param.lineColor = color;
	}
}
void DCMotorProblem::setResultsParameterColors(const std::vector<sf::Color>& colors)
{
	for (size_t i = 0; i < m_resultData.parameterChanges.parameters.size(); ++i)
	{
		if (i < colors.size())
		{
			m_resultData.parameterChanges.parameters[i].lineColor = colors[i];
		}
	}
}
size_t DCMotorProblem::getParameterCount() const
{
	if (m_solverObject)
	{
		return m_solverObject->getBestParameters().size();
	}
	return 0;
}
void DCMotorProblem::setResultsParameterEnabled(size_t index, bool enabled)
{
	if (index < m_resultData.parameterChanges.parameters.size())
	{
		m_resultData.parameterChanges.parameters[index].isEnabled = enabled;
	}
}
void DCMotorProblem::setResultsLearningHistoryEnableColumn(bool enableMin, bool enableAverage, bool enableMax)
{
	m_resultData.learningHistory.worstScore.isEnabled = enableMin;
	m_resultData.learningHistory.averageScore.isEnabled = enableAverage;
	m_resultData.learningHistory.bestScore.isEnabled = enableMax;
}
size_t DCMotorProblem::getResultsStepResponseSingalsCount() const
{
	return m_resultData.stepResponse.responseSignals.size();
}
void DCMotorProblem::setResultsStepResponseSignalColor(size_t index, const sf::Color& color)
{
	if (index < m_resultData.stepResponse.responseSignals.size())
	{
		m_resultData.stepResponse.responseSignals[index].lineColor = color;
	}
}
void DCMotorProblem::setResultsStepResponseSignalName(size_t index, const std::string& name)
{
	if (index < m_resultData.stepResponse.responseSignals.size())
	{
		m_resultData.stepResponse.responseSignals[index].name = name;
	}
}
void DCMotorProblem::setResultsStepResponseSignalLineStyle(size_t index, AutoTuner::CSVExport::LineStyle style)
{
	if (index < m_resultData.stepResponse.responseSignals.size())
	{
		m_resultData.stepResponse.responseSignals[index].lineStyle = style;
	}
}
void DCMotorProblem::setResultsStepResponseSignalLineThickness(size_t index, int thickness)
{
	if (index < m_resultData.stepResponse.responseSignals.size())
	{
		m_resultData.stepResponse.responseSignals[index].lineThickness = thickness;
	}
}
void DCMotorProblem::setResultsStepResponseSignalEnabled(size_t index, bool enabled)
{
	if (index < m_resultData.stepResponse.responseSignals.size())
	{
		m_resultData.stepResponse.responseSignals[index].isEnabled = enabled;
	}
}
const sf::Color& DCMotorProblem::getResultsStepResponseSignalColor(size_t index) const
{
	if (index < m_resultData.stepResponse.responseSignals.size())
	{
		return m_resultData.stepResponse.responseSignals[index].lineColor;
	}
	static const sf::Color defaultColor = sf::Color::White;
	return defaultColor;
}
const std::string& DCMotorProblem::getResultsStepResponseSignalName(size_t index) const
{
	if (index < m_resultData.stepResponse.responseSignals.size())
	{
		return m_resultData.stepResponse.responseSignals[index].name;
	}
	static const std::string emptyString = "";
	return emptyString;
}
const AutoTuner::CSVExport::LineStyle DCMotorProblem::getResultsStepResponseSignalLineStyle(size_t index) const
{
	if (index < m_resultData.stepResponse.responseSignals.size())
	{
		return m_resultData.stepResponse.responseSignals[index].lineStyle;
	}
	return AutoTuner::CSVExport::LineStyle::Solid;
}
int DCMotorProblem::getResultsStepResponseSignalLineThickness(size_t index) const
{
	if (index < m_resultData.stepResponse.responseSignals.size())
	{
		return m_resultData.stepResponse.responseSignals[index].lineThickness;
	}
	return 1;
}
bool DCMotorProblem::isResultsStepResponseSignalEnabled(size_t index) const
{
	if (index < m_resultData.stepResponse.responseSignals.size())
	{
		return m_resultData.stepResponse.responseSignals[index].isEnabled;
	}
	return false;
}
std::vector<double> DCMotorProblem::getBestParameters() const
{
	if (m_solverObject)
	{
		return m_solverObject->getBestParameters();
	}
	return {};
}
std::vector<QString> DCMotorProblem::getParameterLabels() const
{
	std::vector<QString> labels;
	if (m_setupSettings.optimizeKp)
		labels.push_back("Kp");
	if (m_setupSettings.optimizeKi)
		labels.push_back("Ki");
	if (m_setupSettings.optimizeKd)
		labels.push_back("Kd");
	if (m_setupSettings.useKn && m_setupSettings.optimizeKn)
		labels.push_back("Kn");
	if (m_setupSettings.optimizeIntegralSaturation)
		labels.push_back("IntegralSaturation");
	if (m_setupSettings.optimizeAntiWindupBackCalculationConstant)
		labels.push_back("AntiWindupBackCalculationConstant");
	return labels;
}

void DCMotorProblem::testPID(const std::vector<double>& params, ResultData* resultContainer)
{
	if (params.size() == 0)
		return;
	//std::vector<double> timeData;
	//std::vector<double> responseData;
	//AutoTuner::PID pidController(6.48, 26.8, -1.09, 0.547);
	//AutoTuner::PID pidController(6.48, 26.8, -1.09);
	double dt = m_setupSettings.deltaTime;

	m_testSystem.reset();
	m_testSystem.setParameters(params);
	m_chartViewComponent->clearPlotData();

	AutoTuner::ChartViewComponent::PlotData rPlotData("r");
	AutoTuner::ChartViewComponent::PlotData ePlotData("e");
	AutoTuner::ChartViewComponent::PlotData uPlotData("u");
	AutoTuner::ChartViewComponent::PlotData yPlotData("y");
	AutoTuner::ChartViewComponent::PlotData dPlotData("d");


//#ifdef DISABLE_ERROR_INTEGRAL_WHEN_SATURATED
	AutoTuner::ChartViewComponent::PlotData sumErrPlotData("sumErrorEnabled");
//#endif

	double r = 0;
	double disturbance = 0;
	size_t counter = 0;
	size_t nextStepIndex = 0;
	size_t nextDisturbanceIndex = 0;
	double actuatorLimit = m_testSystem.getActuatorInputLimit();
	double systemInputLimit = m_testSystem.getSystemInputLimit();

	const auto& stepData = m_stepData;
	//const std::vector<sf::Vector2<double>> stepData = { {0.0,1.0}, {5.0,-1.0} };
	//AutoTuner::PID pidController;
	//pidController.setParameters(0,1,0);
	//pidController.setIntegralSatturationLimit(10);
	//pidController.setOutputSaturationLimits(0, 3);
	//pidController.setAntiWindupMethod(AutoTuner::PID::AntiWindupMethod::BackCalculation);
	//pidController.setAntiWindupBackCalculationConstant(0.1);
	double lastR = 0;
	int rWasRising = 0;
	for (double t = 0; t < m_setupSettings.endTime; t += dt)
	{
		// Apply step changes
		if (nextStepIndex < stepData.size())
		{
			if (t >= stepData[nextStepIndex].x)
			{
				r = stepData[nextStepIndex].y;
				if(r>lastR)
					rWasRising = 1;
				else
					rWasRising = 0;
				nextStepIndex++;
			}
		}
		if (nextDisturbanceIndex < m_disturbanceData.size())
		{
			if (t >= m_disturbanceData[nextDisturbanceIndex].x)
			{
				disturbance = m_disturbanceData[nextDisturbanceIndex].y;
				nextDisturbanceIndex++;
			}
		}



		m_testSystem.setInputSignals(r, disturbance);
		m_testSystem.update(dt);
		//pidController.setInputSignals(r);
		//pidController.update(dt);

		if (counter < (size_t)(t * 100))
		{
			counter++;
			ePlotData.addDataPoint(t, m_testSystem.getError());
			uPlotData.addDataPoint(t, m_testSystem.getPIDOutput());
			rPlotData.addDataPoint(t, m_testSystem.getReferenceInput());
			yPlotData.addDataPoint(t, m_testSystem.getOutput());
			dPlotData.addDataPoint(t, disturbance);

			//rPlotData.addDataPoint(t, r);
			//yPlotData.addDataPoint(t, pidController.getOutputs()[0]);

//#ifdef DISABLE_ERROR_INTEGRAL_WHEN_SATURATED
			
			// Check if the controller is not saturating
			//double error = m_testSystem.getError() / systemInputLimit;
			
			
			if (m_setupSettings.disableErrorIntegrationWhenSaturated)
			{
				double pidOutput = m_testSystem.getPIDOutput();
				double angularSpeed = m_testSystem.getOutput();
				bool isPositiveSaturated = pidOutput > actuatorLimit - 0.01;
				bool isLowerSaturated = pidOutput < 0.01;
				if (!((isPositiveSaturated && r > angularSpeed) || (isLowerSaturated && r < angularSpeed)))
				{
					// Penalize the integral of the error
					sumErrPlotData.addDataPoint(t, 10);
				}
				else
					sumErrPlotData.addDataPoint(t, 0);
			}


			//if ((lastR < angularSpeed) && (r < angularSpeed) && rWasRising)
			//{
			//	sumErrPlotData.addDataPoint(t, 10);
			//	rWasRising = 2;
			//}
			//else
			//{
			//	if(rWasRising == 2)
			//		rWasRising = 0;
			//	sumErrPlotData.addDataPoint(t, 0);
			//}

//#endif
		}
		lastR = r;
	}
	m_chartViewComponent->addPlotData(rPlotData);
	m_chartViewComponent->addPlotData(ePlotData);
	m_chartViewComponent->addPlotData(uPlotData);
	m_chartViewComponent->addPlotData(dPlotData);
	m_chartViewComponent->addPlotData(yPlotData);
	
//#ifdef DISABLE_ERROR_INTEGRAL_WHEN_SATURATED
	if (m_setupSettings.disableErrorIntegrationWhenSaturated)
		m_chartViewComponent->addPlotData(sumErrPlotData);
//#endif

	if (m_nyquistPlotComponent)
	{
		static int counter = 0;
		counter++;
		static AutoTuner::FrequencyResponse::FrequencyResponseData responseData;
		if (counter % 10 == 0)
		{
			counter = 0;
			responseData = m_frequencyResponse.getResponse(m_testSystem.getFeedForwardPart(), 
				m_setupSettings.nyquistBeginFreq, m_setupSettings.nyquistEndFreq);
			m_nyquistPlotComponent->setFrequencyResponse(responseData);
		}
	}


	if (resultContainer)
	{
		//resultContainer->stepResponse.time.data.clear();
		//resultContainer->stepResponse.responseSignals.data.clear();

		

	/*	ResultData::ColumnData rPlot("r", AutoTuner::CSVExport::LineStyle::Solid, 1, sf::Color(0xf2c94e));
		ResultData::ColumnData lPlot("l", AutoTuner::CSVExport::LineStyle::Solid, 1, sf::Color(0xeb3a23));
		ResultData::ColumnData ePlot("e", AutoTuner::CSVExport::LineStyle::Solid, 1, sf::Color(0xf2c94e));
#ifdef USE_DIFFERENTIAL_EVOLUTION_SOLVER
		ResultData::ColumnData uPlot("u", AutoTuner::CSVExport::LineStyle::Solid, 1, sf::Color(0x506ce6));
		ResultData::ColumnData yPlot("y", AutoTuner::CSVExport::LineStyle::Solid, 1, sf::Color(0x15297d));
#endif
#ifdef USE_GENTIC_SOLVER
		ResultData::ColumnData uPlot("u", AutoTuner::CSVExport::LineStyle::Solid, 1, sf::Color(0x63e0d8));
		ResultData::ColumnData yPlot("y", AutoTuner::CSVExport::LineStyle::Solid, 1, sf::Color(0x228a83));
#endif*/
		// Fill data
		const std::vector<double>& timetamps = rPlotData.getXData();
		const std::vector<double>& rPlotValues = rPlotData.getYData();
		const std::vector<double>& lPlotValues = dPlotData.getYData();
		const std::vector<double>& ePlotValues = ePlotData.getYData();
		const std::vector<double>& uPlotValues = uPlotData.getYData();
		const std::vector<double>& yPlotValues = yPlotData.getYData();

		resultContainer->stepResponse.time.data = timetamps;
		resultContainer->stepResponse.responseSignals[0].data = rPlotValues;
		resultContainer->stepResponse.responseSignals[1].data = lPlotValues;
		resultContainer->stepResponse.responseSignals[2].data = ePlotValues;
		resultContainer->stepResponse.responseSignals[3].data = uPlotValues;
		resultContainer->stepResponse.responseSignals[4].data = yPlotValues;
		std::vector<ResultData::ParameterData> parameterData;
		size_t paramIndexCounter = 0;

		if(m_setupSettings.optimizeKp)
			resultContainer->parameters[paramIndexCounter++].value = params[0];
		else
			resultContainer->parameters[paramIndexCounter++].value = m_setupSettings.defaultKp;

		if (m_setupSettings.optimizeKi)
			resultContainer->parameters[paramIndexCounter++].value = params[1];
		else
			resultContainer->parameters[paramIndexCounter++].value = m_setupSettings.defaultKi;

		if (m_setupSettings.optimizeKd)
			resultContainer->parameters[paramIndexCounter++].value = params[2];
		else
			resultContainer->parameters[paramIndexCounter++].value = m_setupSettings.defaultKd;

		if (m_setupSettings.useKn)
		{
			if (m_setupSettings.optimizeKn)
				resultContainer->parameters[paramIndexCounter++].value = params[3];
			else
				resultContainer->parameters[paramIndexCounter++].value = m_setupSettings.defaultKn;
		}

		if (m_setupSettings.optimizeIntegralSaturation)
			resultContainer->parameters[paramIndexCounter++].value = params[4];
		else
			resultContainer->parameters[paramIndexCounter++].value = m_setupSettings.defaultPIDISaturation;

		if (m_setupSettings.optimizeAntiWindupBackCalculationConstant)
			resultContainer->parameters[paramIndexCounter++].value = params[5];
		else
			resultContainer->parameters[paramIndexCounter++].value = m_setupSettings.defaultPIDAntiWindupBackCalculationConstant;

		/*
#ifdef PARAMETERLIST_ENABLE_KP
		resultContainer->parameters[paramIndexCounter++].value = params[0];
#else
		resultContainer->parameters[paramIndexCounter++].value = s_defaultKp;
#endif

#ifdef PARAMETERLIST_ENABLE_KI
		resultContainer->parameters[paramIndexCounter++].value = params[1];
#else
		resultContainer->parameters[paramIndexCounter++].value = s_defaultKi;
#endif

#ifdef PARAMETERLIST_ENABLE_KD
		resultContainer->parameters[paramIndexCounter++].value = params[2];
#else
		resultContainer->parameters[paramIndexCounter++].value = s_defaultKd;
#endif

#ifdef PARAMETERLIST_ENABLE_KN
		resultContainer->parameters[paramIndexCounter++].value = params[3];
#else
		resultContainer->parameters[paramIndexCounter++].value = s_defaultKn;
#endif




#ifdef PARAMETERLIST_ENABLE_INTEGRAL_SATURATION
		resultContainer->parameters[paramIndexCounter++].value = params[counter++];
#else
		resultContainer->parameters[paramIndexCounter++].value = s_defaultPIDISaturation;
#endif

#ifdef PARAMETERLIST_ENABLE_ANTI_WINDUP_BACK_CALCULATION_CONSTANT
		resultContainer->parameters[paramIndexCounter++].value = params[counter++];
#else
		resultContainer->parameters[paramIndexCounter++].value = s_defaultPIDAntiWindupBackCalculationConstant;
#endif*/
	}
}

std::vector<double> DCMotorProblem::agentTestFunction(const std::vector<double>& parameters, size_t agent)
{
	AT_GENERAL_PROFILING_FUNCTION(AT_COLOR_STAGE_2);
	double dt = m_setupSettings.deltaTime;
	double endTime = m_setupSettings.endTime;

	TestSystem agentSystem(m_setupSettings);
	agentSystem.reset();
	agentSystem.setParameters(parameters);

	double r = 0;
	double disturbance = 0;
	double lastPIDOutput = 0.0;

	double errorSum = 0.0;
	double overshootSum = 0.0;
	double pidOutChangeSum = 0.0;
	double lastR = 0;
	int rWasRising = 0;
	double actuatorLimit = agentSystem.getActuatorInputLimit();
	double systemInputLimit = agentSystem.getSystemInputLimit();


	std::vector<double> losses = { 0.0, 0.0, 0.0, 0.0, 0.0 };


	if (m_tuningGoalFactor_gainMargin != 0 || m_tuningGoalFactor_phaseMargin != 0)
	{
		AutoTuner::FrequencyResponse::FrequencyResponseData responseData = m_frequencyResponse.getResponse(agentSystem.getFeedForwardPart(), 
			m_setupSettings.nyquistBeginFreq, m_setupSettings.nyquistEndFreq);
		double gainMargin = responseData.gainMargin;
		double phaseMargin = responseData.phaseMargin;

		//losses[3] = std::abs(std::max(0.0, targetGainMargin - gainMargin)) * 10;
		//losses[4] = std::abs(std::max(0.0, targetPhaseMargin - phaseMargin)) * 100;

		losses[3] = std::abs(m_setupSettings.targetGainMargin - gainMargin) * m_tuningGoalFactor_gainMargin;
		losses[4] = std::abs(m_setupSettings.targetPhaseMargin - phaseMargin) * m_tuningGoalFactor_phaseMargin;
	}

	const std::vector<sf::Vector2<double>> &disturbanceData = m_learningDisturbanceData;
	const std::vector<sf::Vector2<double>>& stepData = m_learningStepData;



	


	size_t nextStepIndex = 0;
	size_t nextDisturbanceIdex = 0;
	for (double t = 0; t < endTime; t += dt)
	{
		// Apply step changes
		if (nextStepIndex < stepData.size())
		{
			if (t >= stepData[nextStepIndex].x)
			{
				r = stepData[nextStepIndex].y;
				if (r > lastR)
					rWasRising = 1;
				else
					rWasRising = 0;
				nextStepIndex++;
			}
		}

		if (nextDisturbanceIdex < disturbanceData.size())
		{
			if (t >= disturbanceData[nextDisturbanceIdex].x)
			{
				disturbance = disturbanceData[nextDisturbanceIdex].y;
				nextDisturbanceIdex++;
			}
		}
		
		agentSystem.setInputSignals(r, disturbance);
		agentSystem.update(dt);


		double pidOutput = agentSystem.getPIDOutput();
		double angularSpeed = agentSystem.getOutput();

		// Penalize large control changes
		double controllerEffortChange = std::abs(AutoTuner::TimeBasedSystem::getDifferentiated_backwardEuler(lastPIDOutput, pidOutput, dt));
		pidOutChangeSum += controllerEffortChange;
		lastPIDOutput = pidOutput;


		// Check if the controller is not saturating
		double error = agentSystem.getError() / systemInputLimit;
		if (m_setupSettings.disableErrorIntegrationWhenSaturated)
		{
			bool isPositiveSaturated = pidOutput > actuatorLimit - 0.01;
			bool isLowerSaturated = pidOutput < 0.01;
			if (!((isPositiveSaturated && r > angularSpeed) || (isLowerSaturated && r < angularSpeed)))
				errorSum += std::abs(error);
		}
		else
			errorSum += std::abs(error);


		// Check overshoot
		if ((lastR < angularSpeed) && (r < angularSpeed) && rWasRising)
		{
			double overshoot = (angularSpeed - r) / systemInputLimit;
			overshootSum += std::abs(overshoot);
			rWasRising = 2;
		}
		else
		{
			if (rWasRising == 2)
				rWasRising = 0;
		}
		lastR = angularSpeed;
	}
	double invEndTime = 1 / endTime;
	double invUpdateCount = dt / endTime;
	overshootSum *= invUpdateCount * m_tuningGoalFactor_overshoot;
	errorSum *= invUpdateCount * m_tuningGoalFactor_errorIntegral;
	pidOutChangeSum *= invUpdateCount * m_tuningGoalFactor_actuatorEffort / actuatorLimit;
	

	if (m_setupSettings.useMinimizingScore)
	{
		losses[0] = (errorSum);
		losses[1] = (pidOutChangeSum);
		losses[2] = (overshootSum);
		return losses;
	}
	else
	{
		losses[0] += 500 * (errorSum + pidOutChangeSum + overshootSum);
		std::vector<double> scores = losses;
		scores[0] = (500.0 / (losses[0] + 0.1));
		return scores;
	}
}

void DCMotorProblem::printSignalSequenceToConsole(const std::string& name, const std::vector<sf::Vector2<double>>& steps) const
{
	std::string stepSignalTimeData = name + "Time data = [";
	double dt = 0.01;

	for (size_t i=0; i< steps.size(); ++i)
	{
		const auto& step = steps[i];
		if (i > 0)
			stepSignalTimeData += std::to_string(step.x - dt) + " " + std::to_string(step.x) + " ";
		else
			stepSignalTimeData += std::to_string(step.x) + " ";
	}
	stepSignalTimeData += "]";
	qDebug() << stepSignalTimeData.c_str();

	std::string stepSignalValueData = name + "Value data = [";
	for (size_t i = 0; i < steps.size(); ++i)
	{
		const auto& step = steps[i];
		if (i > 0)
			stepSignalValueData += std::to_string(steps[i-1].y) + " " + std::to_string(step.y) + " ";
		else
			stepSignalValueData += std::to_string(step.x) + " ";
	}
	stepSignalValueData += "]";
	qDebug() << stepSignalValueData.c_str();
}

std::vector<sf::Vector2<double>>  DCMotorProblem::generateRandomStepSequence(double stepAmplitude, double maxTime, double minStepDuration, double maxStepDuration, size_t stepCount)
{
	std::vector<sf::Vector2<double>> stepData;
	double currentTime = 0.0;
	for (size_t i = 0; i < stepCount; ++i)
	{
		double stepTime = AutoTuner::Solver::getRandomDouble(currentTime + minStepDuration, std::min(currentTime + maxStepDuration, maxTime));
		double stepValue = AutoTuner::Solver::getRandomDouble(0.0, stepAmplitude);
		stepData.push_back({ stepTime, stepValue });
		currentTime = stepTime;
		if (currentTime >= maxTime)
			break;
	}
	return stepData;
}


void DCMotorProblem::setCSVHeader()
{
	std::vector<std::string> setParameterNames;
	setParameterNames.push_back("Kp");
	setParameterNames.push_back("Ki");
	setParameterNames.push_back("Kd");
	setParameterNames.push_back("Kn");
	setParameterNames.push_back("IntegralSaturation");
	setParameterNames.push_back("AntiWindupBackCalculationConstant");

	m_resultData.setParameterNames(setParameterNames);

	

	ResultData::ColumnData rPlot("r", AutoTuner::CSVExport::LineStyle::Solid, 1, sf::Color(0xf2c94e));
	ResultData::ColumnData lPlot("l", AutoTuner::CSVExport::LineStyle::Solid, 1, sf::Color(0xeb3a23));
	ResultData::ColumnData ePlot("e", AutoTuner::CSVExport::LineStyle::Solid, 1, sf::Color(0xf2c94e));

	sf::Color color1 = sf::Color(0x506ce6);
	sf::Color color2 = sf::Color(0x15297d);
	if(m_setupSettings.solverType == SolverType::GeneticAlgorithm) // specify the correct solver type
	{
		color1 = sf::Color(0x63e0d8);
		color2 = sf::Color(0x228a83);
	}

	ResultData::ColumnData uPlot("u", AutoTuner::CSVExport::LineStyle::Solid, 1, color1);
	ResultData::ColumnData yPlot("y", AutoTuner::CSVExport::LineStyle::Solid, 1, color2);

	ResultData::ColumnData time;
	time.name = "Zeit [s]";
	m_resultData.stepResponse.time = time;

	m_resultData.stepResponse.responseSignals = { rPlot, lPlot, ePlot, uPlot, yPlot };
	//using namespace AutoTuner;
	//m_csvExport.setHeader({ "Epoch", "Average", "Min", "Max"});
	//m_csvExport.setYAxisLabel("Score");
	//m_csvExport.setLineStyles({ 
	//	CSVExport::LineStyle::Solid, 
	//	CSVExport::LineStyle::Solid,
	//	CSVExport::LineStyle::Solid });
	//m_csvExport.setLineThicknesses({ 2, 1, 1 });
	///*std::vector<sf::Color> lineColors = {
	//	sf::Color(0, 0, 255), 
	//	sf::Color(60, 125, 168), 
	//	sf::Color(59, 168, 148)
	//};*/
	//m_csvExport.setLineColors(csvLineColor);
}
void DCMotorProblem::logCSVData()
{
	if (m_solverObject)
	{
		std::vector<double> score = m_solverObject->getScores();
		double minScore = 0.0;
		double maxScore = 0.0;
		double averageScore = 0.0;
		if (score.size() > 0)
		{
			minScore = score[0];
			maxScore = score[0];
			double sumScores = 0.0;
			for (double s : score)
			{
				if (s < minScore)
					minScore = s;
				if (s > maxScore)
					maxScore = s;
				sumScores += s;
			}
			averageScore = sumScores / static_cast<double>(score.size());
		}

		const double filterAlpha = 0.1;
		static double averageScoreFiltered = averageScore;
		static double minScoreFiltered = minScore;
		static double maxScoreFiltered = maxScore;

		if(m_epochCounter == 0)
		{
			averageScoreFiltered = averageScore;
			minScoreFiltered = minScore;
			maxScoreFiltered = maxScore;
		}
		else
		{
			averageScoreFiltered = (filterAlpha * averageScore) + ((1.0 - filterAlpha) * averageScoreFiltered);
			minScoreFiltered = (filterAlpha * minScore) + ((1.0 - filterAlpha) * minScoreFiltered);
			maxScoreFiltered = (filterAlpha * maxScore) + ((1.0 - filterAlpha) * maxScoreFiltered);
		}

		m_resultData.learningHistory.averageScore.data.push_back(averageScoreFiltered);
		m_resultData.learningHistory.worstScore.data.push_back(minScoreFiltered);
		m_resultData.learningHistory.bestScore.data.push_back(maxScoreFiltered);

		//m_csvExport.addRow({ std::to_string(m_epochCounter),
		//	std::to_string(averageScoreFiltered),
		//	std::to_string(minScoreFiltered),
		//	std::to_string(maxScoreFiltered) });
		const auto bestParameters = m_solverObject->getBestParameters();
		std::vector<ResultData::ColumnData>& parameterChanges = m_resultData.parameterChanges.parameters;
		/*for (size_t i = 0; i< bestParameters.size(); ++i)
		{
			if (parameterChanges.size() <= i)
			{
				parameterChanges.push_back(ResultData::ColumnData());
				parameterChanges[parameterChanges.size() - 1].name = "Param_" + std::to_string(i);
			}
			parameterChanges[i].data.push_back(bestParameters[i]);
		}*/


		size_t paramIndexCounter = 0;

		if (m_setupSettings.optimizeKp)
			parameterChanges[paramIndexCounter++].data.push_back(bestParameters[0]);
		else
			parameterChanges[paramIndexCounter++].data.push_back(m_setupSettings.defaultKp);

		if (m_setupSettings.optimizeKi)
			parameterChanges[paramIndexCounter++].data.push_back(bestParameters[1]);
		else
			parameterChanges[paramIndexCounter++].data.push_back(m_setupSettings.defaultKi);

		if (m_setupSettings.optimizeKd)
			parameterChanges[paramIndexCounter++].data.push_back(bestParameters[2]);
		else
			parameterChanges[paramIndexCounter++].data.push_back(m_setupSettings.defaultKd);

		if (m_setupSettings.useKn)
		{
			if (m_setupSettings.optimizeKn)
				parameterChanges[paramIndexCounter++].data.push_back(bestParameters[3]);
			else
				parameterChanges[paramIndexCounter++].data.push_back(m_setupSettings.defaultKn);
		}

		if (m_setupSettings.optimizeIntegralSaturation)
			parameterChanges[paramIndexCounter++].data.push_back(bestParameters[4]);
		else
			parameterChanges[paramIndexCounter++].data.push_back(m_setupSettings.defaultPIDISaturation);

		if (m_setupSettings.optimizeAntiWindupBackCalculationConstant)
			parameterChanges[paramIndexCounter++].data.push_back(bestParameters[5]);
		else
			parameterChanges[paramIndexCounter++].data.push_back(m_setupSettings.defaultPIDAntiWindupBackCalculationConstant);

		/*
#ifdef PARAMETERLIST_ENABLE_KP
		parameterChanges[paramIndexCounter++].data.push_back(bestParameters[0]);
#else
		parameterChanges[paramIndexCounter++].data.push_back(s_defaultKp);
#endif

#ifdef PARAMETERLIST_ENABLE_KI
		parameterChanges[paramIndexCounter++].data.push_back(bestParameters[1]);
#else
		parameterChanges[paramIndexCounter++].data.push_back(s_defaultKi);
#endif

#ifdef PARAMETERLIST_ENABLE_KD
		parameterChanges[paramIndexCounter++].data.push_back(bestParameters[2]);
#else
		parameterChanges[paramIndexCounter++].data.push_back(s_defaultKd);
#endif

#ifdef PARAMETERLIST_ENABLE_KN
		parameterChanges[paramIndexCounter++].data.push_back(bestParameters[3]);
#else
		parameterChanges[paramIndexCounter++].data.push_back(s_defaultKn);
#endif




#ifdef PARAMETERLIST_ENABLE_INTEGRAL_SATURATION
		parameterChanges[paramIndexCounter++].data.push_back(params[counter++]);
#else
		parameterChanges[paramIndexCounter++].data.push_back(s_defaultPIDISaturation);
#endif

#ifdef PARAMETERLIST_ENABLE_ANTI_WINDUP_BACK_CALCULATION_CONSTANT
		parameterChanges[paramIndexCounter++].data.push_back(params[counter++]);
#else
		parameterChanges[paramIndexCounter++].data.push_back(s_defaultPIDAntiWindupBackCalculationConstant);
#endif*/
	}
}

// Get score from given PID parameters
void DCMotorProblem::testCustomPID()
{
	
	double Kp = 11.1112;
	double Ki = 79.365;
	double Kd = -1.8759e-4;
	double Kn = 1/1.9757;

	double integralSaturation = m_setupSettings.defaultPIDISaturation;
	double backCalculationConstant = m_setupSettings.defaultPIDAntiWindupBackCalculationConstant;


	std::vector<double> params = { Kp, Ki, Kd, Kn, integralSaturation, backCalculationConstant };
	auto scores = agentTestFunction(params, 0);
	double score = 0.0;
	for (const auto& s : scores)
	{
		score += s;
		qDebug() << "Score Component: " << s;
	}
	qDebug() << "Custom PID Test Score: " << score;
}

void DCMotorProblem::saveResultDataToFile(const ResultData& resultData, std::string folderPath) const
{
	
	folderPath += "/" + resultData.resultName;

	// Check if folder path exists
	QDir directory(QString::fromStdString(folderPath));
	if (!directory.exists())
	{
		directory.mkpath(".");
	}
	else
	{
		size_t counter = 1;
		QString newPath;
		
		QDir dirCheck(newPath);
		do {
			newPath = QString::fromStdString(folderPath) + "_" + QString::number(counter);
			dirCheck = QDir(newPath);
			++counter;
		} while (dirCheck.exists());
		folderPath = newPath.toStdString();
		directory = QDir(QString::fromStdString(folderPath));
		directory.mkpath(".");
	}

	{
		AutoTuner::CSVExport learningHistoryCSV;
		std::string yAxisLabel = "Fitness";
		if(m_setupSettings.useMinimizingScore)
			yAxisLabel = "Fehler";

//#ifdef GENETIC_USE_MINIMIZING_SCORE
//		std::string yAxisLabel = "Fehler";
//#else
//		std::string yAxisLabel = "Fitness";
//#endif
		std::vector<std::string> labels = {"Epoche" };
		std::vector<AutoTuner::CSVExport::LineStyle> lineStyles;
		std::vector<sf::Color> lineColors;
		std::vector<int> lineThicknesses;

		if (resultData.learningHistory.worstScore.isEnabled)
		{
			labels.push_back(resultData.learningHistory.worstScore.name);
			lineStyles.push_back(resultData.learningHistory.worstScore.lineStyle);
			lineColors.push_back(resultData.learningHistory.worstScore.lineColor);
			lineThicknesses.push_back(resultData.learningHistory.worstScore.lineThickness);
		}
		if (resultData.learningHistory.averageScore.isEnabled)
		{
			labels.push_back(resultData.learningHistory.averageScore.name);
			lineStyles.push_back(resultData.learningHistory.averageScore.lineStyle);
			lineColors.push_back(resultData.learningHistory.averageScore.lineColor);
			lineThicknesses.push_back(resultData.learningHistory.averageScore.lineThickness);
		}
		if (resultData.learningHistory.bestScore.isEnabled)
		{
			labels.push_back(resultData.learningHistory.bestScore.name);
			lineStyles.push_back(resultData.learningHistory.bestScore.lineStyle);
			lineColors.push_back(resultData.learningHistory.bestScore.lineColor);
			lineThicknesses.push_back(resultData.learningHistory.bestScore.lineThickness);
		}


		learningHistoryCSV.setHeader(labels);
		learningHistoryCSV.setYAxisLabel(yAxisLabel);
		learningHistoryCSV.setLineStyles(lineStyles);
		learningHistoryCSV.setLineColors(lineColors);
		learningHistoryCSV.setLineThicknesses(lineThicknesses);

		size_t dataCount = resultData.learningHistory.bestScore.data.size();
		for (size_t i = 0; i < dataCount; ++i)
		{
			std::vector<std::string> rowData = { std::to_string(i) };
			if (resultData.learningHistory.worstScore.isEnabled)
				rowData.push_back(std::to_string(resultData.learningHistory.worstScore.data[i]));
			if (resultData.learningHistory.averageScore.isEnabled)
				rowData.push_back(std::to_string(resultData.learningHistory.averageScore.data[i]));
			if (resultData.learningHistory.bestScore.isEnabled)
				rowData.push_back(std::to_string(resultData.learningHistory.bestScore.data[i]));
			learningHistoryCSV.addRow(rowData);
		}

		learningHistoryCSV.exportToFile(folderPath + "/learning_history.csv", ';');
	}

	{
		AutoTuner::CSVExport stepResponceCSV;
		std::string yAxisLabel = "";

		std::vector<std::string> labels = { resultData.stepResponse.time.name };
		std::vector<AutoTuner::CSVExport::LineStyle> lineStyles;
		std::vector<sf::Color> lineColors;
		std::vector<int> lineThicknesses;

		for(size_t i=0; i< resultData.stepResponse.responseSignals.size(); ++i)
		{
			if(resultData.stepResponse.responseSignals[i].isEnabled == false)
				continue;
			labels.push_back(resultData.stepResponse.responseSignals[i].name);
			lineStyles.push_back(resultData.stepResponse.responseSignals[i].lineStyle);
			lineColors.push_back(resultData.stepResponse.responseSignals[i].lineColor);
			lineThicknesses.push_back(resultData.stepResponse.responseSignals[i].lineThickness);
		}

		stepResponceCSV.setHeader(labels);
		stepResponceCSV.setYAxisLabel(yAxisLabel);
		stepResponceCSV.setLineStyles(lineStyles);
		stepResponceCSV.setLineColors(lineColors);
		stepResponceCSV.setLineThicknesses(lineThicknesses);

		size_t dataCount = resultData.stepResponse.time.data.size();
		for (size_t i = 0; i < dataCount; ++i)
		{
			std::vector<std::string> rowData = { std::to_string(resultData.stepResponse.time.data[i]) };
			for(size_t j=0; j< resultData.stepResponse.responseSignals.size(); ++j)
			{
				if (resultData.stepResponse.responseSignals[j].isEnabled == false)
					continue;
				rowData.push_back(std::to_string(resultData.stepResponse.responseSignals[j].data[i]));
			}
			stepResponceCSV.addRow(rowData);
		}

		stepResponceCSV.exportToFile(folderPath + "/step_response.csv", ';');
	}

	{
		AutoTuner::CSVExport parameterChangeCSV;
		std::string yAxisLabel = "";

		std::vector<std::string> labels = { "Epoche"};
		std::vector<AutoTuner::CSVExport::LineStyle> lineStyles;
		std::vector<sf::Color> lineColors;
		std::vector<int> lineThicknesses;

		for (size_t i = 0; i < resultData.parameterChanges.parameters.size(); ++i)
		{
			if (resultData.parameterChanges.parameters[i].isEnabled == false)
				continue;
			labels.push_back(resultData.parameterChanges.parameters[i].name);
			lineStyles.push_back(resultData.parameterChanges.parameters[i].lineStyle);
			lineColors.push_back(resultData.parameterChanges.parameters[i].lineColor);
			lineThicknesses.push_back(resultData.parameterChanges.parameters[i].lineThickness);
		}

		parameterChangeCSV.setHeader(labels);
		parameterChangeCSV.setYAxisLabel(yAxisLabel);
		parameterChangeCSV.setLineStyles(lineStyles);
		parameterChangeCSV.setLineColors(lineColors);
		parameterChangeCSV.setLineThicknesses(lineThicknesses);

		size_t dataCount = resultData.parameterChanges.parameters[0].data.size();
		for (size_t i = 0; i < dataCount; ++i)
		{
			std::vector<std::string> rowData = { std::to_string(i) };
			for (size_t j = 0; j < resultData.parameterChanges.parameters.size(); ++j)
			{
				if (resultData.parameterChanges.parameters[j].isEnabled == false)
					continue;
				rowData.push_back(std::to_string(resultData.parameterChanges.parameters[j].data[i]));
			}
			parameterChangeCSV.addRow(rowData);
		}

		parameterChangeCSV.exportToFile(folderPath + "/parameter_changes.csv", ';');
	}



	{
		std::ofstream file(folderPath + "/parameters.csv");
		if (!file.is_open())
			return;

		// Write header
		for (size_t i = 0; i < resultData.parameters.size(); ++i)
		{
			file << resultData.parameters[i].name << ";";
		}
		file << "\n";
		// Write values
		for (size_t i = 0; i < resultData.parameters.size(); ++i)
		{
			file << resultData.parameters[i].value << ";";
		}
		file << "\n";
		file.close();
	}

	/*learningHistoryCSV.setHeader({"Epoch", "Average", "Min", "Max"});
	learningHistoryCSV.setYAxisLabel("Score");
	learningHistoryCSV.setLineStyles({
		CSVExport::LineStyle::Solid,
		CSVExport::LineStyle::Solid,
		CSVExport::LineStyle::Solid });
	learningHistoryCSV.setLineThicknesses({ 2, 1, 1 });
	learningHistoryCSV.setLineColors(csvLineColor);*/

}