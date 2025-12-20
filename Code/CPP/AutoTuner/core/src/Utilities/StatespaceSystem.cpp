#include "Utilities/StatespaceSystem.h"


namespace AutoTuner
{
	StatespaceSystem::StatespaceSystem()
		: TimeBasedSystem()
	{
		m_timeStep = 1;
		setIntegrationSolver(getDefaultIntegrationSolver());
		setDifferentiationSolver(getDefaultDifferentiationSolver());
	}
	StatespaceSystem::StatespaceSystem(const StatespaceSystem& other)
		: TimeBasedSystem()
		, m_A(other.m_A)
		, m_B(other.m_B)
		, m_C(other.m_C)
		, m_D(other.m_D)
		, m_x(other.m_x)
		, m_u(other.m_u)
		, m_lastXdot(other.m_lastXdot)

	{
		setIntegrationSolver(other.getIntegrationSolver());
	}
	StatespaceSystem::~StatespaceSystem()
	{}


	void StatespaceSystem::setStateSpaceMatrices(
		const MatlabAPI::Matrix& A,
		const MatlabAPI::Matrix& B,
		const MatlabAPI::Matrix& C,
		const MatlabAPI::Matrix& D)
	{
		m_A = A;
		m_B = B;
		m_C = C;
		m_D = D;

		m_u = MatlabAPI::Matrix(B.getCols(), 1);
		m_x = MatlabAPI::Matrix(A.getRows(), 1);
		m_lastXdot = MatlabAPI::Matrix(A.getRows(), 1);
	}

	void StatespaceSystem::setIntegrationSolver(IntegrationSolver solver)
	{
		TimeBasedSystem::setIntegrationSolver(solver);
		m_processTimeStepFunc = &StatespaceSystem::processTimeStepDiscretized;
		switch (solver)
		{
		case IntegrationSolver::Discretized:  m_processTimeStepFunc = &StatespaceSystem::processTimeStepDiscretized;    break;
		case IntegrationSolver::ForwardEuler:        m_processTimeStepFunc = &StatespaceSystem::processTimeStepForwardEuler;			break;
		case IntegrationSolver::Bilinear:     m_processTimeStepFunc = &StatespaceSystem::processTimeStepBilinear;		break;
		case IntegrationSolver::Rk4:          m_processTimeStepFunc = &StatespaceSystem::processTimeStepRk4;			break;
		case IntegrationSolver::Custom:       m_processTimeStepFunc = &StatespaceSystem::processTimeStepCustom;		    break;
		}
	}


	StatespaceSystem::SSData StatespaceSystem::getSSData() const
	{
		SSData data;
		data.inputCount = m_B.getCols();
		data.outputCount = m_C.getRows();
		data.stateCount = m_A.getRows();
		data.matricesData.clear();
		data.matricesData.reserve(
			m_A.getRows() * m_A.getCols() +
			m_B.getRows() * m_B.getCols() +
			m_C.getRows() * m_C.getCols() +
			m_D.getRows() * m_D.getCols()
		);
		// A matrix
		for (size_t i = 0; i < m_A.getRows(); i++)
		{
			for (size_t j = 0; j < m_A.getCols(); j++)
			{
				data.matricesData.push_back(m_A(i, j));
			}
		}
		// B matrix
		for (size_t i = 0; i < m_B.getRows(); i++)
		{
			for (size_t j = 0; j < m_B.getCols(); j++)
			{
				data.matricesData.push_back(m_B(i, j));
			}
		}
		// C matrix
		for (size_t i = 0; i < m_C.getRows(); i++)
		{
			for (size_t j = 0; j < m_C.getCols(); j++)
			{
				data.matricesData.push_back(m_C(i, j));
			}
		}
		// D matrix
		for (size_t i = 0; i < m_D.getRows(); i++)
		{
			for (size_t j = 0; j < m_D.getCols(); j++)
			{
				data.matricesData.push_back(m_D(i, j));
			}
		}
		return data;
	}
	bool StatespaceSystem::setSSData(const SSData& data)
	{
		size_t expectedSize =
			data.stateCount * data.stateCount +
			data.stateCount * data.inputCount +
			data.outputCount * data.stateCount +
			data.outputCount * data.inputCount;
		if (data.matricesData.size() != expectedSize)
		{
			return false;
		}
		size_t index = 0;
		// A matrix
		MatlabAPI::Matrix A(data.stateCount, data.stateCount);
		for (size_t i = 0; i < data.stateCount; i++)
		{
			for (size_t j = 0; j < data.stateCount; j++)
			{
				A(i, j) = data.matricesData[index++];
			}
		}
		// B matrix
		MatlabAPI::Matrix B(data.stateCount, data.inputCount);
		for (size_t i = 0; i < data.stateCount; i++)
		{
			for (size_t j = 0; j < data.inputCount; j++)
			{
				B(i, j) = data.matricesData[index++];
			}
		}
		// C matrix
		MatlabAPI::Matrix C(data.outputCount, data.stateCount);
		for (size_t i = 0; i < data.outputCount; i++)
		{
			for (size_t j = 0; j < data.stateCount; j++)
			{
				C(i, j) = data.matricesData[index++];
			}
		}
		// D matrix
		MatlabAPI::Matrix D(data.outputCount, data.inputCount);
		for (size_t i = 0; i < data.outputCount; i++)
		{
			for (size_t j = 0; j < data.inputCount; j++)
			{
				D(i, j) = data.matricesData[index++];
			}
		}
		setStateSpaceMatrices(A, B, C, D);
		return true;
	}

	void StatespaceSystem::processTimeStepDiscretized(const MatlabAPI::Matrix& u)
	{
		//AT_GENERAL_PROFILING_FUNCTION(AT_COLOR_STAGE_5);
		m_x = m_A * m_x + m_B * u;
	}
	void StatespaceSystem::processTimeStepForwardEuler(const MatlabAPI::Matrix& u)
	{
		//AT_GENERAL_PROFILING_FUNCTION(AT_COLOR_STAGE_5);
		m_x += (m_A * m_x + m_B * u) * m_timeStep;
	}
	void StatespaceSystem::processTimeStepBilinear(const MatlabAPI::Matrix& u)
	{
		//AT_GENERAL_PROFILING_FUNCTION(AT_COLOR_STAGE_5);
		auto xDot = (m_A * m_x + m_B * u);
		m_x += (xDot + m_lastXdot) * (m_timeStep / 2.0);
		m_lastXdot = xDot;
	}

	void StatespaceSystem::processTimeStepRk4(const MatlabAPI::Matrix& u)
	{
		//AT_GENERAL_PROFILING_FUNCTION(AT_COLOR_STAGE_5);
		auto f = [&](const MatlabAPI::Matrix& x_in) {
			return m_A * x_in + m_B * u;
			};

		double timestep2 = m_timeStep / 2.0;

		MatlabAPI::Matrix k1 = f(m_x);
		MatlabAPI::Matrix k2 = f(m_x + k1 * timestep2);
		MatlabAPI::Matrix k3 = f(m_x + k2 * timestep2);
		MatlabAPI::Matrix k4 = f(m_x + k3 * m_timeStep);

		m_x += (k1 + k2 * 2.0 + k3 * 2.0 + k4) * (m_timeStep / 6.0);
	}
}