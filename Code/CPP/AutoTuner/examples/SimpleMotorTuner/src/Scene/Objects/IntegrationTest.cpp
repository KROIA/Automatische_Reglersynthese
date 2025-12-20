#include "Scene/Objects/IntegrationTest.h"


IntegrationTest::IntegrationTest(const std::string& name,
	GameObject* parent)
	: GameObject(name, parent)
{
	{
		Sun* sun = new Sun("Sun1");
		sun->setPosition(sf::Vector2f(500.f, 500.f));
		addChild(sun);
		m_suns.push_back(sun);
	}
	//{
	//	Sun* sun = new Sun("Sun2");
	//	sun->setPosition(sf::Vector2f(800.f, 500.f));
	//	addChild(sun);
	//	m_suns.push_back(sun);
	//}

	{
		Planet* planet = new Planet("Planet1");
		planet->setPosition(sf::Vector2f(500.f, 300.f));
		planet->setVelocity(sf::Vector2f(15.f, 0.f));
		planet->setColor(sf::Color::Green);
		addChild(planet);
		m_planets.push_back(planet);
	}
	{
		Planet* planet = new Planet("Planet2");
		AutoTuner::StatespaceSystem& ss = planet->getStatespaceSystem();
		ss.setIntegrationSolver(AutoTuner::StatespaceSystem::IntegrationSolver::ForwardEuler);

		MatlabAPI::Matrix A({ 
			{ 0, 0, 0, 0 }, 
			{ 0, 0, 0, 0 }, 
			{ 1, 0, 0, 0 }, 
			{ 0, 1, 0, 0 } 
			});
		MatlabAPI::Matrix B({ 
			{ 1.0 / planet->getMass(), 0},
			{ 0, 1.0 / planet->getMass()},
			 {0, 0}, 
			 {0, 0} 
			});
		MatlabAPI::Matrix C({ { 0, 0, 1, 0 }, { 0, 0, 0, 1 } });
		MatlabAPI::Matrix D({ { 0, 0 }, {0, 0 } });
		ss.setStateSpaceMatrices(A, B, C, D);
		ss.setStates({ 15, 0.0, 500.0, 300.0 });

		planet->setPosition(sf::Vector2f(500.f, 300.f));
		planet->setVelocity(sf::Vector2f(15.f, 0.f));
		planet->setColor(sf::Color(255,0,0));
		addChild(planet);
		m_planets.push_back(planet);
	}
	{
		Planet* planet = new Planet("Planet3");
		AutoTuner::StatespaceSystem& ss = planet->getStatespaceSystem();
		ss.setIntegrationSolver(AutoTuner::StatespaceSystem::IntegrationSolver::Bilinear);

		MatlabAPI::Matrix A({
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 0 }
			});
		MatlabAPI::Matrix B({
			{ 1.0 / planet->getMass(), 0},
			{ 0, 1.0 / planet->getMass()},
			 {0, 0},
			 {0, 0}
			});
		MatlabAPI::Matrix C({ { 0, 0, 1, 0 }, { 0, 0, 0, 1 } });
		MatlabAPI::Matrix D({ { 0, 0 }, {0, 0 } });
		ss.setStateSpaceMatrices(A, B, C, D);
		ss.setStates({ 15, 0.0, 500.0, 300.0 });

		planet->setPosition(sf::Vector2f(500.f, 300.f));
		planet->setVelocity(sf::Vector2f(15.f, 0.f));
		planet->setColor(sf::Color(255, 160, 0));
		addChild(planet);
		m_planets.push_back(planet);
	}
	{
		Planet* planet = new Planet("Planet4");
		AutoTuner::StatespaceSystem& ss = planet->getStatespaceSystem();
		ss.setIntegrationSolver(AutoTuner::StatespaceSystem::IntegrationSolver::Rk4);

		MatlabAPI::Matrix A({
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 0 }
			});
		MatlabAPI::Matrix B({
			{ 1.0 / planet->getMass(), 0},
			{ 0, 1.0 / planet->getMass()},
			 {0, 0},
			 {0, 0}
			});
		MatlabAPI::Matrix C({ { 0, 0, 1, 0 }, { 0, 0, 0, 1 } });
		MatlabAPI::Matrix D({ { 0, 0 }, {0, 0 } });
		ss.setStateSpaceMatrices(A, B, C, D);
		ss.setStates({ 15, 0.0, 500.0, 300.0 });

		planet->setPosition(sf::Vector2f(500.f, 300.f));
		planet->setVelocity(sf::Vector2f(15.f, 0.f));
		planet->setColor(sf::Color(255, 120, 0));
		addChild(planet);
		m_planets.push_back(planet);
	}
	{
		Planet* planet = new Planet("Planet5");
		AutoTuner::StatespaceSystem& ss = planet->getStatespaceSystem();
		ss.setIntegrationSolver(AutoTuner::StatespaceSystem::IntegrationSolver::Discretized);

		/*
		Matlab:

			planetMass = 1000;
			dt = 0.5;

			A = [0 0 0 0;
				 0 0 0 0;
				 1 0 0 0;
				 0 1 0 0 ];

			B = [1/planetMass 0;
				 0 1/planetMass;
				 0 0;
				 0 0];

			C = [0 0 1 0;
				 0 0 0 1 ];

			D = [0 0;
				 0 0];

			planet = ss(A, B, C, D);

			sysd = c2d(planet, dt, 'foh');
			[Ad,Bd,Cd,Dd] = ssdata(sysd)


		*/

		MatlabAPI::Matrix A({
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0.5, 0, 1, 0 },
			{ 0, 0.5, 0, 1 }
			});
		MatlabAPI::Matrix B({
			{ 1.0e-3*0.5, 0},
			{ 0, 1.0e-3*0.5},
			 {1.0e-3*0.25, 0},
			 {0, 1.0e-3*0.25}
			});
		MatlabAPI::Matrix C({ { 0, 0, 1, 0 }, { 0, 0, 0, 1 } });
		MatlabAPI::Matrix D({ { 1.0e-4* 0.4167, 0 }, {0, 1.0e-4*0.4167 } });
			

		/*MatlabAPI::Matrix A({
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0.5, 0, 1, 0 },
			{ 0, 0.5, 0, 1 }
			});
		MatlabAPI::Matrix B({
			{ 1.0e-3 * 0.5, 0},
			{ 0, 1.0e-3 * 0.5},
			 {1.0e-3 * 0.125, 0},
			 {0, 1.0e-3 * 0.125}
			});
		MatlabAPI::Matrix C({ { 0, 0, 1, 0 }, { 0, 0, 0, 1 } });
		MatlabAPI::Matrix D({ { 0, 0 }, {0, 0 } });*/
		ss.setStateSpaceMatrices(A, B, C, D);
		ss.setStates({ 15, 0.0, 500.0, 300.0 });

		planet->setPosition(sf::Vector2f(500.f, 300.f));
		planet->setVelocity(sf::Vector2f(15.f, 0.f));
		planet->setColor(sf::Color(0, 120, 255));
		addChild(planet);
		m_planets.push_back(planet);
	}


}

