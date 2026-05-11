#include "floorGenerator.hpp"
#include <iostream>

FloorGenerator::FloorGenerator(int width, int height)
{
	m_rooms.push_back(Room(0, 0, width, height));

	GenerateLevelUnseeded();
}

void FloorGenerator::draw() {
	for (const Room& room : m_rooms) {
		DrawRectangle(room.x, room.y, room.w, room.h, room.roomFillColor);
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

void FloorGenerator::GenerateLevelUnseeded()
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
			if (firstSelectedRoom == selectedRoom) { return; } //no valid squares to split
		}
		SplitRoom(selectedRoom);
		selectedRoom = GetRandomValue(0, m_rooms.size()-1);
	}
	// convert all the rooms into walls
	{
		Vector2 topLeft;
		for (auto& room : m_rooms) {
			Vector2 topLeft = { (float)room.x, (float)room.y + (float)room.h};
			Vector2 topRight = { (float)room.x + (float)room.w, (float)room.y + (float)room.h};
			Vector2 bottomRight = { (float)room.x + (float)room.w, (float)room.y};
			Vector2 bottomLeft = { (float)room.x, (float)room.y};
			m_walls.push_back(Line2D{ topRight, topLeft }); //top
			m_walls.push_back(Line2D{ bottomLeft, topRight }); //bottom
			m_walls.push_back(Line2D{ topLeft, topRight }); //left
			m_walls.push_back(Line2D{ topLeft, topRight }); //right

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
