#ifndef BLACKBOARD_HPP
#define BLACKBOARD_HPP

#include "GunningUp.h"

struct BlackBoard {
	Player* player; //holds a reference to the player for instant referals
					//line data for navigation
};

static BlackBoard blackboard{
	nullptr		//player reference
};

#endif