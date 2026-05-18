#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "GunningUp.h"

class Enemy : public Entity {
public:
	Enemy(Vector2 position, float size);
private:
	Player* playerRef{nullptr};
};

#endif