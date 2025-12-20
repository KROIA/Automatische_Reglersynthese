#include "GameObjects/GeneticSolver.h"
#include <thread>
#include "implot_internal.h"

namespace AutoTuner
{
	GeneticSolver::GeneticSolver(const std::string& name,
		QSFML::Objects::GameObject* parent)
		: Solver(name, parent)
	{
		m_painter = new Painter("GeneticSolverPainter");
		addComponent(m_painter);

/*#ifdef GENETIC_SOLVER_USE_THREAD_POOL
		unsigned int numThreads = std::thread::hardware_concurrency();
		if (numThreads == 0) numThreads = 4;
		initializeThreadPool(8);
#endif*/
	}
	GeneticSolver::~GeneticSolver()
	{
#ifdef GENETIC_SOLVER_USE_THREAD_POOL
		shutdownThreadPool();
#endif
	}


	void GeneticSolver::setInitialParameters(const std::vector<std::vector<double>>& parameterList)
	{
#ifdef GENETIC_SOLVER_USE_THREAD_POOL
		if (m_workerThreads.size() != 0)
		{
			shutdownThreadPool();
		}
#endif
		m_population.clear();
		for (const auto& params : parameterList)
		{
			Agent agent;
			agent.parameters = params;
			agent.mutationFactors.resize(params.size(), 0.1);
			m_population.push_back(agent);
		}
		m_bestLastRoundAgent = Agent();
		m_alltimeBestAgent = Agent();

		if (m_optimizingDirection == OptimizingDirection::Minimize)
		{
			m_bestLastRoundAgent.score = std::numeric_limits<double>::infinity();
			m_alltimeBestAgent.score = std::numeric_limits<double>::infinity();
		}


		m_tauPrime = 1.0 / std::sqrt(2.0 * std::sqrt(static_cast<double>(m_population[0].parameters.size())));
		m_tau = 1.0 / std::sqrt(2.0 * static_cast<double>(m_population[0].parameters.size()));
#ifdef GENETIC_SOLVER_USE_THREAD_POOL
		unsigned int numThreads = std::thread::hardware_concurrency();
		if (numThreads == 0) numThreads = 4;
		initializeThreadPool(numThreads);
#endif
	}

	/*void GeneticSolver::setPopulation(const std::vector<Agent>& population)
	{
		m_population = population;
		if (m_population.size() == 0)
			return;
		for(size_t i=0; i< m_population.size(); ++i)
		{
			m_population[i].mutationFactors.resize(m_population[i].parameters.size(), 0.1);
		}
		
		m_tauPrime = 1.0 / std::sqrt(2.0 * std::sqrt(static_cast<double>(m_population[0].parameters.size())));
		m_tau = 1.0 / std::sqrt(2.0 * static_cast<double>(m_population[0].parameters.size()));
#ifdef GENETIC_SOLVER_USE_THREAD_POOL
		unsigned int numThreads = std::thread::hardware_concurrency();
		if (numThreads == 0) numThreads = 4;
		initializeThreadPool(numThreads);
#endif
	}*/



	void GeneticSolver::iterate()
	{
		
		std::vector<Agent> nextGeneration(m_population.size());

		double sumScores = 0.0;
		sortPopulation(m_population);
		m_painter->setPopulation(m_population, sumScores);

		m_lastPopulationScores.clear();
		m_lastPopulationScores.reserve(m_population.size());

		switch (m_optimizingDirection)
		{
			case OptimizingDirection::Minimize:
			{
				double maxLoss = -std::numeric_limits<double>::infinity();
				double minLoss = -maxLoss;
				size_t bestIndex = 0;
				for (size_t i = 0; i < m_population.size(); ++i)
				{
					const auto& agent = m_population[i];
					maxLoss = std::max(maxLoss, agent.score);
					if (agent.score < minLoss)
					{
						minLoss = agent.score;
						bestIndex = i;
					}
					m_lastPopulationScores.push_back(agent.score);
				}
				m_bestLastRoundAgent = m_population[bestIndex];
				double scoreOffest = 0;
				if (minLoss < 0)
				{
					// Move all scores into positive range
					scoreOffest = -minLoss;
					maxLoss -= minLoss;
				}

				// Invert scores for minimization
				for (auto& agent : m_population)
				{
					agent.score = maxLoss - (agent.score + scoreOffest) + m_minimizingStaticOffset;
					sumScores += agent.score;
					//m_lastPopulationScores.push_back(agent.score);
				}

				
				break;
			}
			case  OptimizingDirection::Maximize:
			{
				//double maxSore = -std::numeric_limits<double>::infinity();
				size_t bestIndex = 0;
				double bestScore = -std::numeric_limits<double>::infinity();
				for (size_t i=0; i< m_population.size(); ++i)
				{
					//maxSore = std::max(maxSore, agent.score);
					const auto& agent = m_population[i];
					sumScores += agent.score;
					m_lastPopulationScores.push_back(agent.score);

					if (agent.score > bestScore)
					{
						bestScore = agent.score;
						bestIndex = i;
					}
				}
				m_bestLastRoundAgent = m_population[bestIndex];
				break;
			}
		}

		

		m_globalNoise = m_tauPrime * getRandomDouble(-1, 1);
		for (size_t i = 0; i < m_population.size(); i+=2)
		{
			auto [parent1Idx, parent2Idx] = selectParents(m_population, sumScores);
			const Agent& parent1 = m_population[parent1Idx];
			const Agent& parent2 = m_population[parent2Idx];

			Agent& offspring1 = nextGeneration[i];
			Agent& offspring2 = nextGeneration[i+1];

			crossover(parent1, parent2, offspring1, offspring2);
			mutate(offspring1);
			mutate(offspring2);
		}

	
		m_population = nextGeneration;
		
	}

