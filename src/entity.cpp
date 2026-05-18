#include "entity.hpp"

Entity::Entity(Vector2 position, float size)
{
	m_position = position;
	m_rotation = Vector2{ 0,0 };
	m_size = size;
}
