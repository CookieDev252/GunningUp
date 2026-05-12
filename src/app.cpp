#include "app.hpp"
#include <string>

App::App(int winWidth, int winHeight, char* title) :
	m_winwidth(winWidth), m_winheight(winHeight), m_title(title)
{
	m_window = new raylib::Window(winWidth, winHeight, title);

	m_player = new Player(raylib::Vector2(winWidth/2,winHeight/2), raylib::Vector2(0,0));

	m_floor = new FloorGenerator(1024,1024);

	m_camera = new Camera2D();
	m_camera->zoom = 1.f;
	m_camera->offset = { GetScreenWidth() / 2.f, GetScreenHeight() / 2.f };
}

App::~App()
{
	delete m_window;
	delete m_player;
	delete m_floor;
	delete m_camera;
}

void App::update(float dt)
{
	//check if the window should close first
	if (m_window->ShouldClose()) {
		running = false;
		return;
	}

	if (IsKeyPressed(KEY_M)) { m_viewMode = RenderMode::FIRSTPERSON; }
	if (IsKeyPressed(KEY_N)) { m_viewMode = RenderMode::TOPDOWN; }

	m_player->update(dt);
	m_player->MoveAndCollideWithMap(m_floor->getRooms());

	m_camera->target = m_player->getPosition();
}

void App::draw()
{
	if (m_viewMode == RenderMode::TOPDOWN){

		m_window->BeginDrawing();


		BeginMode2D(*m_camera);

		m_window->ClearBackground(WHITE);

		/*
		* all drawing is done here
		*/

		m_floor->draw();

		m_player->draw();

		EndMode2D();

		m_window->EndDrawing();
	}
	else if (m_viewMode == RenderMode::FIRSTPERSON) {
		
		m_window->BeginDrawing();

		m_window->ClearBackground(WHITE);

		//start by casting rays into the scene
		int currentRayIndex;
		float angle = m_player->getRotationHorizontal();

		//ray data
		Vector2 startPoint = m_player->getPosition();
		Vector2 endPoint;
		float rayDistance = 50.f;

		//Trace Data
		Color traceColor{BLACK};
		float traceDistance{99999};
		Vector2 collisionPoint;

		float tempDistance{0};

		for (currentRayIndex = -m_fov/2; currentRayIndex < m_fov/2; currentRayIndex++) {
			//get the end point of the ray
			endPoint = {
				startPoint.x + std::cosf((angle + (float)currentRayIndex) * (PI / 180.f)) * rayDistance,
				startPoint.y + std::sinf((angle + 90.f + (float)currentRayIndex) * (PI / 180.f)) * rayDistance
			};
			//compare it do other walls
			for (Line2D& wall : m_floor->getWalls()) {
				//check for collision
				if (CheckCollisionLines(wall.startPoint,wall.endPoint,startPoint,endPoint,&collisionPoint)) {
					tempDistance = Vector2Distance(startPoint, collisionPoint);
					if (tempDistance < traceDistance) {
						//update the trace data
						traceDistance = tempDistance;
						traceColor = wall.color;
					}
				}
			}

			DrawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, BLACK);

			/*if (tempDistance != 0)
			{

				DrawRectangle(
					m_winwidth / m_fov * currentRayIndex,
					m_winheight / 4 - rayDistance / tempDistance * 10.f,
					m_winwidth / m_fov * currentRayIndex + 1,
					m_winheight / 4 * 3 + rayDistance / tempDistance * 10.f,
					traceColor
				);
			}*/

			tempDistance = 9999.f; // some big number
			traceColor = BLACK;

		}
		m_window->EndDrawing();
	}
}
