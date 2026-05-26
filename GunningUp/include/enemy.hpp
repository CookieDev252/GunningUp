#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "GunningUp.h"

class NavigationNode;
class Player;

/** \class Enemy
* 
*   \brief enemy 
*/
class Enemy : public Entity {
public:
	Enemy(Vector2 position, Vector2 rotation);
	virtual void draw() const override;
	virtual void update(float dt) override;
	void setPlayer(Player* player);
	void setNavigationNode(NavigationNode* node);
	void MoveAndCollideWithEnemies(std::vector<Enemy>& enemies);
private:
	Player* m_playerRef{ nullptr };
	//nav variables
	NavigationNode* m_navRef{ nullptr };
	float distanceBeforeSwitchSqr{ 64.f };
	float distanceBeforeShootingSqr{ 100.f };
	bool shouldChargePlayer{ false };
	bool shouldShootPlayer{ false };
};

#endif