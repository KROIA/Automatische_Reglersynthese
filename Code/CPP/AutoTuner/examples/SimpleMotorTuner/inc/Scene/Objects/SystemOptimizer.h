#pragma once

#include "AutoTuner.h"

class SystemOptimizer : public QObject, public QSFML::Objects::GameObject
{
	Q_OBJECT
public:
	struct StimulusResponseData
	{
		double deltaTime;
		std::vector<double> inputSignal;
		std::vector<double> outputSignal;
	};

	SystemOptimizer(const std::string& name = "SystemOptimizer",
		GameObject* parent = nullptr);
	
	virtual void setModel(const std::shared_ptr<AutoTuner::TunableTimeBasedSystem>& model)
	{
		m_systemPlotModel = model;
		if(m_systemPlotModel)
			m_initialParameters = m_systemPlotModel->getParameters();
		if (m_optimizing)
		{
			cloneSystems();
		}
	}
	virtual const std::vector<double>& getInitialParameters() const
	{
		return m_initialParameters;
	}
	virtual const std::vector<double>& getBestParameters() const
	{
		return m_bestParameters;
	}
	virtual void setSolverObject(AutoTuner::Solver* solver);
	virtual void setStimulusResponseDataCollection(const std::vector<StimulusResponseData>& dataCollection)
	{
		m_stimulusResponseDataCollection = dataCollection;
	}


	virtual void startOptimization(size_t agentsCount, double startAreaSpread = 10);
	virtual void startOptimization();
	virtual void startOptimization(const std::vector<std::vector<double>> &startParams);
	virtual void stopOptimization();
	bool isOptimizing() const
	{
		return m_optimizing;
	}
	size_t getCurrentEpoch() const
	{
		return m_currentEpoch;
	}

	virtual void updateBestParametersChartView(double dtResolution = 0.01);
	void update() override;

protected:

	virtual std::vector<double> agentTestFunction(const std::vector<double>& parameters, size_t index);
private:
	void cloneSystems()
	{
		m_agentsSystemModels.clear();
		m_agentsSystemModels.reserve(m_stimulusResponseDataCollection.size());
		for (size_t i = 0; i < m_stimulusResponseDataCollection.size(); ++i)
		{
			auto copy = m_systemPlotModel->clone();
			AutoTuner::TunableTimeBasedSystem* copyPtr = dynamic_cast<AutoTuner::TunableTimeBasedSystem*>(copy);
			if(copyPtr)
				m_agentsSystemModels.push_back(std::shared_ptr<AutoTuner::TunableTimeBasedSystem>(copyPtr));
			else
			{
				throw std::runtime_error("SystemOptimizer::cloneSystems: Failed to clone TunableTimeBasedSystem.");
			}
		}
	}
	
	
	std::shared_ptr<AutoTuner::TunableTimeBasedSystem> m_systemPlotModel;
	std::vector< std::shared_ptr<AutoTuner::TunableTimeBasedSystem>> m_agentsSystemModels;
	std::vector<StimulusResponseData> m_stimulusResponseDataCollection;
	AutoTuner::Solver* m_solverObject;

	AutoTuner::ChartViewComponent* m_chartViewComponent = nullptr;

	std::vector<double> m_bestParameters;
	std::vector<double> m_initialParameters;

	bool m_optimizing = false;
	size_t m_currentEpoch = 0;
	size_t m_printBestCounter = 0;
	//struct Agent
	//{
	//	std::vector<double> parameters;
	//	double score;
	//};
};