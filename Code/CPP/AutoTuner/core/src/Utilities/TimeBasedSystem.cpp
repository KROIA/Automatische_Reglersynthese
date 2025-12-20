#include "Utilities/TimeBasedSystem.h"


namespace AutoTuner
{

	TimeBasedSystem::IntegrationSolver TimeBasedSystem::s_defaultSolver = TimeBasedSystem::IntegrationSolver::BackwardEuler;
	TimeBasedSystem::DifferentiationSolver TimeBasedSystem::s_defaultDiffSolver = TimeBasedSystem::DifferentiationSolver::BackwardEuler;
	void TimeBasedSystem::setDefaultIntegrationSolver(IntegrationSolver solver) 
	{ 
		s_defaultSolver = solver; 
	}
	TimeBasedSystem::IntegrationSolver TimeBasedSystem::getDefaultIntegrationSolver()
	{ 
		return s_defaultSolver; 
	}

	void TimeBasedSystem::setDefaultDifferentiationSolver(DifferentiationSolver solver)
	{
		s_defaultDiffSolver = solver;
	}
	TimeBasedSystem::DifferentiationSolver TimeBasedSystem::getDefaultDifferentiationSolver()
	{
		return s_defaultDiffSolver;
	}

	TimeBasedSystem::TimeBasedSystem() 
	{
		//setIntegrationSolver(s_defaultSolver);
	}
	TimeBasedSystem::~TimeBasedSystem() 
	{}

}