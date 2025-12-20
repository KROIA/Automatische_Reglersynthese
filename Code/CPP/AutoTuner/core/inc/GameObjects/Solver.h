#pragma once

#include "AutoTuner_base.h"


namespace AutoTuner
{
	class AUTO_TUNER_API Solver : public QSFML::Objects::GameObject
	{
	public:
		enum class OptimizingDirection
		{
			Minimize,
			Maximize
		};

		typedef std::function<sf::Color(const std::vector<double>&)> ParametersToColorFunc;
		typedef std::function<std::vector<double>(const std::vector<double>&, size_t)> ParametersTestFunc;

		Solver(const std::string& name = "Solver",
			   GameObject* parent = nullptr);

		virtual void setOptimizingDirection(OptimizingDirection direction)
		{
			m_optimizingDirection = direction;
		}
		OptimizingDirection getOptimizingDirection() const
		{
			return m_optimizingDirection;
		}


		virtual void setInitialParameters(const std::vector<std::vector<double>>& parameterList) = 0;

		/**
		 * @brief 
		 * Changes the parameters of the system being tested.
		 */
		virtual void iterate() = 0;

		/**
		 * @brief
		 * Tests the current parameters of the system.
		 */
		virtual void test() = 0;

		virtual void setMutationAmount(double amount) = 0;
		virtual double getMutationAmount() const = 0;

		virtual void setParametersToColorFunc(ParametersToColorFunc func) = 0;
		virtual void setParametersTestFunc(ParametersTestFunc func) = 0;
		virtual void setScorePartsLabels(const std::vector<std::string>& labels) = 0;

		virtual std::vector<double> getAlltimeBestParameters() const = 0;
		virtual std::vector<double> getBestParameters() const = 0;

		virtual void clearAlltimeBestParameters() = 0;

		virtual std::vector<double> getScores() = 0;


		static double getRandomDouble(double min, double max)
		{
			return min + static_cast<double>(rand()) / RAND_MAX * (max - min);
		}
		static double getRandomGaussDouble(double min, double max)
		{
			// Using Box-Muller transform
			double u1 = static_cast<double>(rand()) / RAND_MAX;
			double u2 = static_cast<double>(rand()) / RAND_MAX;
			double z0 = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
			// Scale and shift to desired range
			double mean = (min + max) / 2.0;
			double stddev = (max - min) / 6.0; // 99.7% of values within [min, max]
			double value = z0 * stddev + mean;
			// Clamp to [min, max]
			if (value < min) value = min;
			if (value > max) value = max;
			return value;
		}

		// Inclusive
		static size_t getRandomSizeT(size_t min, size_t max)
		{
			return min + static_cast<size_t>(rand()) % (max - min + 1);
		}

	protected:
		OptimizingDirection m_optimizingDirection = OptimizingDirection::Maximize;

	private:

	};
}