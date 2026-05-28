#include "enemy.hpp"

/*! \defgroup enemyFunctions
* @{
*/

/*!
* Class constructor
*/
Enemy::Enemy(Vector2 position, Vector2 rotation)
	: Entity()
{
	m_position = position;
	m_rotation = rotation;
	m_speed /= 1.75f;

	for (int i = 0; i < 10; i++) {
		m_bullets.push_back(Bullet{});
	}
}

/*! Draw - draws the enemy in 2D view */
void Enemy::draw() const
{
	DrawCircle(m_position.x, m_position.y, m_size / 2.f, RED);
}

/*! Update - updates the enemy */
inline void Enemy::update(float dt)
{
	//check if enemy is close enough to the node
	if (m_distanceBeforeSwitchSqr >= Vector2DistanceSqr(m_position, m_navRef->getPosition()) ){
		//check surrounding nodes
		float currentDistance = 9999;
		NavigationNode* optimalNode = m_navRef;
		for (NavigationNode* node : m_navRef->m_connectedNodes) {
			if (Vector2DistanceSqr(m_playerRef->getPosition(), node->getPosition()) < currentDistance) {
				optimalNode = node;
				currentDistance = Vector2DistanceSqr(m_playerRef->getPosition(), node->getPosition());
			}
		}
		m_shouldChargePlayer = (currentDistance <= Vector2DistanceSqr(m_position, m_playerRef->getPosition()));
		m_shouldShootPlayer = (currentDistance <= m_distanceBeforeShootingSqr);
		
		//assign the most optimal node based on distance
		m_navRef = optimalNode;
	}

	//move towards node or player
	if (m_shouldShootPlayer) {

	}
	else{
		if (!m_shouldChargePlayer)
			m_position = Vector2MoveTowards(m_position, m_navRef->getPosition(), m_speed * dt);
		else
			m_position = Vector2MoveTowards(m_position, m_playerRef->getPosition(), m_speed * dt);
	}
	
}

/*! SetPlayer - assigns the location of the Player class to m_playerRef */
void Enemy::setPlayer(Player* player)
{
	m_playerRef = player;
}

/*! SetNavigationNode - assigns the location of the NavigationNode class to m_navRef */
void Enemy::setNavigationNode(NavigationNode* node)
{
	m_navRef = node;
}

/*! MoveAndCollideWithEnemies - displaces the enemy if a collision has occured with another Enemy object */
void Enemy::MoveAndCollideWithEnemies(std::vector<Enemy>& enemies) {
	for (Enemy& enemy : enemies) {
		if (CheckCollisionCircles(m_position, m_size / 2.f, enemy.getPosition(), enemy.getSize() / 2.f)) {
			m_position = Vector2Add(enemy.getPosition(),Vector2Scale(Vector2Normalize(Vector2Subtract(getPosition(), enemy.getPosition())),enemy.getSize()/2.0f+getSize()/2.0f));
		}
	}
}

/*! @} */
