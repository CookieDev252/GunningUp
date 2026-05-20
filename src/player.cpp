#include "player.hpp"
#include <stdio.h>

//pre declare functions
Vector2 ClosestPoint(Vector2 a, Vector2 b, Vector2 p);


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
		m_rotation.y += (fabsf(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y)) > m_rightStickDeadzoneY) ? GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * m_lookSensitivity * dt: 0;
		if (m_rotation.y >= 360.f) m_rotation.y -= 360.f;
		if (m_rotation.y < 0) m_rotation.y += 360.f;
		m_rotation.x += (fabsf(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X)) > m_rightStickDeadzoneX) ? GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * m_lookSensitivity * dt : 0;
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
		if (CheckCollisionCircleLine(m_position, m_size * 0.5f, wall.startPoint, wall.endPoint)) {
			//check what side collided
			if (Vector2DotProduct(m_position, ClosestPoint(m_position, wall.startPoint, wall.endPoint))) {
				Vector2 closestPoint = ClosestPoint(wall.startPoint, wall.endPoint, m_position);

				Vector2 directionAndLength = Vector2Scale(Vector2Normalize(Vector2Subtract(closestPoint, m_position)), m_size * 0.51f); //add a little extra on the end so it doesn't get stuck

				m_position = Vector2Add(m_position, Vector2Subtract(Vector2Subtract(closestPoint, m_position), directionAndLength));
			}
		}
	}
}

//calculates the nearest point to the line with a given point
Vector2 ClosestPoint(Vector2 a, Vector2 b, Vector2 p) {

	// get vector differences
	Vector2 D = Vector2Subtract(b, a);
	Vector2 AP = Vector2Subtract(p, a);

	// projected length + normalization
	float t = Vector2DotProduct(AP, D) / Vector2DotProduct(D, D);

	//clamp to [0,1]
	t = fmaxf(0, fminf(1, t));

	//calculate point
	return Vector2Add(a, Vector2Scale(D, t));
}