	void GeneticSolver::test()
	{
		if (!m_agentTestFunc || m_population.empty())
			return;

		//auto startTime = std::chrono::high_resolution_clock::now();

#ifdef GENETIC_SOLVER_USE_THREAD_POOL
		m_threadsBusy = true;
		// Reset completion counter
		
		m_completedThreads = 0;
		//m_hasWork = m_workerThreads.size();
		for(size_t i=0; i< m_workerThreads.size(); ++i)
		{
			m_threadHasWork[i].store(true);
		}
		//system("cls");
		//qDebug() << " \n\n";
		//qDebug() << "Starting work on " << m_hasWork.load() << " threads.";
		

		// Wake up all worker threads
		AT_GENERAL_PROFILING_NONSCOPED_BLOCK("Notify Worker Threads", AT_COLOR_STAGE_2);
		m_cvWork.notify_all();
		AT_GENERAL_PROFILING_END_BLOCK;

		// Wait for all threads to complete their work
		{
			AT_GENERAL_PROFILING_NONSCOPED_BLOCK("WaitForFinishedWork", AT_COLOR_STAGE_2);
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cvComplete.wait(lock, [this] {
				for (size_t i = 0; i < m_workerThreads.size(); ++i)
				{
					if(m_threadHasWork[i].load())
						return false;
				}
				return true;
				});
			AT_GENERAL_PROFILING_END_BLOCK;
		//	qDebug() << "All threads completed work.";
			AT_GENERAL_PROFILING_NONSCOPED_BLOCK("Notify Worker Threads", AT_COLOR_STAGE_2);
			m_cvWork.notify_all();
			AT_GENERAL_PROFILING_END_BLOCK;
		}
		for (auto& agent : m_population)
		{
			if (m_optimizingDirection == OptimizingDirection::Maximize && agent.score > m_alltimeBestAgent.score ||
				m_optimizingDirection == OptimizingDirection::Minimize && agent.score < m_alltimeBestAgent.score)
			{
				m_alltimeBestAgent = agent;
			}
		}

		m_threadsBusy = false;
		
		//std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Give some time for threads to go to sleep
#else
		
		size_t agentIndex = 0;
		for (auto& agent : m_population)
		{
			agent.scoreParts = m_agentTestFunc(agent.parameters, agentIndex);
			++agentIndex;
			agent.score = 0;
			for(const auto& sc : agent.scoreParts)
				agent.score += sc;

			if(m_optimizingDirection == OptimizingDirection::Maximize && agent.score > m_alltimeBestAgent.score ||
			   m_optimizingDirection == OptimizingDirection::Minimize && agent.score < m_alltimeBestAgent.score)
			{
				m_alltimeBestAgent = agent;
			}
			
		}
#endif
		//auto endTime = std::chrono::high_resolution_clock::now(); //end measurement here
		//auto elapsed = endTime - startTime;
		//double elapsedMs = std::chrono::duration<double, std::milli>(elapsed).count();
		//static double averageMs = 0.0;
		//averageMs = (averageMs * 0.9) + (elapsedMs * 0.1);
		//qDebug() << "GeneticSolver Test Time: " << elapsedMs << " ms (Avg: " << averageMs << " ms)";
	}

