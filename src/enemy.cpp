#include "enemy.hpp"

Enemy::Enemy(Vector2 position, float size) :
	Entity(position, size)
{
	playerRef = blackboard.player;
}
