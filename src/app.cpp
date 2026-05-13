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

	basicWall = LoadTexture("..\\assets\\textures\\test_wall.png");

	m_sliceInfo = NPatchInfo{};
	m_sliceInfo.layout = NPATCH_NINE_PATCH; // strectch it on both x and y axis
}

App::~App()
{
	delete m_window;
	delete m_player;
	delete m_floor;
	delete m_camera;

	UnloadRenderTexture(m_minimapTexture); // unload minimap texture
	UnloadTexture(basicWall);
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

		//Trace Data
		Color traceColor{ BLACK };
		float traceDistance{ 99999 };
		Vector2 collisionPoint;
		Vector2 wallStart{};
		Vector2 wallEnd{};
		Vector2 pointAlongWall{};
		float percentAlongWall{};

		float tempDistance{ 0 };

		for (currentRayIndex = -m_fov / 2; currentRayIndex <= m_fov / 2; currentRayIndex++) {
			//get the end point of the ray
			endPoint = {
				startPoint.x - (std::cosf(((float)currentRayIndex + angle) * TORADIANS) * rayDistance),
				startPoint.y - (std::sinf(((float)currentRayIndex + angle) * TORADIANS) * rayDistance)
			};
			//compare it do other walls
			for (Line2D& wall : m_floor->getWalls()) {
				//check for collision
				if (CheckCollisionLines(startPoint, endPoint, wall.startPoint, wall.endPoint, &collisionPoint)) {
					tempDistance = Vector2Distance(startPoint, collisionPoint);
					if (tempDistance < traceDistance) {
						//update the trace data
						traceDistance = tempDistance;
						traceColor = wall.color;
						wallStart = wall.startPoint;
						wallEnd = wall.endPoint;
						pointAlongWall = collisionPoint;
					}
				}
			}


			if (tempDistance > 0)
			{
				percentAlongWall = Vector2Distance(wallStart, pointAlongWall) / Vector2Distance(wallStart, wallEnd);
				DrawRectangle(
					(m_winwidth / m_fov) * (currentRayIndex + m_fov / 2)/*re centre the rayindex*/,
					(float)(m_winheight / 2) - wallSize / traceDistance,
					(m_winwidth / m_fov),
					wallSize / traceDistance * 2.0f,
					{
						(unsigned char)(Lerp(/*(float)traceColor.r*/0,255,percentAlongWall)),
						(unsigned char)(Lerp(/*(float)traceColor.g*/0,255,percentAlongWall)),
						(unsigned char)(Lerp(/*(float)traceColor.b*/0,255,percentAlongWall)),
						traceColor.a
					}
				);
				//slice of the texture
				m_sliceInfo.source = {
					(float)basicWall.width * percentAlongWall, // x
					0,
					1,
					(float)basicWall.height
				};
				m_sliceInfo.top = 1;
				m_sliceInfo.bottom = 1;
				m_sliceInfo.left = 1;
				m_sliceInfo.right = 1;
				DrawTextureNPatch(
					basicWall,		//original texture
					m_sliceInfo,	//slice info
					{				//destination
						(float)(m_winwidth / m_fov)* (currentRayIndex + m_fov / 2),
						(float)(m_winheight / 2) - wallSize / traceDistance,
						(float)(m_winwidth / m_fov),
						(float)wallSize / traceDistance * 2.0f 
					},
					Vector2{ 0,0 },	//origin
					0,				//rotation
					WHITE			//tint
				);
			}

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