	void GeneticSolver::sortPopulation(std::vector<Agent>& population)
	{
		std::sort(population.begin(), population.end(),
			[](const Agent& a, const Agent& b)
			{
				return a.score > b.score;
			});
	}
	std::pair<size_t, size_t> GeneticSolver::selectParents(const std::vector<Agent>& sortedPopulation, double sumScore)
	{
		size_t parent1 = 0;
		size_t parent2 = 0;
		
		double rand1 = getRandomDouble(0, sumScore);
		// Find first parent
		double cumulativeScore = 0.0;
		for (size_t i = 0; i < sortedPopulation.size(); ++i)
		{
			cumulativeScore += sortedPopulation[i].score;
			if (cumulativeScore >= rand1)
			{
				parent1 = i;
				break;
			}
		}
		double rand2;
		// Find second parent
		size_t tryCount = 0;
		do {
			cumulativeScore = 0.0;
			rand2 = getRandomDouble(0.0, sumScore);
			for (size_t i = 0; i < sortedPopulation.size(); ++i)
			{
				cumulativeScore += sortedPopulation[i].score;
				if (cumulativeScore >= rand2)
				{
					parent2 = i;
					break;
				}
			}
			++tryCount;
			if(tryCount > m_maxSelectionTryCount)
				break;
		} while (parent1 == parent2);
		return { parent1, parent2 };
	}
	void GeneticSolver::mutate(Agent& agent)
	{
		for (size_t i=0; i<agent.parameters.size(); ++i)
		{
			double &mutationFactor = m_mutationAmount;
			if (m_useAdaptiveMutation)
			{
				mutationFactor = agent.mutationFactors[i];
				mutationFactor *= std::exp(m_globalNoise + m_tau * getRandomDouble(-1, 1));
			}

			double randVal = getRandomDouble(0.0, 1.0);
			if (randVal < m_mutationPropability)
			{
				double mutation = getRandomDouble(-1,1) * mutationFactor;
#ifdef GENETIC_SOLVER_USE_INDIVIDUAL_PARAMETER_MUTATION_RATE
				mutation *= std::abs(agent.parameters[i]+0.1);
#endif
				agent.parameters[i] += mutation;
			}
		}
	}
	void GeneticSolver::crossover(const Agent& parent1, const Agent& parent2, Agent& offspring1, Agent& offspring2)
	{
		size_t paramCount = parent1.parameters.size();
		size_t crossoverPoint = getRandomSizeT(1, paramCount - 1);
		offspring1.parameters.resize(paramCount);
		offspring2.parameters.resize(paramCount);

		memcpy(offspring1.parameters.data(), parent1.parameters.data(), crossoverPoint * sizeof(double));
		memcpy(offspring1.parameters.data() + crossoverPoint, parent2.parameters.data() + crossoverPoint,
			(paramCount - crossoverPoint) * sizeof(double));

		memcpy(offspring2.parameters.data(), parent2.parameters.data(), crossoverPoint * sizeof(double));
		memcpy(offspring2.parameters.data() + crossoverPoint, parent1.parameters.data() + crossoverPoint,
			(paramCount - crossoverPoint) * sizeof(double));

		if (m_useAdaptiveMutation)
		{
			offspring1.mutationFactors.resize(paramCount);
			memcpy(offspring1.mutationFactors.data(), parent1.mutationFactors.data(), crossoverPoint * sizeof(double));
			memcpy(offspring1.mutationFactors.data() + crossoverPoint, parent2.mutationFactors.data() + crossoverPoint,
				(paramCount - crossoverPoint) * sizeof(double));

			offspring2.mutationFactors.resize(paramCount);
			memcpy(offspring2.mutationFactors.data(), parent1.mutationFactors.data(), crossoverPoint * sizeof(double));
			memcpy(offspring2.mutationFactors.data() + crossoverPoint, parent2.mutationFactors.data() + crossoverPoint,
				(paramCount - crossoverPoint) * sizeof(double));
		}
		/*for (size_t i = 0; i < paramCount; ++i)
		{
			if (i < crossoverPoint)
			{
				offspring.parameters[i] = parent1.parameters[i];
			}
			else
			{
				offspring.parameters[i] = parent2.parameters[i];
			}
		}*/
	}


#ifdef GENETIC_SOLVER_USE_THREAD_POOL
	void GeneticSolver::initializeThreadPool(size_t numThreads)
	{
		if (m_workerThreads.size() != 0)
		{
			shutdownThreadPool();
		}
		m_stopThreads = false;
		//m_hasWork = 0;
		m_workerThreads.reserve(numThreads);

		// Calculate fixed ranges for each thread
		size_t populationSize = m_population.size();
		numThreads = std::min(numThreads, populationSize);
		numThreads = std::min(numThreads, s_maxThreadWorkerCount);
		size_t chunkSize = (populationSize + numThreads - 1) / numThreads;

		m_populationSizeOnSetup = populationSize;

		m_threadHasWork = new std::atomic<bool>[numThreads];

		for (unsigned int i = 0; i < numThreads; ++i)
		{
			size_t start = i * chunkSize;
			size_t end = std::min(start + chunkSize, populationSize);
			
			m_threadHasWork[i].store(false);
			m_workerThreads.emplace_back(&GeneticSolver::workerThread, this, i, start, end);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(30)); // Give some time for threads to go to sleep
		qDebug() << "GeneticSolver: Initialized thread pool with " << numThreads << " threads.";
	}