void IntegrationTest::update()
{



	float deltaT = getFixedDeltaT();
	int counter = 0;
	for (Planet* planet : m_planets)
	{
		switch (counter)
		{
		case 0:
		{
			sf::Vector2f force = getForceFor(planet);
			sf::Vector2f acceleration = force / static_cast<float>(planet->getMass());
			sf::Vector2f newVelocity = planet->getVelocity() + acceleration * deltaT;
			sf::Vector2f newPosition = planet->getPosition() + newVelocity * deltaT;

			planet->setVelocity(newVelocity);
			planet->setPosition(newPosition);
			break;
		}
		case 1:
		case 2:
		case 3:
		case 4:
		{
			double mass = planet->getMass();
			//sf::Vector2f acceleration1 = getForceFor(planet->getPosition(), mass) / static_cast<float>(mass);
			//sf::Vector2f acceleration2 = getForceFor(planet->getPosition() + acceleration1*deltaT*deltaT *0.5f, mass) / static_cast<float>(mass);
			//sf::Vector2f acceleration3 = getForceFor(planet->getPosition() + acceleration2*deltaT* deltaT *0.5f, mass) / static_cast<float>(mass);
			//sf::Vector2f acceleration4 = getForceFor(planet->getPosition() + acceleration3*deltaT* deltaT, mass) / static_cast<float>(mass);
			//
			//sf::Vector2f acceleration = (acceleration1 + 2.f * acceleration2 + 2.f * acceleration3 + acceleration4) / 6.f;
			//sf::Vector2f newVelocity = planet->getVelocity() + acceleration * deltaT;
			//sf::Vector2f newPosition = planet->getPosition() + newVelocity * deltaT;

			//sf::Vector2f force = getForceFor(planet);
			//sf::Vector2f acceleration = force / static_cast<float>(planet->getMass());
			////sf::Vector2f sumAccel = (planet->getAcceleration() + acceleration) * 0.3f;
			//sf::Vector2f newVelocity = planet->getVelocity() + (acceleration) * deltaT;
			//sf::Vector2f newPosition = planet->getPosition() + (newVelocity + planet->getVelocity()) * deltaT * 0.5f;

			AutoTuner::StatespaceSystem& ss = planet->getStatespaceSystem();
			sf::Vector2f force = getForceFor(planet);
			ss.setInputSignals({ static_cast<double>(force.x), static_cast<double>(force.y) });
			ss.update(deltaT);
			std::vector<double> states = ss.getStates();


			//planet->setAcceleration(acceleration);
			planet->setVelocity({ (float)states[0], (float)states[1] });
			planet->setPosition({ (float)states[2], (float)states[3] });
			break;
		}
		}
		
		++counter;
	}
}


sf::Vector2f IntegrationTest::getForceFor(const Planet* planet)
{
	sf::Vector2f force(0.f, 0.f);

	for (const Sun* sun : m_suns)
	{
		force += getForce(planet->getPosition(), planet->getMass(), sun->getPosition(), sun->getMass());
	}

	return force;
}
sf::Vector2f IntegrationTest::getForceFor(const sf::Vector2f& planetPos, double planetMass)
{
	sf::Vector2f force(0.f, 0.f);

	for (const Sun* sun : m_suns)
	{
		force += getForce(planetPos, planetMass, sun->getPosition(), sun->getMass());
	}

	return force;
}

sf::Vector2f IntegrationTest::getForce(const sf::Vector2f& pos1, double mass1, const sf::Vector2f& pos2, double mass2)
{
	sf::Vector2f direction = pos2 - pos1;
	float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	if (distance > 0.f)
	{
		float forceMagnitude = static_cast<float>((mass1 * mass2) / (distance * distance));
		sf::Vector2f force = (direction / distance) * forceMagnitude;
		return force;
	}
}