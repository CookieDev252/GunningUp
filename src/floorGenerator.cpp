#include "floorGenerator.hpp"
#include <iostream>

FloorGenerator::FloorGenerator(int width, int height)
{
	m_rooms.push_back(Room(0, 0, width, height));
	this->width = width;
	this->height = height;

	GenerateLevel();
}

void FloorGenerator::draw() {
	for (const Line2D& wall : m_walls) {
		DrawLine(wall.startPoint.x, wall.startPoint.y, wall.endPoint.x, wall.endPoint.y, wall.color);
	}
	for (const NavigationNode& node : m_navNodes) {
		DrawCircleV(node.position, 5, GREEN);
		for (const NavigationNode* connectedNode : node.m_connectedNodes) {
			DrawLineV(node.position, connectedNode->position, BLACK);
		}
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
		m_navNodes.push_back(NavigationNode{ Vector2{(float)(room.x + room.w / 2 + offset), (float)(room.y)} });
		m_rooms.insert(m_rooms.begin() + selectedRoom+1,
			Room(
				room.x + room.w / 2 + offset + m_intersection,
				room.y,
				room.w / 2 - offset - m_intersection,
				room.h
			)
		);
		m_navNodes.push_back(NavigationNode{ Vector2{(float)(room.x + room.w / 2 + offset), (float)(room.y + room.h)} });
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
		m_navNodes.push_back(NavigationNode{ Vector2{(float)(room.x), (float)(room.y + room.h / 2 + offset)} });
		m_rooms.insert(m_rooms.begin() + selectedRoom+1,
			Room(
				room.x,
				room.y + room.h / 2 + offset + m_intersection,
				room.w,
				room.h / 2 - offset - m_intersection
			)
		);
		m_navNodes.push_back(NavigationNode{ Vector2{(float)(room.x + room.w), (float)(room.y + room.h / 2 + offset)} });
		m_rooms.erase(m_rooms.begin() + selectedRoom);
	}
}

void FloorGenerator::GenerateLevel()
{
	int splits; // counter for which split it's on
	int selectedRoom = 0; //set to 0 because there should only be 1 room right now
	int firstSelectedRoom;
	bool splitfound = false;
	for (splits = 0; splits < m_splitCount; splits++) {
		//here we'll select a random room, check if it's splittable and if true
		//cut through the longest side (this'll prevent too skinny rooms)
		if (CanBeSplit(m_rooms[selectedRoom])) { SplitRoom(selectedRoom); }
		firstSelectedRoom = selectedRoom;
		while (!CanBeSplit(m_rooms[selectedRoom])) { 
			selectedRoom += 1; if (selectedRoom == m_rooms.size()) selectedRoom = 0;
			if (firstSelectedRoom == selectedRoom) { break; } //no valid squares to split#
			if (CanBeSplit(m_rooms[selectedRoom])) { splitfound = true; break; }
		}
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
			//create four wallls
			m_walls.push_back(Line2D{ topRight, topLeft, room.roomFillColor }); //top
			m_walls.push_back(Line2D{ bottomLeft, bottomRight, room.roomFillColor }); //bottom
			m_walls.push_back(Line2D{ topLeft, bottomLeft, room.roomFillColor }); //left
			m_walls.push_back(Line2D{ bottomRight, topRight, room.roomFillColor }); //right
			//create a random door
			int selected = GetRandomValue(1, 4);
			Line2D wall = m_walls.at(m_walls.size() - selected);
			m_walls.erase(m_walls.end() - selected);
			//top and bottom
			m_walls.push_back(Line2D{
				wall.startPoint, 
				Vector2Add(wall.startPoint,Vector2Scale(Vector2Subtract(wall.endPoint, wall.startPoint),0.4)),
				wall.color
				}
			);
			m_walls.push_back(Line2D{
				Vector2Add(wall.startPoint,Vector2Scale(Vector2Subtract(wall.endPoint, wall.startPoint),0.6)),
				wall.endPoint,
				wall.color
				}
			);
		}
	}
	//add back in the map walls
	m_walls.push_back(Line2D{ Vector2{0,0}, Vector2{(float)width,0}, {0,0,0,0} });
	m_walls.push_back(Line2D{ Vector2{(float)width,0}, Vector2{(float)width,(float)height}, {0,0,0,0}});
	m_walls.push_back(Line2D{ Vector2{(float)width,(float)height}, Vector2{0,(float)height}, {0,0,0,0}});
	m_walls.push_back(Line2D{ Vector2{0,(float)height}, Vector2{0,0}, {0,0,0,0} });

	Vector2* emptyPointer = new Vector2{};

	int skippingNode = 0, currentNode = 0, wallsObstructing = 0;
	for (NavigationNode& node : m_navNodes) {
		for (NavigationNode& otherNode : m_navNodes) {
			if (skippingNode != currentNode) {
				for (Line2D& wall : m_walls) {
					//check if the line between the two nodes is obstructed by a wall
					wallsObstructing += CheckCollisionLines(node.position, otherNode.position, wall.startPoint, wall.endPoint, emptyPointer);
					std::cout << "looping walls" << std::endl;
				}
				if (wallsObstructing == 0) node.m_connectedNodes.push_back(&otherNode);
			}
			currentNode++;
		}
		skippingNode++;
		currentNode = 0;
		wallsObstructing = 0;
	}

	delete emptyPointer;
}

bool FloorGenerator::CanBeSplit(Room& room) const
{
	return (
		room.w > m_minRoomArea * 2 &&
		room.h > m_minRoomArea * 2
	);
}