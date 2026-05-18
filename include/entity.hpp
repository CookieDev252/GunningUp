#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "GunningUp.h"

class Entity {
public:
	Entity(Vector2 position, float size);
	Vector2&	GetPosition() { return m_position; }
	void		SetPosition(Vector2& position) { m_position = position; }
	Vector2&	GetRotation() { return m_rotation; }
	void		SetRotation(Vector2& rotation) { m_rotation = rotation; }
	float		GetSize() { return m_size; }
	void		SetSize(float& size) { m_size = size; }
protected:
	Vector2 m_position{0,0};
	Vector2 m_rotation{0,0};
	float m_size{1.f};
private:
};

#endif