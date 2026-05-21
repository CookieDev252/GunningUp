#include "entity.hpp"

Vector2 ClosestPoint(Vector2 a, Vector2 b, Vector2 p);

void Entity::MoveAndCollideWithMap(std::vector<Line2D>& walls)
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