	void GeneticSolver::workerThread(size_t threadId, size_t start, size_t end)
	{
		AT_PROFILING_THREAD("GeneticSolver Worker Thread");
		qDebug() << "GeneticSolver: Worker thread " << threadId << " started. Processing agents " << start << " to " << end - 1;
		std::vector<Agent*> localAgents;
		for (size_t i = start; i < end && i < m_population.size(); ++i)
		{
			localAgents.push_back(&m_population[i]);
		}
		//bool isLast = false;
		std::atomic<bool>& threadHasWork = m_threadHasWork[threadId];
		while (true)
		{
			{
				AT_GENERAL_PROFILING_BLOCK("GeneticSolver Worker Thread Wait", AT_COLOR_STAGE_1);
				std::unique_lock<std::mutex> lock(m_mutex);
				// Wait for work or shutdown signal
				
				//if (!isLast)
				//{
				//	m_cvWork.wait(lock, [this] {
				//		return !threadHasWork || m_stopThreads;
				//		});
				//}
				//isLast = false;

				//qDebug() << "GeneticSolver: Worker thread " << threadId << " waiting for work...";
				// Wait for work or shutdown signal
				m_cvWork.wait(lock, [this, &threadHasWork] {
					return m_stopThreads || threadHasWork.load();
					});

				//qDebug() << "GeneticSolver: Worker thread " << threadId << " woke up.";
				if (m_stopThreads)
				{
					qDebug() << "GeneticSolver: Worker thread " << threadId << " stopping.";
					return;
				}
			}

			// Process the fixed range assigned to this thread
			AT_GENERAL_PROFILING_NONSCOPED_BLOCK("Agents Test", AT_COLOR_STAGE_1);
			for (size_t i = 0; i < localAgents.size(); ++i)
			{
				auto& agent = localAgents[i];
				agent->scoreParts = m_agentTestFunc(agent->parameters, start+i);
				agent->score = 0;
				for (const auto& sc : agent->scoreParts)
					agent->score += sc;
			}
			AT_GENERAL_PROFILING_END_BLOCK;

			if (m_stopThreads)
			{
				qDebug() << "GeneticSolver: Worker thread " << threadId << " stopping.";
				return;
			}

			// Signal completion
			AT_GENERAL_PROFILING_NONSCOPED_BLOCK("Notify completion", AT_COLOR_STAGE_1);
			//size_t completed = ++m_completedThreads;
			//if (completed == m_workerThreads.size())
			//qDebug() << "GeneticSolver: Worker thread " << threadId << " completed work.";
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				threadHasWork = false;
				m_cvComplete.notify_one();
			}
			AT_GENERAL_PROFILING_END_BLOCK;
			
		}
		qDebug() << "GeneticSolver: Worker thread " << threadId << " stopping2.";
	}
	void GeneticSolver::shutdownThreadPool()
	{
		qDebug() << "GeneticSolver: Shutting down thread pool...";
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_stopThreads = true;
		}
		m_cvWork.notify_all();

		for (auto& thread : m_workerThreads)
		{
			if (thread.joinable())
				thread.join();
		}
		qDebug() << "GeneticSolver: Thread pool shut down.";
		m_workerThreads.clear();

		delete[] m_threadHasWork;
		m_threadHasWork = nullptr;
	}
