#pragma once

#include "QSFML_EditorWidget.h"

class Ball : public QSFML::Objects::GameObject
{
	friend class Painter;
public:
	Ball(float radius, const sf::Color& color);

	float getRadius() { return m_radius; }
protected:

private:

	class Painter : public QSFML::Components::Drawable
	{
	public:
		Painter(Ball *ball, const sf::Color &color)
			: Drawable("Painter")
			, m_ball(ball)
		{
			m_circle.setRadius(ball->m_radius);
			m_circle.setFillColor(color);
			m_circle.setOrigin(ball->m_radius, ball->m_radius);


		}


		void drawComponent(sf::RenderTarget& target, sf::RenderStates states) const override
		{
		     // Draw your stuff here
		     // target.draw(...) 
		
		     // If you want to draw using the relative position of the parent object, 
		     // pass the states to the target.draw(...) function
		     // target.draw(..., states) 

			//m_circle.po(m_ball->m_position);
			target.draw(m_circle, states);

			// Draw line to indicate angle
			sf::Vertex line[] =
			{
				sf::Vertex({0,0}, sf::Color::Red),
				sf::Vertex(sf::Vector2f(m_ball->m_radius, 0), sf::Color::Red)
			};
			target.draw(line, 2, sf::Lines, states);
		}

	private:
		Ball* m_ball;

		sf::CircleShape m_circle;
	};


	Painter* m_painter = nullptr;
	float m_radius = 10.f;
	//float m_mass = 0.5f;
	//
	//
	//sf::Vector2f m_position = { 0.f, 0.f };
	//sf::Vector2f m_velocity = { 0.f, 0.f };
	//sf::Vector2f m_acceleration = { 0.f, 0.f };
	//
	//
	//float m_angle = 0.f; // Angle of the beam in radians
	//float m_angularVelocity = 0.f;
	//float m_angularAcceleration = 0.f;



};