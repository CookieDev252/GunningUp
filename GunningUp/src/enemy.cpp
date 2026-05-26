#include "enemy.hpp"

Enemy::Enemy(Vector2 position, Vector2 rotation)
	: Entity()
{
	m_position = position;
	m_rotation = rotation;
	m_speed /= 1.75f;
}

void Enemy::draw() const
{
	DrawCircle(m_position.x, m_position.y, m_size / 2.f, RED);
}

inline void Enemy::update(float dt)
{
	//check if enemy is close enough to the node
	if (distanceBeforeSwitchSqr >= Vector2DistanceSqr(m_position, m_navRef->getPosition()) ){
		//check surrounding nodes
		float currentDistance = 9999;
		NavigationNode* optimalNode = m_navRef;
		for (NavigationNode* node : m_navRef->m_connectedNodes) {
			if (Vector2DistanceSqr(m_playerRef->getPosition(), node->getPosition()) < currentDistance) {
				optimalNode = node;
				currentDistance = Vector2DistanceSqr(m_playerRef->getPosition(), node->getPosition());
			}
		}
		shouldChargePlayer = (currentDistance <= Vector2DistanceSqr(m_position, m_playerRef->getPosition()));
		shouldShootPlayer = (currentDistance <= distanceBeforeShootingSqr);
		
		//assign the most optimal node based on distance
		m_navRef = optimalNode;
	}

	//move towards node or player
	if (shouldShootPlayer) {

	}
	else{
		if (!shouldChargePlayer)
			m_position = Vector2MoveTowards(m_position, m_navRef->getPosition(), m_speed * dt);
		else
			m_position = Vector2MoveTowards(m_position, m_playerRef->getPosition(), m_speed * dt);
	}
	
}

void Enemy::setPlayer(Player* player)
{
	m_playerRef = player;
}

void Enemy::setNavigationNode(NavigationNode* node)
{
	m_navRef = node;
}

void Enemy::MoveAndCollideWithEnemies(std::vector<Enemy>& enemies) {
	for (Enemy& enemy : enemies) {
		if (CheckCollisionCircles(m_position, m_size / 2.f, enemy.getPosition(), enemy.getSize() / 2.f)) {
			m_position = Vector2Subtract(m_position,
				Vector2SubtractValue(Vector2Subtract(m_position, enemy.getPosition()),enemy.getSize()/2.f+getSize()/2.f));
		}
	}
}
