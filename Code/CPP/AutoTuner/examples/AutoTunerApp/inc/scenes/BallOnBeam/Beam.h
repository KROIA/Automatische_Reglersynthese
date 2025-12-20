#pragma once

#include "QSFML_EditorWidget.h"

class Beam : public QSFML::Objects::GameObject
{
	friend class Painter;
public:
	Beam(float width);

	void setBeamAngle(float angle)
	{
		m_angle = angle;
	}
	float getThickness() const
	{
		return 10;
	}
protected:

private:

	class Painter : public QSFML::Components::Drawable
	{
	public:
		Painter(Beam* beam)
			: Drawable("Painter")
			, m_beam(beam)
		{
			m_pivot.setRadius(10);
			m_pivot.setPointCount(3);
			m_pivot.setFillColor(sf::Color(100, 100, 100));
			m_pivot.setOrigin(m_pivot.getRadius(), m_pivot.getRadius());
			

			m_rect = sf::RectangleShape(sf::Vector2f(beam->m_width, m_beam->getThickness()));
			m_rect.setOrigin(beam->m_width / 2.f, m_rect.getSize().y);
			m_rect.setPosition(0, m_rect.getSize().y);
			m_pivot.setPosition(m_rect.getPosition());
			m_rect.setFillColor(sf::Color(150, 150, 150));
		}


		void drawComponent(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			// Draw your stuff here
			// target.draw(...) 

			// If you want to draw using the relative position of the parent object, 
			// pass the states to the target.draw(...) function
			// target.draw(..., states) 

		   //m_circle.po(m_ball->m_position);
			

			// Rectangle
			// Set rotation
			float rotation = m_beam->m_angle * 180.f / M_PI;
			m_rect.setRotation(rotation);
			target.draw(m_rect, states);

			m_pivot.setRotation(rotation);
			target.draw(m_pivot, states);
		}

	private:
		Beam* m_beam;

		mutable sf::CircleShape m_pivot;
		mutable sf::RectangleShape m_rect;
	};


	Painter* m_painter = nullptr;

	float m_width = 0.f;

	float m_angle = 0.f; 
};