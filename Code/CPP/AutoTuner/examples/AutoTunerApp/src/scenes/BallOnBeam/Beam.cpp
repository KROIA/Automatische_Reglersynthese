#include "scenes/BallOnBeam/Beam.h"

Beam::Beam(float width)
	: QSFML::Objects::GameObject("Beam")
	, m_width(width)
{
	m_painter = new Painter(this);
	addComponent(m_painter);
}
