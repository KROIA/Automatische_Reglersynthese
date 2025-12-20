#include "Scene/Objects/SystemOptimizer.h"


SystemOptimizer::SystemOptimizer(const std::string& name,
	GameObject* parent)
	: QSFML::Objects::GameObject(name, parent)
	, m_systemPlotModel(nullptr)
	, m_solverObject(nullptr)
{

	m_chartViewComponent = new AutoTuner::ChartViewComponent("ChartViewComponent");
	addComponent(m_chartViewComponent);
}





void SystemOptimizer::setSolverObject(AutoTuner::Solver* solver)
{
	if (m_solverObject)
	{
		m_solverObject->setParametersTestFunc(nullptr);
		removeChild(m_solverObject);
	}
	m_solverObject = solver;
	if (m_solverObject)
	{
		m_solverObject->setParametersTestFunc(std::bind(&SystemOptimizer::agentTestFunction, this, std::placeholders::_1, std::placeholders::_2));
		m_solverObject->setScorePartsLabels({ "rmse" });
		m_solverObject->setOptimizingDirection(AutoTuner::Solver::OptimizingDirection::Minimize);
		addChild(m_solverObject);
	}
}

void SystemOptimizer::startOptimization(size_t agentsCount, double startAreaSpread)
{
	std::vector<std::vector<double>> initialPopulation;

	for (size_t i = 0; i < agentsCount; ++i)
	{
		std::vector<double> individual;
		for (const auto& param : m_initialParameters)
		{
			double spread = startAreaSpread * (static_cast<double>(rand()) / RAND_MAX - 0.5) * 2.0;
			individual.push_back(param + spread);
		}
		initialPopulation.push_back(individual);
	}

	startOptimization(initialPopulation);
}
void SystemOptimizer::startOptimization()
{
	startOptimization(30, 10.0);
}
void SystemOptimizer::startOptimization(const std::vector<std::vector<double>>& startParams)
{
	if (m_optimizing || !m_solverObject || !m_systemPlotModel)
		return;
	m_optimizing = true;
	m_currentEpoch = 0;
	m_solverObject->clearAlltimeBestParameters();
	m_solverObject->setInitialParameters(startParams);

	cloneSystems();
}
void SystemOptimizer::stopOptimization()
{
	if(!m_optimizing)
		return;
	m_optimizing = false;
}


void SystemOptimizer::updateBestParametersChartView(double dtResolution)
{
	if (m_solverObject && m_systemPlotModel && m_chartViewComponent && m_stimulusResponseDataCollection.size() > 2)
	{
		++m_printBestCounter;
		if (m_printBestCounter >= 1000)
		{
			m_printBestCounter = 0;
			auto bestParams = m_solverObject->getBestParameters();
			std::cout << "Epoch " << m_currentEpoch << " Best Parameters: [";
			for (const auto& p : bestParams)
			{
				std::cout << p << " ";
			}
			auto scores = m_solverObject->getScores();
			std::cout << "] Score: ";
			double scoreSum = 0;
			for (const auto& s : scores)
			{
				scoreSum += s;
			}
			std::cout << scoreSum;
			std::cout << std::endl;
		}

		//m_bestParameters = m_solverObject->getAlltimeBestParameters();
		m_bestParameters = m_solverObject->getBestParameters();
		m_chartViewComponent->clearPlotData();

		std::vector<AutoTuner::ChartViewComponent::PlotData> inputSingals;
		std::vector<AutoTuner::ChartViewComponent::PlotData> desiredOuptutSignals;
		std::vector<AutoTuner::ChartViewComponent::PlotData> systemOutputSignals;

		inputSingals.resize(m_systemPlotModel->getInputs().size());
		desiredOuptutSignals.resize(m_systemPlotModel->getOutputs().size());
		systemOutputSignals.resize(m_systemPlotModel->getOutputs().size());

		// Set plot names
		for (size_t i = 0; i < inputSingals.size(); ++i)
		{
			inputSingals[i].setName("u" + std::to_string(i));
		}
		for (size_t i = 0; i < desiredOuptutSignals.size(); ++i)
		{
			desiredOuptutSignals[i].setName("y_desired_" + std::to_string(i));
			systemOutputSignals[i].setName("y_system_" + std::to_string(i));
		}

		double time = 0;
		double createSnapshotTime = 0;
		m_systemPlotModel->reset();
		m_systemPlotModel->setParameters(m_bestParameters);
		for (const auto& data : m_stimulusResponseDataCollection)
		{
			

			m_systemPlotModel->setInputSignals(data.inputSignal);
			m_systemPlotModel->update(data.deltaTime);
			auto outputs = m_systemPlotModel->getOutputs();
			time += data.deltaTime;
			createSnapshotTime += data.deltaTime;

			if (createSnapshotTime >= dtResolution)
			{
				createSnapshotTime = 0;
				for (size_t i = 0; i < inputSingals.size(); ++i)
				{
					inputSingals[i].addDataPoint(time, data.inputSignal[i]);
				}
				for (size_t i = 0; i < outputs.size(); ++i)
				{
					systemOutputSignals[i].addDataPoint(time, outputs[i]);
					desiredOuptutSignals[i].addDataPoint(time, data.outputSignal[i]);
				}
			}			
		}

		// Add plots to chart view
		for (const auto& plot : inputSingals)
		{
			m_chartViewComponent->addPlotData(plot);
		}
		for (const auto& plot : desiredOuptutSignals)
		{
			m_chartViewComponent->addPlotData(plot);
		}
		for (const auto& plot : systemOutputSignals)
		{
			m_chartViewComponent->addPlotData(plot);
		}
	}
}

void SystemOptimizer::update()
{
	if (m_optimizing)
	{
		if (m_solverObject && m_systemPlotModel)
		{
			m_solverObject->test();
			m_solverObject->iterate();
			m_currentEpoch++;

			double plotDt = 0.1;
			if (m_stimulusResponseDataCollection.size() > 0)
			{
				double stepsCount = m_stimulusResponseDataCollection.size();
				plotDt = m_stimulusResponseDataCollection[0].deltaTime * stepsCount/1000;
			}
			updateBestParametersChartView(plotDt);
		}
	}
}

std::vector<double> SystemOptimizer::agentTestFunction(const std::vector<double>& parameters, size_t index)
{
	double errorSum = 0;
	double time = 0;
	std::shared_ptr<AutoTuner::TunableTimeBasedSystem> systemModel = m_agentsSystemModels[index];
	systemModel->reset();
	systemModel->setParameters(parameters);
	for (const auto& data : m_stimulusResponseDataCollection)
	{
		systemModel->setInputSignals(data.inputSignal);
		systemModel->update(data.deltaTime);
		auto outputs = systemModel->getOutputs();

		for (size_t i = 0; i < outputs.size(); ++i)
		{
			double error = data.outputSignal[i] - outputs[i];
			errorSum += error * error;
		}
		time += data.deltaTime;
	}
	double rmse = (errorSum / (time * systemModel->getOutputs().size()));
	return { rmse };
}