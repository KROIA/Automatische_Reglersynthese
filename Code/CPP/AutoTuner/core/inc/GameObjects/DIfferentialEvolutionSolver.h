#pragma once

#include "AutoTuner_base.h"
#include "GameObjects/Solver.h"

#define DIFFERENTIAL_SOLVER_USE_THREAD_POOL
namespace AutoTuner
{
	class AUTO_TUNER_API DifferentialEvolutionSolver : public Solver
	{
	public:
		DifferentialEvolutionSolver();
		~DifferentialEvolutionSolver();

		void setOptimizingDirection(OptimizingDirection direction) override;

		void setInitialParameters(const std::vector<std::vector<double>>& parameterList) override;
		void iterate() override;
		void test() override;


		void setMutationAmount(double amount) override;
		double getMutationAmount() const override;
		void setParametersToColorFunc(ParametersToColorFunc func) override;
		void setParametersTestFunc(ParametersTestFunc func) override;
		void setScorePartsLabels(const std::vector<std::string>& labels) override;

		std::vector<double> getAlltimeBestParameters() const override;
		std::vector<double> getBestParameters() const override;

		std::vector<double> getScores() override
		{
			std::vector<double> scores;
			const auto &population = m_differentialEvolution.getPopulation();
			scores.reserve(population.size());
			for (const auto& agent : population)
			{
				scores.push_back(agent.fitness);
			}
			return scores;
		}

		void clearAlltimeBestParameters() override;
	private:
		double fitnessFunction(const std::vector<double>& parameters, size_t index);

		class AUTO_TUNER_API Painter : public QSFML::Components::Drawable
		{
		public:
			Painter(const std::string& name = "Painter")
				: QSFML::Components::Drawable(name)
			{

			}
			void reset()
			{
				m_averageScoresHistory.clear();
				m_averageScoresHistoryTimeline.clear();
			}
			void setPopulation(const std::vector<QSFML::Utilities::DifferentialEvolution::Individual>& individuals);
			void setScorePartsLabels(const std::vector<std::string>& labels);
			void setScoreParts(const std::vector<double>& parts)
			{
				m_scoreParts = parts;
			}
			void setAgentToColorFunc(ParametersToColorFunc func) { m_agentToColorFunc = func; }

		protected:
			void drawComponent(sf::RenderTarget& target, sf::RenderStates states) const override;

		private:

			size_t m_historySize = 1000;
			std::vector<double> m_averageScoresHistory;
			std::vector<double> m_averageScoresHistoryTimeline;

			std::vector<double> m_scoreParts;
			std::vector<const char*> m_scorePartsLabels;


			std::vector<QSFML::Utilities::DifferentialEvolution::Individual> m_population;

			ParametersToColorFunc m_agentToColorFunc = nullptr;
		};
		Painter* m_painter = nullptr;

		ParametersTestFunc m_parametersTestFunc = nullptr;


		QSFML::Utilities::DifferentialEvolution m_differentialEvolution;
		std::vector<double> m_tmpScoreCollector;

		QSFML::Utilities::DifferentialEvolution::Individual m_alltimeBestIndividual;
		QSFML::Utilities::DifferentialEvolution::Individual m_lastRoundBestIndividual;

	};
}