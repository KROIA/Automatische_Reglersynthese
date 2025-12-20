#pragma once

#include "AutoTuner_base.h"
#include "MatlabAPI.h"

namespace AutoTuner
{
	class AUTO_TUNER_API TimeBasedSystem
	{
		typedef void (TimeBasedSystem::* ProcessTimeStepFunc)(const MatlabAPI::Matrix& u);
	public:
		/**
		 * @brief
		 * Integration solvers for continuous time state space models
		 * Euler: Forward Euler method
		 * Bilinear: Bilinear (Tustin) method
		 * Rk4: 4th-order Runge-Kutta method
		 * Discretized: Use the discretized version of the model (Ad, Bd, Cd, Dd), created using the specified C2DMethod
		 */
		enum class IntegrationSolver
		{
			ForwardEuler,
			BackwardEuler,
			Bilinear,
			Rk4,
			Discretized,
			Custom
		};
		enum class DifferentiationSolver
		{
			BackwardEuler,
			Custom
		};

		static void setDefaultIntegrationSolver(IntegrationSolver solver);
		static IntegrationSolver getDefaultIntegrationSolver();

		static void setDefaultDifferentiationSolver(DifferentiationSolver solver);
		static DifferentiationSolver getDefaultDifferentiationSolver();

		TimeBasedSystem();
		TimeBasedSystem(const TimeBasedSystem& other)
		{
			m_solver = other.m_solver;
			m_diffSolver = other.m_diffSolver;
		}
		virtual ~TimeBasedSystem();

		virtual TimeBasedSystem* clone() = 0;

		virtual void setIntegrationSolver(IntegrationSolver solver)
		{
			m_solver = solver;
		}
		IntegrationSolver getIntegrationSolver() const { return m_solver; }

		virtual void setDifferentiationSolver(DifferentiationSolver solver)
		{
			m_diffSolver = solver;
		}
		DifferentiationSolver getDifferentiationSolver() const { return m_diffSolver; }

		

		virtual void reset() {}

		virtual void setInputSignals(double u) = 0;
		virtual void setInputSignal(size_t input, double value) = 0;
		virtual void setInputSignals(const std::vector<double>& u) = 0;

		/**
		 * @brief
		 * Advances the system by the given time delta.
		 */
		virtual void update(double deltaTime) = 0;

		virtual std::vector<double> getInputs() const = 0;
		virtual std::vector<double> getOutputs() const = 0;

		virtual double getOutput(size_t index) const = 0;
		virtual double getInput(size_t index) const = 0;


		static std::string integrationSolverToString(IntegrationSolver solver)
		{
			using namespace std::string_literals;
			switch (solver)
			{
			case IntegrationSolver::ForwardEuler:        return "Forward Euler"s;
			case IntegrationSolver::BackwardEuler:       return "Backward Euler"s;
			case IntegrationSolver::Bilinear:			 return "Bilinear (Tustin)"s;
			case IntegrationSolver::Rk4:				 return "4th-order Runge-Kutta"s;
			case IntegrationSolver::Discretized:		 return "Discretized Model"s;
			case IntegrationSolver::Custom:				 return "Custom"s;
			}
			return "Unknown Solver"s;
		}
		static std::string differentiationSolverToString(DifferentiationSolver solver)
		{
			using namespace std::string_literals;
			switch (solver)
			{
			case DifferentiationSolver::BackwardEuler:        return "BackwardEuler Euler"s;
			case DifferentiationSolver::Custom:				 return "Custom"s;
			}
			return "Unknown Solver"s;
		}

		/**
		* Bilinear (Tustin) integration
		* @param uZ_1 Input signal at time z^(-1)
		* @param uZ_0 Input signal at time z^(0)
		* @param y_z1 Output signal at time z^(-1)
		* @param deltaTime Time step
		* @return Integrated output signal at time z^(0)
		*/
		static inline double getIntegrated_Bilinear(double uZ_1, double uZ_0, double yZ_1, double deltaTime)
		{
			// Bilinear (Tustin) approximation
			// y(k) = y(k-1) + (deltaT/2) * (u(k) + u(k-1))
			return yZ_1 + (deltaTime / 2.0) * (uZ_0 + uZ_1);
		}

		/**
		* Forward Euler integration
		* @param uZ_1 Input signal at time z^(-1)
		* @param yZ_1 Output signal at time z^(-1)
		* @param deltaTime Time step
		* @return Integrated output signal at time z^(0)
		*/
		static inline double getIntegrated_forwardEuler(double uZ_1, double yZ_1, double deltaTime)
		{
			// Forward Euler integration
			// y(k) = y(k-1) + deltaT * u(k)
			return yZ_1 + deltaTime * uZ_1;
		}

		/**
		* Backward Euler integration
		* @param uZ_0 Input signal at time z^(0)
		* @param yZ_1 Output signal at time z^(-1)
		* @param deltaTime Time step
		* @return Integrated output signal at time z^(0)
		*/
		static inline double getIntegrated_backwardEuler(double uZ_0, double yZ_1, double deltaTime)
		{
			// Backward Euler integration
			// y(k) = y(k-1) + deltaT * u(k-1)
			return yZ_1 + deltaTime * uZ_0;
		}

		/**
		* Backward Euler differentiation
		* @param uZ_1 Input signal at time z^(-1)
		* @param uZ_0 Input signal at time z^(0)
		* @param deltaTime Time step
		* @return Differentiated output signal at time z^(0)
		*/
		static inline double getDifferentiated_backwardEuler(double uZ_1, double uZ_0, double deltaTime)
		{
			// Backward Euler differentiation
			// y(k) = (u(k) - u(k-1)) / deltaT
			return (uZ_0 - uZ_1) / deltaTime;
		}

	protected:




	private:
		IntegrationSolver m_solver;
		DifferentiationSolver m_diffSolver;
		static IntegrationSolver s_defaultSolver;
		static DifferentiationSolver s_defaultDiffSolver;
	};
}