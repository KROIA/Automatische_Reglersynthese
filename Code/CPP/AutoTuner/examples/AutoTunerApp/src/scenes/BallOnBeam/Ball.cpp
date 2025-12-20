#include "scenes/BallOnBeam/Ball.h"


Ball::Ball(float radius, const sf::Color& color)
	: QSFML::Objects::GameObject("Ball")
	, m_radius(radius)
{
	m_painter = new Painter(this, color);
	addComponent(m_painter);
}