#endif




	//
	// GeneticSolver::Painter
	// 

	void GeneticSolver::Painter::setPopulation(const std::vector<Agent>& agents, double sumScore)
	{
		m_population = agents;
		m_piChartLabels = std::vector<char*>(agents.size(), nullptr);
		m_piChartData = std::vector<float>(agents.size(), 0.0f);


		if (sumScore == 0)
		{
			for(size_t i = 0; i < agents.size(); ++i)
			{
				sumScore += agents[i].score;
			}
		}


		for (size_t i = 0; i < agents.size(); ++i)
		{
			std::string label = "Agent " + std::to_string(i) + " (Score: " + std::to_string(agents[i].score) + ")";
			m_piChartLabels[i] = _strdup(label.c_str());
			m_piChartData[i] = static_cast<float>(agents[i].score * 100.0/ sumScore);

			for (size_t j = 0; j < m_scoreParts.size(); ++j)
			{
				m_scoreParts[j] += agents[i].scoreParts[j];
			}
		}
		if (agents.size() > 0)
		{
			double invAgentsCount = 1.0 / agents.size();
			m_averageScoresHistory.push_back(sumScore * invAgentsCount);
			double sumScores = 0;
			for (size_t j = 0; j < m_scoreParts.size(); ++j)
			{
				sumScores += m_scoreParts[j];
			}
			sumScores = 1.0 / sumScores;
			for (size_t j = 0; j < m_scoreParts.size(); ++j)
			{
				m_scoreParts[j] *= sumScores;
			}
			double time = 0;
			if (m_averageScoresHistoryTimeline.size() > 0)
				time = m_averageScoresHistoryTimeline[m_averageScoresHistoryTimeline.size() - 1] + 1;
			m_averageScoresHistoryTimeline.push_back(time);
			if (m_averageScoresHistory.size() > m_historySize)
			{
				m_averageScoresHistory.erase(m_averageScoresHistory.begin());
				m_averageScoresHistoryTimeline.erase(m_averageScoresHistoryTimeline.begin());
			}
		}
	}
	void GeneticSolver::Painter::setScorePartsLabels(const std::vector<std::string>& labels) 
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

	void GeneticSolver::Painter::drawComponent(sf::RenderTarget& target, sf::RenderStates states) const
	{
		ImGui::Begin("Genetic Solver Population");

		if (ImPlot::BeginPlot("Pie Chart", ImVec2(400, 400), ImPlotFlags_NoLegend)) {
			const int n = (int)m_piChartData.size()*2;
			static std::vector<ImVec4> colors;
			colors.resize(n);

			static ImPlotColormap cmap = -1;

			if (cmap == -1) {
				colors.resize(n);
				ImVec4 startColor = ImVec4(0.0f, 0.2f, 0.8f, 1.0f); // dark blue
				ImVec4 endColor = ImVec4(0.4f, 0.7f, 1.0f, 1.0f); // light blue
				for (int i = 0; i < n; ++i) {
					float t = (float)i / (float)std::max(1, n - 1);
					colors[i].x = startColor.x + t * (endColor.x - startColor.x);
					colors[i].y = startColor.y + t * (endColor.y - startColor.y);
					colors[i].z = startColor.z + t * (endColor.z - startColor.z);
					colors[i].w = 1.0f;
				}

				// Register the colormap once
				cmap = ImPlot::AddColormap("BlueGradient", colors.data(), n);
			}

			// Use this colormap temporarily
			ImPlot::PushColormap(cmap);
			ImPlot::GetCurrentPlot()->Items.ColormapIdx = 0;

			ImPlot::PlotPieChart(
				m_piChartLabels.data(),
				m_piChartData.data(),
				m_piChartLabels.size(),
				0.5, 0.5,   // center
				0.4,        // radius
				"%.1f%%",   // label format
				90.0        // start angle
			);

			ImPlot::PopColormap();
			ImPlot::EndPlot();
		}


		if (ImPlot::BeginPlot("Average score history", ImVec2(-1, 200))) {
			ImPlot::SetupAxes("Iteration", "Average score");
			int dataSize = m_averageScoresHistory.size();

			if (dataSize > 0) {

				// Set axis limits based on actual x values
				double xMin = m_averageScoresHistoryTimeline[0];
				double xMax = m_averageScoresHistoryTimeline[m_averageScoresHistoryTimeline.size()-1];

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
				double size = agent.score*0.1;
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