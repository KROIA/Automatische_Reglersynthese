#pragma once

#include "AutoTuner_base.h"
#include "GameObjects/Solver.h"

#define GENETIC_SOLVER_USE_THREAD_POOL
#define GENETIC_SOLVER_USE_INDIVIDUAL_PARAMETER_MUTATION_RATE

namespace AutoTuner
{
	
	class AUTO_TUNER_API GeneticSolver : public Solver
	{
		static constexpr size_t s_maxThreadWorkerCount = 64;
	public:
		
		struct Agent
		{
			//size_t ID = 0;
			double score = 0.0;
			std::vector<double> scoreParts;
			std::vector<double> parameters;

			std::vector<double> mutationFactors;
		};
		

		GeneticSolver(const std::string& name = "GeneticSolver",
			GameObject* parent = nullptr);
		~GeneticSolver();

		std::vector<Agent>& getPopulation() { return m_population; }
		//void setPopulation(const std::vector<Agent>& population);
		void setInitialParameters(const std::vector<std::vector<double>>& parameterList) override;
		void clearAlltimeBestParameters() 
		{
			m_alltimeBestAgent = Agent(); 
			m_painter->reset();
		}
		void setScorePartsLabels(const std::vector<std::string>& labels) override
		{
			m_painter->setScorePartsLabels(labels);
		}

		std::vector<double> getAlltimeBestParameters() const override { return m_alltimeBestAgent.parameters; }
		std::vector<double> getBestParameters() const override {
			return  m_bestLastRoundAgent.parameters;
		}
		void setMutationAmount(double amount) override { m_mutationAmount = amount; }
		double getMutationAmount() const override { return m_mutationAmount; }
		void setMutationPropability(double rate) { m_mutationPropability = rate; }
		double getMutationPropability() const { return m_mutationPropability; }

		void setParametersToColorFunc(ParametersToColorFunc func) override { m_painter->setAgentToColorFunc(func); }
		void setParametersTestFunc(ParametersTestFunc func) override { m_agentTestFunc = func; }

		void setOptimizingDirection(OptimizingDirection direction)
		{
			m_optimizingDirection = direction;
		}
		OptimizingDirection getOptimizingDirection() const
		{
			return m_optimizingDirection;
		}
		void setMinimizingStaticOffset(double offset)
		{
			m_minimizingStaticOffset = offset;
		}
		double getMinimizingStaticOffset() const
		{
			return m_minimizingStaticOffset;
		}

		std::vector<double> getScores() override
		{
			return m_lastPopulationScores;
		}

		/**
		 * @brief
		 * Changes the parameters of the system being tested.
		 */
		void iterate() override;

		/**
		 * @brief
		 * Tests the current parameters of the system.
		 */
		void test() override;



		void sortPopulation(std::vector<Agent>& population);
		std::pair<size_t,size_t> selectParents(const std::vector<Agent>& population, double sumScore);
		void mutate(Agent& agent);
		void crossover(const Agent& parent1, const Agent& parent2, Agent& offspring1, Agent& offspring2);

		bool isThreadsBusy() const
		{
#ifdef GENETIC_SOLVER_USE_THREAD_POOL
			return m_threadsBusy.load();
#else
			return false;
#endif
		}
	private:
		class AUTO_TUNER_API Painter : public QSFML::Components::Drawable
		{
		public:
			Painter(const std::string& name = "Painter")
				: QSFML::Components::Drawable(name)
			{

			}
			void setPopulation(const std::vector<Agent>& agents, double sumScore);
			void setScorePartsLabels(const std::vector<std::string>& labels);
			void setAgentToColorFunc(ParametersToColorFunc func) { m_agentToColorFunc = func; }

			void reset()
			{
				m_averageScoresHistory.clear();
				m_averageScoresHistoryTimeline.clear();
			}

		protected:
			void drawComponent(sf::RenderTarget& target, sf::RenderStates states) const override;

		private:
			std::vector<Agent> m_population;

			std::vector<char*> m_piChartLabels;
			std::vector<float> m_piChartData;

			size_t m_historySize = 1000;
			std::vector<double> m_averageScoresHistory;
			std::vector<double> m_averageScoresHistoryTimeline;


			std::vector<double> m_scoreParts;
			std::vector<const char*> m_scorePartsLabels;

			ParametersToColorFunc m_agentToColorFunc = nullptr;
		};
		Painter* m_painter = nullptr;


		ParametersTestFunc m_agentTestFunc = nullptr;
		
		

		std::vector<double> m_lastPopulationScores;
		std::vector<Agent> m_population;
		Agent m_alltimeBestAgent;
		Agent m_bestLastRoundAgent;
		//std::vector<Agent> m_nextGeneration;


		// Selection parameters
		size_t m_maxSelectionTryCount = 10;

		// Crossover parameters
		//size_t m_crossoverPoints = 1;


		// Mutatuion parameters
		double m_mutationPropability = 0.05;
		double m_mutationAmount = 0.01;

		bool m_useAdaptiveMutation = false;
		double m_tauPrime = 0.0;
		double m_tau = 0.0;
		double m_globalNoise = 0.0;
		double m_minimizingStaticOffset = 1e-6;

#ifdef GENETIC_SOLVER_USE_THREAD_POOL
		// Thread pool members
		std::vector<std::thread> m_workerThreads;
		size_t m_populationSizeOnSetup = 0;
		std::mutex m_mutex;
		std::condition_variable m_cvWork;
		std::condition_variable m_cvComplete;
		std::atomic<bool> m_stopThreads{ false };
		std::atomic<bool> *m_threadHasWork{ nullptr };
		std::atomic<size_t> m_completedThreads{ 0 };

		std::atomic<bool> m_threadsBusy{ false };

		void workerThread(size_t threadId, size_t start, size_t end);
		void initializeThreadPool(size_t numThreads);
		void shutdownThreadPool();
#endif
	};
}