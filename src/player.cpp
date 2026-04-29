#include "player.hpp"

Player::Player()
{}

Player::Player(raylib::Vector2& position, raylib::Vector2& rotation) {

}

Player::~Player()
{

}

void Player::draw() const
{
	DrawCircle(m_position.x, m_position.y, 5.f, { 0,255,0,255 } );
	DrawLine(m_position.x, m_position.y, m_position.x + m_up.x*10, m_position.y - m_up.y*10, {0,122,0,255} );
}

void Player::update(float dt)
{

	if (IsKeyDown(KEY_UP)) {
		m_rotation.y += dt * m_rotSpeed;
		if (m_rotation.y >= 360.f) { m_rotation.y -= 360.f; }
	}
	if (IsKeyDown(KEY_LEFT)) { 
		m_rotation.x -= dt * m_rotSpeed; 
		if (m_rotation.x < 0.f) { m_rotation.x += 360.f; }
	}
	if (IsKeyDown(KEY_DOWN)) { 
		m_rotation.y -= dt * m_rotSpeed;
		if (m_rotation.y < 0) { m_rotation.y += 360.f; }
	}
	if (IsKeyDown(KEY_RIGHT)) { 
		m_rotation.x += dt * m_rotSpeed; 
		if (m_rotation.x >= 360.f) { m_rotation.x -= 360.f; }
	}

	m_up = {
		std::sinf(m_rotation.x * (PI / 180.f)) ,
		std::cosf(m_rotation.x * (PI / 180.f))
	};

	m_right = {
		std::sinf((m_rotation.x+90.f) * (PI / 180.f)),
		std::cosf((m_rotation.x+90.f) * (PI / 180.f)) 
	};

	if (IsKeyDown(KEY_W)) { m_position = { m_position.x + m_up.x * m_speed, m_position.y - m_up.y * m_speed }; }
	if (IsKeyDown(KEY_A)) { m_position = { m_position.x - m_right.x * m_speed, m_position.y + m_right.y * m_speed }; }
	if (IsKeyDown(KEY_S)) { m_position = { m_position.x - m_up.x * m_speed, m_position.y + m_up.y * m_speed }; }
	if (IsKeyDown(KEY_D)) { m_position = { m_position.x + m_right.x * m_speed, m_position.y - m_right.y * m_speed }; }

}
