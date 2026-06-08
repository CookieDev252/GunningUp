#include "floorGenerator.hpp"


FloorGenerator::FloorGenerator(int width, int height)
{
	m_rooms.push_back(Room(m_intersection*2, m_intersection*2, width-m_intersection*4, height-m_intersection*4));
	this->m_width = width;
	this->m_height = height;
	GenerateLevel();
}

void FloorGenerator::draw() {
	for (const Line2D& wall : m_walls) { 
		DrawLine(wall.startPoint.x, wall.startPoint.y, wall.endPoint.x, wall.endPoint.y, wall.color);
	}
	for (const NavigationNode& nav : m_navNodes) {
		DrawCircle(nav.getPosition().x, nav.getPosition().y, m_intersection, { 0,255,0,50 }); //barely visisble green
		for (const NavigationNode* nav2 : nav.m_connectedNodes) {
			DrawLineV(nav.getPosition(), nav2->getPosition(), { 255,0,0,50 });
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
		m_navNodes.push_back(NavigationNode{ Vector2{(float)(room.x + room.w / 2 + offset), (float)(room.y) - m_intersection} });
		m_rooms.insert(m_rooms.begin() + selectedRoom+1,
			Room(
				room.x + room.w / 2 + offset + m_intersection,
				room.y,
				room.w / 2 - offset - m_intersection,
				room.h
			)
		);
		m_navNodes.push_back(NavigationNode{ Vector2{(float)(room.x + room.w / 2 + offset), (float)(room.y + room.h)+m_intersection} });
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
		m_navNodes.push_back(NavigationNode{ Vector2{(float)(room.x)-m_intersection, (float)(room.y + room.h / 2 + offset)} });
		m_rooms.insert(m_rooms.begin() + selectedRoom+1,
			Room(
				room.x,
				room.y + room.h / 2 + offset + m_intersection,
				room.w,
				room.h / 2 - offset - m_intersection
			)
		);
		m_navNodes.push_back(NavigationNode{ Vector2{(float)(room.x + room.w)+m_intersection, (float)(room.y + room.h / 2 + offset)} });
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
			Vector2 normalDirection = Vector2Normalize(Vector2Subtract(wall.endPoint, wall.startPoint));
			m_walls.erase(m_walls.end() - selected);
			m_walls.push_back(Line2D{
				wall.startPoint, 
				Vector2Add(wall.startPoint,Vector2Subtract(Vector2Scale(Vector2Subtract(wall.endPoint, wall.startPoint),0.5),Vector2Scale(normalDirection,m_intersection))),
				wall.color
				}
			);
			m_walls.push_back(Line2D{
				Vector2Add(wall.startPoint,Vector2Add(Vector2Scale(Vector2Subtract(wall.endPoint, wall.startPoint),0.5), Vector2Scale(normalDirection,m_intersection))),
				wall.endPoint,
				wall.color
				}
			);
			m_navNodes.push_back(NavigationNode{ Vector2Add(Vector2Scale(Vector2Add(wall.startPoint,wall.endPoint),0.5),Vector2Scale(wall.normal,m_intersection/2.f)) });

			m_navNodes.push_back(NavigationNode{ Vector2Subtract(Vector2Scale(Vector2Add(wall.startPoint,wall.endPoint),0.5),Vector2Scale(wall.normal,m_intersection / 2.f))});
		}
	}
	//add back in the map walls
	m_walls.push_back(Line2D{ Vector2{0,0}, Vector2{(float)m_width,0}, BLACK });
	m_walls.push_back(Line2D{ Vector2{(float)m_width,0}, Vector2{(float)m_width,(float)m_height}, BLACK });
	m_walls.push_back(Line2D{ Vector2{(float)m_width,(float)m_height}, Vector2{0,(float)m_height}, BLACK});
	m_walls.push_back(Line2D{ Vector2{0,(float)m_height}, Vector2{0,0}, BLACK });

	Vector2* emptyPointer = new Vector2{};

	int skippingNode = 0, currentNode = 0, wallsObstructing = 0;
	//Loop through all the nodes to connect them to visible nodes
	for (NavigationNode& node : m_navNodes) {
		for (NavigationNode& otherNode : m_navNodes) {
			if (skippingNode != currentNode) {
				for (Line2D& wall : m_walls) {
					//check if the line between the two nodes is obstructed by a wall
					if (CheckCollisionLines(node.getPosition(), otherNode.getPosition(), wall.startPoint, wall.endPoint, emptyPointer)) {
						wallsObstructing++;
					}
				}
				if (wallsObstructing == 0) {
					node.m_connectedNodes.push_back(&otherNode);
				}
			}
			wallsObstructing = 0;
			currentNode++;
		}
		skippingNode++;
		currentNode = 0;
	}
	//remove the temporary pointer
	delete emptyPointer;

	//loop through the Navigation nodes, but this time culling all nodes to 4 nearest nodes
	for (NavigationNode& node : m_navNodes) { // iterate the base nodes
		if (node.m_connectedNodes.size() > 4)
		{
			//sort the nodes by their distance from the original
			std::sort(node.m_connectedNodes.begin(), node.m_connectedNodes.end(), [node](const NavigationNode* a, const NavigationNode* b) {
				return Vector2DistanceSqr(node.getPosition(), a->getPosition()) < Vector2DistanceSqr(node.getPosition(), b->getPosition());
				});
			//truncate all the unnecessary nodes
			for (int i = 4; i < node.m_connectedNodes.size(); i) {
				node.RemoveNode(node.m_connectedNodes[i]);
			}
		}
	}
}

bool FloorGenerator::CanBeSplit(Room& room) const
{
	return (
		room.w > m_minRoomArea * 2 &&
		room.h > m_minRoomArea * 2
	);
}

void NavigationNode::RemoveNode(NavigationNode* node)
{
	int i = 0; 
	//iter through each Node
	for (NavigationNode* n : m_connectedNodes) 
	{ 
		//if the position in memory is the same as the target
		if (n == node) {
			//remove it from the list
			m_connectedNodes.erase(m_connectedNodes.begin() + i);
			return;
		}
		i++;
	}
	std::cout << "can't find node" << std::endl;
	return;
}
