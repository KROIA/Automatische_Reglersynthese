#pragma once

#include "AutoTuner_base.h"
#include "Utilities/TimeBasedSystem.h"

namespace AutoTuner
{
	class AUTO_TUNER_API StatespaceSystem: public TimeBasedSystem
	{
		typedef void (StatespaceSystem::* ProcessTimeStepFunc)(const MatlabAPI::Matrix& u);
	public:
		struct SSData
		{
			size_t inputCount = 0;
			size_t outputCount = 0;
			size_t stateCount = 0;
			std::vector<double> matricesData;

			SSData() = default;
			SSData(size_t inCount, size_t outCount, size_t stateCount)
				: inputCount(inCount)
				, outputCount(outCount)
				, stateCount(stateCount)
			{
				size_t elementsCount = 					
					stateCount * stateCount +          // A matrix
					stateCount * inCount +             // B matrix
					outCount * stateCount +            // C matrix
					outCount * inCount;                // D matrix

				matricesData.resize(elementsCount, 0.0);
			}
		};

		StatespaceSystem();
		StatespaceSystem(const StatespaceSystem &other);
		virtual ~StatespaceSystem();
		TimeBasedSystem* clone() override
		{
			return new StatespaceSystem(*this);
		}


		void setStateSpaceMatrices(
			const MatlabAPI::Matrix& A,
			const MatlabAPI::Matrix& B,
			const MatlabAPI::Matrix& C,
			const MatlabAPI::Matrix& D);

		void setIntegrationSolver(IntegrationSolver solver) override;

		void reset() override
		{
			setStates(0.0);
		}


		void setInputSignals(double u) override
		{
			for (size_t i = 0; i < m_u.getRows(); i++)
			{
				m_u(i, 0) = u;
			}
		}
		void setInputSignal(size_t input, double value) override
		{
			if (input < m_u.getRows())
			{
				m_u(input, 0) = value;
			}
		}
		void setInputSignals(const std::vector<double>& u) override
		{
			size_t inputSize = std::min(u.size(), m_u.getRows());
			for (size_t i = 0; i < inputSize; i++)
			{
				m_u(i, 0) = u[i];
			}
		}

		/**
		 * @brief
		 * Advances the system by the given time delta.
		 */
		void update(double deltaTime) override
		{
			//AT_GENERAL_PROFILING_FUNCTION(AT_COLOR_STAGE_5);
			m_timeStep = deltaTime;
			processTimeStep(m_u);
		}

		std::vector<double> getInputs() const override
		{
			std::vector<double> inputs;
			for (size_t i = 0; i < m_u.getRows(); i++)
			{
				inputs.push_back(m_u(i, 0));
			}
			return inputs;
		}

		void setStates(double x)
		{
			for (size_t i = 0; i < m_x.getRows(); i++)
			{
				m_x(i, 0) = x;
			}
		}
		void setStates(const std::vector<double>& x)
		{
			size_t stateSize = std::min(x.size(), m_x.getRows());
			for (size_t i = 0; i < stateSize; i++)
			{
				m_x(i, 0) = x[i];
			}
		}
		std::vector<double> getStates() const
		{
			std::vector<double> states;
			for (size_t i = 0; i < m_x.getRows(); i++)
			{
				states.push_back(m_x(i, 0));
			}
			return states;
		}
		std::vector<double> getOutputs() const override
		{
			MatlabAPI::Matrix y = m_C * m_x + m_D * m_u;
			std::vector<double> outputs;
			for (size_t i = 0; i < y.getRows(); i++)
			{
				outputs.push_back(y(i, 0));
			}
			return outputs;
		}
		double getOutput(size_t index) const override
		{
			MatlabAPI::Matrix y = m_C * m_x + m_D * m_u;
			if (index < y.getRows())
			{
				return y(index, 0);
			}
			return 0.0;
		}
		double getInput(size_t index) const override
		{
			if (index < m_u.getRows())
			{
				return m_u(index, 0);
			}
			return 0.0;
		}

		const MatlabAPI::Matrix& getMatrixA() const { return m_A; }
		const MatlabAPI::Matrix& getMatrixB() const { return m_B; }
		const MatlabAPI::Matrix& getMatrixC() const { return m_C; }
		const MatlabAPI::Matrix& getMatrixD() const { return m_D; }


		void setMatrixA(const MatlabAPI::Matrix& A) 
		{ 
			m_A = A; 

			//m_u = MatlabAPI::Matrix(B.getCols(), 1);
			m_x = MatlabAPI::Matrix(A.getRows(), 1);
			m_lastXdot = MatlabAPI::Matrix(A.getRows(), 1);
		}
		void setMatrixB(const MatlabAPI::Matrix& B) 
		{ 
			m_B = B; 

			m_u = MatlabAPI::Matrix(B.getCols(), 1);
			//m_x = MatlabAPI::Matrix(A.getRows(), 1);
			//m_lastXdot = MatlabAPI::Matrix(A.getRows(), 1);
		}
		void setMatrixC(const MatlabAPI::Matrix& C) { m_C = C; }
		void setMatrixD(const MatlabAPI::Matrix& D) { m_D = D; }


		SSData getSSData() const;
		bool setSSData(const SSData& data);

		
	protected:

		/**
		 * @brief Automatically process one time step using the selected integration solver
		 * @param u input of the system. Must be a column vector with size equal to the number of inputs of the system (B.cols)
		 */
		void processTimeStep(const MatlabAPI::Matrix& u)
		{
			(this->*m_processTimeStepFunc)(u);
		}

		virtual void processTimeStepCustom(const MatlabAPI::Matrix& u) {}

		/**
		 * @brief Explicitly process one time step using the discretized model
		 * @param u input of the system. Must be a column vector with size equal to the number of inputs of the system (B.cols)
		 */
		void processTimeStepDiscretized(const MatlabAPI::Matrix& u);

		/**
		 * @brief Explicitly process one time step using the Forward Euler method
		 * @param u input of the system. Must be a column vector with size equal to the number of inputs of the system (B.cols)
		 */
		void processTimeStepForwardEuler(const MatlabAPI::Matrix& u);

		/**
		 * @brief Explicitly process one time step using the Bilinear (Tustin) method
		 * @param u input of the system. Must be a column vector with size equal to the number of inputs of the system (B.cols)
		 */
		void processTimeStepBilinear(const MatlabAPI::Matrix& u);

		/**
		 * @brief Explicitly process one time step using the 4th-order Runge-Kutta method
		 * @param u input of the system. Must be a column vector with size equal to the number of inputs of the system (B.cols)
		 */
		void processTimeStepRk4(const MatlabAPI::Matrix& u);


	private:
		MatlabAPI::Matrix m_A;
		MatlabAPI::Matrix m_B;
		MatlabAPI::Matrix m_C;
		MatlabAPI::Matrix m_D;

		MatlabAPI::Matrix m_u;
		MatlabAPI::Matrix m_x;
		MatlabAPI::Matrix m_lastXdot;

		
		ProcessTimeStepFunc m_processTimeStepFunc;
		double m_timeStep;

		
	};
}