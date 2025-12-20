#include "GameObjects/Solver.h"

namespace AutoTuner
{
	Solver::Solver(const std::string& name,
		QSFML::Objects::GameObject* parent)
		: QSFML::Objects::GameObject(name, parent)
	{

	}
}