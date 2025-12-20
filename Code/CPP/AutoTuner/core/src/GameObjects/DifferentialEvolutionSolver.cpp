#include "GameObjects/DifferentialEvolutionSolver.h"

namespace AutoTuner
{
	DifferentialEvolutionSolver::DifferentialEvolutionSolver()
		: Solver("DifferentialEvolutionSolver")
		, m_alltimeBestIndividual(1)
		, m_lastRoundBestIndividual(1)
	{
		m_differentialEvolution.setFitnessFunction(std::bind(&DifferentialEvolutionSolver::fitnessFunction, this, std::placeholders::_1, std::placeholders::_2));
		m_differentialEvolution.setMaxGenerations(1000000);
		m_painter = new Painter("DifferentialEvolutionSolverPainter");
		addComponent(m_painter);

#ifdef DIFFERENTIAL_SOLVER_USE_THREAD_POOL
		m_differentialEvolution.enableThreadPool(true);
#endif
	}
	DifferentialEvolutionSolver::~DifferentialEvolutionSolver()
	{
	}

	void DifferentialEvolutionSolver::setOptimizingDirection(OptimizingDirection direction)
	{
		Solver::setOptimizingDirection(direction);
		switch (direction)
		{
			case OptimizingDirection::Minimize:
				m_differentialEvolution.setOptimizingDirection(QSFML::Utilities::DifferentialEvolution::OptimizingDirection::Minimize);
				break;
			case OptimizingDirection::Maximize:
				m_differentialEvolution.setOptimizingDirection(QSFML::Utilities::DifferentialEvolution::OptimizingDirection::Maximize);
		}
	}


	void DifferentialEvolutionSolver::setInitialParameters(const std::vector<std::vector<double>>& parameterList)
	{
		std::vector<QSFML::Utilities::DifferentialEvolution::Individual> initialPopulation;
		for (const auto& params : parameterList)
		{
			QSFML::Utilities::DifferentialEvolution::Individual individual(params.size());
			individual.parameters = params;
			initialPopulation.push_back(individual);
		}
		m_differentialEvolution.setPopulation(initialPopulation);
		m_painter->reset();
	}


	void DifferentialEvolutionSolver::iterate()
	{
		m_painter->setPopulation(m_differentialEvolution.getPopulation());
		// Implement the logic to update the parameters using Differential Evolution algorithm
		// This is a placeholder implementation
		for(size_t i=0; i< m_tmpScoreCollector.size(); ++i)
		{
			m_tmpScoreCollector[i] = 0.0;
		}
		m_differentialEvolution.evolve();

		double sumScores = 0.0;
		for (size_t i = 0; i < m_tmpScoreCollector.size(); ++i)
		{
			sumScores += m_tmpScoreCollector[i];
		}
		for (size_t i = 0; i < m_tmpScoreCollector.size(); ++i)
		{
			m_tmpScoreCollector[i] /= sumScores;
		}
		m_painter->setScoreParts(m_tmpScoreCollector);


		m_lastRoundBestIndividual = m_differentialEvolution.getBestIndividual();
		if(m_optimizingDirection == OptimizingDirection::Maximize && m_lastRoundBestIndividual.fitness > m_alltimeBestIndividual.fitness ||
		   m_optimizingDirection == OptimizingDirection::Minimize && m_lastRoundBestIndividual.fitness < m_alltimeBestIndividual.fitness)
		{
			m_alltimeBestIndividual = m_lastRoundBestIndividual;
		}
	}
	void DifferentialEvolutionSolver::test()
	{
		// Implement the logic to test the current parameters
		// This is a placeholder implementation
		//m_differentialEvolution.
	}

	void DifferentialEvolutionSolver::setMutationAmount(double amount) 
	{ 
		m_differentialEvolution.setMutationFactor(amount);
	}
	double DifferentialEvolutionSolver::getMutationAmount() const 
	{ 
		return m_differentialEvolution.getMutationFactor();
	}
	

	void DifferentialEvolutionSolver::setParametersToColorFunc(ParametersToColorFunc func)
	{
		m_painter->setAgentToColorFunc(func);
	}
	void DifferentialEvolutionSolver::setParametersTestFunc(ParametersTestFunc func)
	{
		m_parametersTestFunc = func;
	}
	void DifferentialEvolutionSolver::setScorePartsLabels(const std::vector<std::string>& labels)
	{
		m_painter->setScorePartsLabels(labels);
		m_tmpScoreCollector.resize(labels.size(), 0.0);
	}

	std::vector<double> DifferentialEvolutionSolver::getAlltimeBestParameters() const
	{
		return m_alltimeBestIndividual.parameters;
	}
	std::vector<double> DifferentialEvolutionSolver::getBestParameters() const
	{
		return m_lastRoundBestIndividual.parameters;
	}

	void DifferentialEvolutionSolver::clearAlltimeBestParameters()
	{
		m_alltimeBestIndividual = QSFML::Utilities::DifferentialEvolution::Individual(m_alltimeBestIndividual.parameters.size());
		if (m_optimizingDirection == OptimizingDirection::Minimize)
		{
			m_alltimeBestIndividual.fitness = std::numeric_limits<double>::infinity();
		}
		else
		{
			m_alltimeBestIndividual.fitness = -std::numeric_limits<double>::infinity();
		}
		m_painter->reset();
	}

