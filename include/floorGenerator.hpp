#ifndef FLOORGENERATOR_HPP
#define FLOORGENERATOR_HPP

#include "raylib-cpp.hpp"
#include <vector>

struct Room{
	int x, y, w, h;
	Color roomFillColor{0,0,0,0};
	Room(int x,int y,int w,int h){
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

struct Line2D {
	Line2D(Vector2 start, Vector2 end) {
		startPoint = start;
		endPoint = end;
	}
	Vector2 startPoint;
	Vector2 endPoint;
private:
	Vector2 normal;
};

class FloorGenerator {
private:
	int m_splitCount{ 100 }; // default to 3 splits
	int m_minRoomArea{100}; // small room
	int m_maxRoomArea{50}; // massive room 
	int m_intersection{ 10 }; // gap betweem room split (hallways)
	std::vector<Room> m_rooms = {}; //constains all the rooms
	std::vector<Line2D> m_walls = {}; // contains all the walls
	

public:
	FloorGenerator(int width, int height);
	void draw();
	
	void setSplitCount(int count) { m_splitCount = count; }

	void SplitRoom(int selectedRoom);
	void GenerateLevelUnseeded();
	bool CanBeSplit(Room& room) const;

	std::vector<Room>& getRooms() { return m_rooms; }
};

#endif