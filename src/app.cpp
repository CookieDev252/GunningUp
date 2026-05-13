#include "app.hpp"
#include <string>

App::App(int winWidth, int winHeight, char* title) :
	m_winwidth(winWidth), m_winheight(winHeight), m_title(title)
{
	m_window = new raylib::Window(winWidth, winHeight, title);

	m_player = new Player(raylib::Vector2(winWidth/2,winHeight/2), raylib::Vector2(0,0));

	m_floor = new FloorGenerator(1024,1024);

	m_camera = new Camera2D();
	m_camera->zoom = 2.f;
	m_camera->offset = { GetScreenWidth() / 8.f, GetScreenHeight() / 8.f };
	m_minimapTexture = LoadRenderTexture(m_winwidth/4, m_winwidth/4);

	basicWall = LoadTexture("./assets/textures/wall.png");
}

App::~App()
{
	delete m_window;
	delete m_player;
	delete m_floor;
	delete m_camera;

	UnloadRenderTexture(m_minimapTexture); // unload minimap texture
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
	m_camera->rotation = -m_player->getRotationHorizontals() - 90.f;
}

void App::draw()
{
	m_window->BeginDrawing();

	//Draw MiniMap
	{
		BeginTextureMode(m_minimapTexture);
		{
			BeginMode2D(*m_camera);

			m_window->ClearBackground(WHITE);

			/*
			* all drawing is done here
			*/

			m_floor->draw();

			m_player->draw();

			EndMode2D();
		}
		EndTextureMode();
	}
	
	//Draw Rays
	{
		m_window->ClearBackground(WHITE);

		//start by casting rays into the scene
		int currentRayIndex;
		float angle = m_player->getRotationHorizontals();

		//ray data
		Vector2 startPoint = m_player->getPosition();
		Vector2 endPoint;
		float rayDistance = 250.f;
		float actualRayDistance{ 0 };

		//Trace Data
		Color traceColor{ BLACK };
		float traceDistance{ 99999 };
		Vector2 collisionPoint;

		float tempDistance{ 0 };

		for (currentRayIndex = -m_fov / 2; currentRayIndex <= m_fov / 2; currentRayIndex++) {
			//get the end point of the ray
			endPoint = {
				startPoint.x - (std::cosf(((float)currentRayIndex + angle) * TORADIANS) * rayDistance) ,
				startPoint.y - (std::sinf(((float)currentRayIndex + angle) * TORADIANS) * rayDistance)
			};
			//compare it do other walls
			for (Line2D& wall : m_floor->getWalls()) {
				//check for collision
				if (CheckCollisionLines(startPoint, endPoint, wall.startPoint, wall.endPoint, &collisionPoint)) {
					tempDistance = Vector2Distance(startPoint, collisionPoint);
					actualRayDistance = Vector2Distance(startPoint, endPoint);
					if (tempDistance < traceDistance) {
						//update the trace data
						traceDistance = tempDistance;
						traceColor = wall.color;
					}
					//traceDistance = tempDistance;
					//traceColor = wall.color;
				}
			}


			if (tempDistance > 0)
			{
				float distancePercent = traceDistance / actualRayDistance;

				DrawRectangle(
					(m_winwidth / m_fov) * (currentRayIndex + m_fov / 2)/*re centre the rayindex*/,
					(float)(m_winheight / 2) - wallSize * (1.0f - distancePercent) + playerHeight,
					(m_winwidth / m_fov),
					wallSize * (1.0f - distancePercent),
					{
						(unsigned char)((float)traceColor.r * (1.0f - distancePercent)),
						(unsigned char)((float)traceColor.g * (1.0f - distancePercent)),
						(unsigned char)((float)traceColor.b * (1.0f - distancePercent)),
						traceColor.a
					}
				);
			}

			//DrawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, traceColor);

			traceDistance = 9999.f; // some big number
			traceColor = BLACK;

		}
	}

	//minimap rendering
	{
		//border for minimap
		DrawRectangle(m_winwidth / 8 * 5 , m_winheight * 0.125 , m_winwidth / 4 + 4, m_winheight / 4 + 4, BLACK);
		//draw in minimap over screen
		DrawTextureRec(
			m_minimapTexture.texture,
			//minimap to screen size rect
			{
				0,
				0,
				-(float)m_winwidth / 4.f,
				(float)m_winheight / 4.f
			},
			//minimap position
			{
				(float)m_winwidth / 8.f * 5.f,
				(float)m_winheight / 8.f
			},
			WHITE //no tint
		);
	}

	m_window->EndDrawing();
}
