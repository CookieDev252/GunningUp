#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "GunningUp.h"

class NavigationNode;
class Bullet;
class Player;

/** \class Enemy */
class Enemy : public Entity {
public:
	Enemy(Vector2 position, Vector2 rotation);
	virtual void draw() const override; ///< draws the enemy (overrides the base class function)
	virtual void update(float dt) override; ///< updates the enemy (overrides the base class function)
	void setPlayer(Player* player); ///< sets the player pointer
	void setNavigationNode(NavigationNode* node); ///< sets the navigation node
	void MoveAndCollideWithEnemies(std::vector<Enemy>& enemies); ///< moves and collides the enemy with other enemies
	float getHeight() const { return m_enemyHeight; }; ///< returns the height of the enemy
	bool isHit() const { return m_beenShot; }; ///< returns whether or not the enemy has been shot
	void setHit(bool state) { m_beenShot = state; } ///< sets the state of m_beenShot
	
	std::vector<Bullet>& getBullets() { return m_bullets; } ///< returns the vector of bullets
private:
	Player* m_playerRef{ nullptr }; ///< a reference to the player class
	//nav variables
	NavigationNode* m_navRef{ nullptr }; ///< a reference pointer to a node, the node has attatched nodes which is how the enemy navigates the scene
	float m_enemyHeight{ 1000.f }; ///< the height that the enemy is drawn to the screen
	//behaviour variables
	float m_distanceBeforeSwitchSqr{ 64.f }; ///< required distance to switch nodes squared
	float m_distanceBeforeShootingSqr{ 800.f }; ///< required distance to shoot the player squared
	bool m_shouldChargePlayer{ false }; //< should the enemy charge towards the player
	bool m_shouldShootPlayer{ false }; ///< should the enemy shoot the player (priority over shouldChargePlayer)
	bool m_beenShot{ false }; ///< has the enemy been hit by a bullet
	//bullet variables
	std::vector<Bullet> m_bullets{}; ///< using object pooling to prevent lag from instantiation
	
};

#endif