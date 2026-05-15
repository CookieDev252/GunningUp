#include "player.hpp"
#include <stdio.h>

//pre declare functions
Vector2 closestPoint(Vector2 a, Vector2 b, Vector2 p);


Player::Player()
{}

Player::Player(raylib::Vector2& position, raylib::Vector2& rotation) {
	m_position = position;
	m_rotation = rotation;
}

Player::~Player()
{

}

void Player::draw() const
{
	DrawCircle(m_position.x, m_position.y, m_size/2.f, { 0,255,0,255 } );
	DrawLine(m_position.x, m_position.y, m_position.x + m_up.x*m_size, m_position.y - m_up.y*m_size, {0,122,0,255} ); 
}

void Player::update(float dt)
{

	if (IsGamepadAvailable(0)) {
		m_rotation.y += (fabsf(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y)) > m_rightStickDeadzoneY) ? GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * m_lookSensitivity : 0;
		if (m_rotation.y >= 360.f) m_rotation.y -= 360.f;
		if (m_rotation.y < 0) m_rotation.y += 360.f;
		m_rotation.x += (fabsf(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X)) > m_rightStickDeadzoneX) ? GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * m_lookSensitivity : 0;
		if (m_rotation.x >= 360.f) m_rotation.x -= 360.f;
		if (m_rotation.x < 0) m_rotation.x += 360.f;

	}else{
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
	}

	m_up = {
		std::sinf((m_rotation.x - 90.f) * TORADIANS) ,
		std::cosf((m_rotation.x - 90.f) * TORADIANS)
	};

	m_right = {
		std::sinf((m_rotation.x) * TORADIANS),
		std::cosf((m_rotation.x) * TORADIANS)
	};

	float deltaSpeed = dt * m_speed;

	//get movement axis

	if (IsGamepadAvailable(0))
	{
		m_controllerAxis.x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
		if (m_controllerAxis.x < m_leftStickDeadzoneX && m_controllerAxis.x > -m_leftStickDeadzoneX) m_controllerAxis.x = 0;
		m_controllerAxis.y = -GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
		if (m_controllerAxis.y < m_leftStickDeadzoneY && m_controllerAxis.y > -m_leftStickDeadzoneY) m_controllerAxis.y = 0;
	} 
	else {
		if (IsKeyPressed(KEY_W)) { m_controllerAxis.y += 1.f; }
		else if (IsKeyReleased(KEY_W)) { m_controllerAxis.y -= 1.f; }
		if (IsKeyPressed(KEY_A)) { m_controllerAxis.x -= 1.f; }
		else if (IsKeyReleased(KEY_A)) { m_controllerAxis.x += 1.f; }
		if (IsKeyPressed(KEY_S)) { m_controllerAxis.y -= 1.f; }
		else if (IsKeyReleased(KEY_S)) { m_controllerAxis.y += 1.f; }
		if (IsKeyPressed(KEY_D)) { m_controllerAxis.x += 1.f; }
		else if (IsKeyReleased(KEY_D)) { m_controllerAxis.x -= 1.f; }
	}

	//apply movement to up and right vectors

	m_translationTotal = {
		((m_up.x * m_controllerAxis.y) + (m_right.x * m_controllerAxis.x)),
		((m_up.y * m_controllerAxis.y) + (m_right.y * m_controllerAxis.x))
	};

	float weight = sqrtf(m_translationTotal.x * m_translationTotal.x + m_translationTotal.y * m_translationTotal.y);

	
	if (weight != 0)
	{
		m_translationUnit = { m_translationTotal.x / weight , m_translationTotal.y / weight };
		m_position.x += m_translationUnit.x * m_speed * dt;
		m_position.y += m_translationUnit.y * -m_speed * dt;
	}

}

void Player::MoveAndCollideWithMap(std::vector<Line2D>& walls)
{
	float dX, dY, dXW, dYH, radius;

	radius = m_size * 0.5f;

	for (Line2D& wall : walls) {
		////get the absolute distances for each value
		//dX = room.x - m_position.x;
		//dXW = room.x + room.w - m_position.x;
		//dY = room.y - m_position.y;
		//dYH = room.y + room.h - m_position.y;

		////check if intersection happened
		////top
		//if (std::fabsf(dY) < radius && 
		//	dX < radius &&
		//	dXW > -radius
		//) 
		//{
		//	m_position.y += dY - radius;
		//}

		////bottom
		//else if (std::fabsf(dYH) < radius &&
		//	dX < radius &&
		//	dXW > -radius
		//) 
		//{
		//	m_position.y += dYH + radius;
		//}

		////left
		//if (std::fabsf(dX) < radius &&
		//	dY < radius &&
		//	dYH > -radius
		//) 
		//{
		//	m_position.x += dX - radius;
		//}
		//
		////right
		//else if (std::fabsf(dXW) < radius &&
		//	dY < radius &&
		//	dYH > -radius
		//)
		//{
		//	m_position.x += dXW + radius;
		//}
		if (CheckCollisionCircleLine(m_position, m_size, wall.startPoint, wall.endPoint)) {
			//check what side collided
			if (Vector2DotProduct(m_position,closestPoint(m_position, wall.startPoint, wall.endPoint))) {}
		}
	}
}

//calculates the nearest point to the line with a given point
Vector2 closestPoint(Vector2 a, Vector2 b, Vector2 p) {

	// get vector differences
	Vector2 AB = Vector2Subtract(b, a);
	Vector2 AP = Vector2Subtract(p, a);

	// projected length + normalization
	float t = Vector2DotProduct(AP, AB) / Vector2DotProduct(AB, AP);

	//clamp to [0,1]
	t -= (float)(int)t;

	//calculate point
	return Vector2Add(a, Vector2Scale(AB, t));
}