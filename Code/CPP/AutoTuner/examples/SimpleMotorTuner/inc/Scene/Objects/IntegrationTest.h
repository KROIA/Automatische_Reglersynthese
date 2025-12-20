#pragma once

#include "AutoTuner.h"


class Sun : public QSFML::Objects::GameObject
{
public:
	Sun(const std::string& name = "Sun",
		GameObject* parent = nullptr)
		: QSFML::Objects::GameObject(name, parent)
	{
		m_pointPainter = new QSFML::Components::PointPainter("SunPainter");
		addComponent(m_pointPainter);

		setMass(100000.0);
	}

	void setMass(double mass)
	{
		m_mass = mass;
		m_pointPainter->clear();
		m_pointPainter->addPoint((sf::Vector2f(0.f, 0.f)), static_cast<float>(sqrt(mass)*0.1), sf::Color::Yellow);
	}
	double getMass() const
	{
		return m_mass;
	}

private:
	QSFML::Components::PointPainter* m_pointPainter = nullptr;
	double m_mass;
};

class Planet : public QSFML::Objects::GameObject
{
public:
	Planet(const std::string& name = "Planet",
		GameObject* parent = nullptr)
		: QSFML::Objects::GameObject(name, parent)
	{
		m_pointPainter = new QSFML::Components::PointPainter("PlanetPainter");
		addComponent(m_pointPainter);

		m_pathPainter = new QSFML::Components::PathPainter("PlanetPathPainter");
		m_pathPainter->ignoreTransform(true);
		addComponent(m_pathPainter);

		setMass(1000.0);
	}

	void setColor(const sf::Color& color)
	{
		m_color = color;
		m_pathPainter->setColor(m_color);
		setMass(m_mass);
	}

	void setMass(double mass)
	{
		m_mass = mass;
		m_pointPainter->clear();
		m_pointPainter->addPoint((sf::Vector2f(0.f, 0.f)), static_cast<float>(sqrt(mass)*0.1), m_color);
	}
	double getMass() const
	{
		return m_mass;
	}
	void setVelocity(const sf::Vector2f& velocity)
	{
		m_velocity = velocity;
	}
	sf::Vector2f getVelocity() const
	{
		return m_velocity;
	}
	void setAcceleration(const sf::Vector2f& acceleration)
	{
		m_acceleration = acceleration;
	}
	sf::Vector2f getAcceleration() const
	{
		return m_acceleration;
	}

	AutoTuner::StatespaceSystem& getStatespaceSystem()
	{
		return m_ssSystem;
	}
	void update() override
	{
		//double dt = getDeltaT();
		//sf::Vector2f position = getPosition();
		//position += m_velocity * static_cast<float>(dt);
		//setPosition(position);

		double age = getAge();
		if ((int)(age * 10) % 1 == 0)
			m_pathPainter->appendPoint(getPosition());
	}

private:
	QSFML::Components::PointPainter* m_pointPainter = nullptr;
	QSFML::Components::PathPainter* m_pathPainter = nullptr;
	double m_mass;
	sf::Vector2f m_velocity;
	sf::Vector2f m_acceleration;
	sf::Color m_color;
	AutoTuner::StatespaceSystem m_ssSystem;
};



class IntegrationTest : public QSFML::Objects::GameObject
{
public: 

	IntegrationTest(const std::string& name = "IntegrationTest",
		GameObject* parent = nullptr);


	void update() override;

private:
	sf::Vector2f getForceFor(const Planet* planet);
	sf::Vector2f getForceFor(const sf::Vector2f &planetPos, double planetMass);
	sf::Vector2f getForce(const sf::Vector2f &pos1, double mass1, const sf::Vector2f& pos2, double mass2);

	std::vector<Planet*> m_planets;
	std::vector<Sun*> m_suns;
};