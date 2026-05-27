#include "bullet.hpp"


inline void Bullet::Draw() const {
	if (m_active) DrawCircle(m_position.x, m_position.y, m_size/2.f, YELLOW);
}

inline void Bullet::Update(float dt) {
	if (m_active) {
		m_lifetime += dt;
		if (m_lifetime > m_maximumLifetime) reset(); return;
		m_prevPos = m_position;
		m_position.x += m_direction.x * m_speed * dt;
		m_position.y += m_direction.y * m_speed * dt;
	}
}

inline void Bullet::reset() {
	m_position = { -9999,-9999 };
	m_prevPos = { -9999,-9999 };
	m_direction = { 0,0 };
	m_active = false;
	m_lifetime = 0;
}

inline void Bullet::Fire(Vector2 position, Vector2 direction)
{
	if (!m_active) {
		m_active = true; // bullet is now in an active state
		m_position = position;
		m_prevPos = position;
		m_direction = direction;
	}
}

inline bool Bullet::CollidesWith(Player& player)
{
	if (CheckCollisionCircleLine(player.getPosition(), player.getSize() * 0.5f, m_prevPos, m_position))
	{
		reset();
		return true;
	}
	return false;
}

inline bool Bullet::CollidesWith(Enemy& enemy)
{
	if (CheckCollisionCircleLine(enemy.getPosition(), enemy.getSize() * 0.5f, m_prevPos, m_position))
	{
		reset();
		return true;
	}
	return false;
}

inline bool Bullet::CollidesWith(Line2D& wall)
{
	if (CheckCollisionLines(m_prevPos, m_position, wall.startPoint, wall.endPoint, m_collisionPoint)) {
		if (m_ricochetsLeft != 0) {
			m_direction = Vector2Reflect(m_direction, wall.normal);
			//move bullet away from wall from the point of collision by the difference in how far the bullet should've gone compared to the actual distance it went
			m_position = Vector2Add(*m_collisionPoint, Vector2Scale(m_direction, Vector2Length(Vector2Subtract(m_prevPos, m_position)) - Vector2Length(Vector2Subtract(m_prevPos, *m_collisionPoint))));
		}
		else
			reset();
		return true;
	}
	return false;

}


