#ifndef FLOORGENERATOR_HPP
#define FLOORGENERATOR_HPP

#include "GunningUp.h"

struct Room {
	int x, y, w, h;
	Color roomFillColor{ 0,0,0,0 };
	Room(int x, int y, int w, int h) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->roomFillColor = {
			(unsigned char)GetRandomValue(0,255),
			(unsigned char)GetRandomValue(0, 255),
			(unsigned char)GetRandomValue(0, 255),
			255
		};
	}

};



//similar to Line2D but can connect to multiple nodes
class NavigationNode {
public:
	NavigationNode(Vector2 position) { m_position = position; }
	~NavigationNode() = default;
	void AddNode(NavigationNode* node) { m_connectedNodes.push_back(node); }
	void RemoveNode(NavigationNode* node);
	Vector2 getPosition() const { return m_position; };
	std::vector<NavigationNode*> m_connectedNodes = {};
private:
	Vector2 m_position;
private:
};

class FloorGenerator {
private:
	int width{ 0 };
	int height{ 0 };
	int m_splitCount{ 100 }; // default to 3 splits
	int m_minRoomArea{ 20 }; // small room
	int m_maxRoomArea{ 50 }; // massive room
	int m_intersection{ 10 }; // gap betweem room split (hallways)
	std::vector<Room> m_rooms = {}; //constains all the rooms
	std::vector<Line2D> m_walls = {}; // contains all the walls
	std::vector<NavigationNode> m_navNodes = {}; // contains all the navigation nodes


public:
	FloorGenerator(int width, int height);
	void draw();

	void setSplitCount(int count) { m_splitCount = count; }

	void SplitRoom(int selectedRoom);
	void GenerateLevel();
	void setSeed(unsigned int seed) { SetRandomSeed(seed); }
	bool CanBeSplit(Room& room) const;

	std::vector<Room>& getRooms() { return m_rooms; }
	std::vector<Line2D>& getWalls() { return m_walls; }
	std::vector<NavigationNode>& getNavigationNodes() { return m_navNodes; }
};

#endif