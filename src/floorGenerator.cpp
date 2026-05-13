#include "floorGenerator.hpp"
#include <iostream>

FloorGenerator::FloorGenerator(int width, int height)
{
	m_rooms.push_back(Room(0, 0, width, height));

	GenerateLevel();
}

void FloorGenerator::draw() {
	for (const Line2D& wall : m_walls) {
		DrawLine(wall.startPoint.x, wall.startPoint.y, wall.endPoint.x, wall.endPoint.y, wall.color);
	}
}

void FloorGenerator::SplitRoom(int selectedRoom)
{
	float offset;
	Room room = m_rooms[selectedRoom];
	if (room.w > room.h) {
		offset = GetRandomValue(-room.w/8,room.w/8);
		m_rooms.insert(m_rooms.begin() + selectedRoom+1,
			Room(
				room.x,
				room.y,
				room.w / 2 + offset  - m_intersection,
				room.h
				)
		);
		m_rooms.insert(m_rooms.begin() + selectedRoom+1,
			Room(
				room.x + room.w / 2 + offset + m_intersection,
				room.y,
				room.w / 2 - offset - m_intersection,
				room.h
			)
		);
		m_rooms.erase(m_rooms.begin() + selectedRoom);
	}
	else {
		offset = GetRandomValue(-room.h / 8, room.h / 8);
		m_rooms.insert(m_rooms.begin() + selectedRoom+1,
			Room(
				room.x,
				room.y,
				room.w,
				room.h / 2 + offset - m_intersection
			)
		);
		m_rooms.insert(m_rooms.begin() + selectedRoom+1,
			Room(
				room.x,
				room.y + room.h / 2 + offset + m_intersection,
				room.w,
				room.h / 2 - offset - m_intersection
			)
		);
		m_rooms.erase(m_rooms.begin() + selectedRoom);
	}
}

void FloorGenerator::GenerateLevel()
{
	int splits; // counter for which split it's on
	int selectedRoom = 0; //set to 0 because there should only be 1 room right now
	int firstSelectedRoom;
	for (splits = 0; splits < m_splitCount; splits++) {
		//here we'll select a random room, check if it's splittable and if true
		//cut through the longest side (this'll prevent too skinny rooms)
		firstSelectedRoom = selectedRoom;
		while (!CanBeSplit(m_rooms[selectedRoom])) { 
			selectedRoom += 1; if (selectedRoom == m_rooms.size()) selectedRoom = 0;
			if (firstSelectedRoom == selectedRoom) { break; } //no valid squares to split
		}
		SplitRoom(selectedRoom);
		selectedRoom = GetRandomValue(0, m_rooms.size()-1);
	}
	// convert all the rooms into walls
	{
		Vector2 topLeft;
		Vector2 topRight;
		Vector2 bottomRight;
		Vector2 bottomLeft;
		for (auto& room : m_rooms) {
			topLeft = { (float)room.x, (float)room.y + (float)room.h};
			topRight = { (float)room.x + (float)room.w, (float)room.y + (float)room.h};
			bottomRight = { (float)room.x + (float)room.w, (float)room.y};
			bottomLeft = { (float)room.x, (float)room.y};
			m_walls.push_back(Line2D{ topRight, topLeft, room.roomFillColor }); //top
			m_walls.push_back(Line2D{ bottomLeft, bottomRight, room.roomFillColor }); //bottom
			m_walls.push_back(Line2D{ topLeft, bottomLeft, room.roomFillColor }); //left
			m_walls.push_back(Line2D{ bottomRight, topRight, room.roomFillColor }); //right
		}
	}
}

bool FloorGenerator::CanBeSplit(Room& room) const
{
	return (
		room.w > m_minRoomArea &&
		room.h > m_minRoomArea
	);
}