	double DifferentialEvolutionSolver::fitnessFunction(const std::vector<double>& parameters, size_t index)
	{
		double score = 0.0;
		if (m_parametersTestFunc)
		{
			auto scores = m_parametersTestFunc(parameters, index);

			for (size_t i=0; i<scores.size(); ++i)
			{
				score += scores[i];
				m_tmpScoreCollector[i] += scores[i];
			}
		}
		return score;
	}


	//
	// DifferentialEvolutionSolver::Painter
	// 


	void DifferentialEvolutionSolver::Painter::setPopulation(const std::vector<QSFML::Utilities::DifferentialEvolution::Individual>& individuals)
	{
		double sumScore = 0.0;
		m_population = individuals;
		for(size_t i=0; i< individuals.size(); ++i)
		{
			sumScore += individuals[i].fitness;
		}
		sumScore /= static_cast<double>(individuals.size());

		

		double time = 0;
		if (m_averageScoresHistoryTimeline.size() > 0)
			time = m_averageScoresHistoryTimeline[m_averageScoresHistoryTimeline.size() - 1] + 1;
		m_averageScoresHistoryTimeline.push_back(time);
		m_averageScoresHistory.push_back(sumScore);
		if (m_averageScoresHistory.size() > m_historySize)
		{
			m_averageScoresHistory.erase(m_averageScoresHistory.begin());
			m_averageScoresHistoryTimeline.erase(m_averageScoresHistoryTimeline.begin());
		}
	}
	void DifferentialEvolutionSolver::Painter::setScorePartsLabels(const std::vector<std::string>& labels)
	{
		for (size_t i = 0; i < m_scorePartsLabels.size(); ++i)
		{
			delete[] m_scorePartsLabels[i];
		}
		m_scorePartsLabels.clear();
		m_scorePartsLabels.reserve(labels.size());
		for (const auto& label : labels)
		{
			char* s = new char[label.size() + 1];
			memcpy(s, label.c_str(), label.size() + 1);
			m_scorePartsLabels.push_back(s);
		}
		m_scoreParts = std::vector<double>(labels.size(), 0.0);
	}
	/*void DifferentialEvolutionSolver::Painter::setScorePartsLabels(const std::vector<std::string>& labels)
	{
		for(size_t i=0; i< m_scorePartsLabels.size(); ++i)
		{
			delete[] m_scorePartsLabels[i];
		}
		m_scorePartsLabels.clear();
		m_scorePartsLabels.reserve(labels.size());
		for (const auto& label : labels)
		{
			char* s = new char[label.size() + 1];
			std::strcpy(s, label.c_str());
			m_scorePartsLabels.push_back(s);
		}
		m_scoreParts = std::vector<double>(labels.size(), 0.0);
	}*/

	void DifferentialEvolutionSolver::Painter::drawComponent(sf::RenderTarget& target, sf::RenderStates states) const
	{
		ImGui::Begin("Differential Evolution Solver");

		if (ImPlot::BeginPlot("Average score history", ImVec2(-1, 200))) {
			ImPlot::SetupAxes("Iteration", "Average score");
			int dataSize = m_averageScoresHistory.size();

			if (dataSize > 0) {

				// Set axis limits based on actual x values
				double xMin = m_averageScoresHistoryTimeline[0];
				double xMax = m_averageScoresHistoryTimeline[m_averageScoresHistoryTimeline.size() - 1];

				ImPlot::SetupAxisLimits(ImAxis_X1, xMin, xMax, ImPlotCond_Always);

				// Auto-fit Y axis for visible data
				auto yBegin = m_averageScoresHistory.begin();
				auto yEnd = m_averageScoresHistory.end();
				ImPlot::SetupAxisLimits(ImAxis_Y1,
					*std::min_element(yBegin, yEnd),
					*std::max_element(yBegin, yEnd),
					ImPlotCond_Always);

				// Plot only the visible window
				ImPlot::PlotLine("Average score",
					m_averageScoresHistoryTimeline.data(),
					m_averageScoresHistory.data(),
					m_averageScoresHistory.size());
			}

			ImPlot::EndPlot();
		}

		if (m_scoreParts.size() > 1)
		{
			// Create a plot
			if (ImPlot::BeginPlot("Score parts", ImVec2(-1, 200))) {
				// Convert std::string labels to const char* (ImPlot needs this form)
				// Set up x positions for each bar
				ImPlot::SetupAxes("Category", "Value");
				ImPlot::SetupAxisTicks(ImAxis_X1, 0, m_scoreParts.size() - 1, m_scoreParts.size(), m_scorePartsLabels.data());

				// Draw bars
				ImPlot::PlotBars("Data", m_scoreParts.data(), m_scoreParts.size(), 0.5f); // 0.5f = bar width

				ImPlot::EndPlot();
			}
		}


		ImGui::End();


		if (m_agentToColorFunc != nullptr)
		{
			size_t counter = 0;
			size_t gridColumns = std::sqrt(m_population.size());

			for (const auto& agent : m_population)
			{
				sf::Color color = m_agentToColorFunc(agent.parameters);
				double size = agent.fitness * 0.1;
				if (size < 1)
					size = 1;
				if (size > 10)
					size = 10;
				sf::RectangleShape rect(sf::Vector2f(size, size));
				rect.setFillColor(color);

				size_t gridPosX = counter % gridColumns;
				size_t gridPosY = counter / gridColumns;
				rect.setPosition(static_cast<float>(gridPosX * 11), static_cast<float>(gridPosY * 11));
				counter++;
				target.draw(rect, states);
			}
		}
	}
}