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
	m_camera->offset = { GetScreenWidth() / 8.f, GetScreenHeight() / 8.f };
	m_minimapTexture = LoadRenderTexture(m_winwidth/4, m_winwidth/4);

	//load in the wall texture
	basicWall = LoadTexture("..\\assets\\textures\\test_wall.png");
	//this will be used to slice up the wall texture
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
	m_player->MoveAndCollideWithMap(m_floor->getWalls());

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
		float rayDistance = 150.f;

		//Trace Data
		Color traceColor{ BLACK };
		float traceDistance{ 99999 };
		Vector2 collisionPoint;
		Vector2 wallStart{};
		Vector2 wallEnd{};
		Vector2 pointAlongWall{};
		float percentAlongWall{};

		float tempDistance{ 0 };

		for (currentRayIndex = 0; currentRayIndex <= m_winwidth; currentRayIndex++) {
			float angleX = std::cosf(((float)currentRayIndex / (float)m_winwidth * (float)m_fov - (float)m_fov / 2.0f + angle) * TORADIANS);
			float angleY = std::sinf(((float)currentRayIndex / (float)m_winwidth * (float)m_fov - (float)m_fov / 2.0f + angle) * TORADIANS);
			//get the end point of the ray
			endPoint = {
				startPoint.x - ( angleX * rayDistance),
				startPoint.y - ( angleY * rayDistance)
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
				percentAlongWall = Vector2Distance(wallStart, pointAlongWall);
				percentAlongWall /= m_wallLength;
				percentAlongWall = percentAlongWall - (float)(int)percentAlongWall;
				//takes a sample of the texture for the wall and trnasforms it into the correct shape for the wall
				{
					m_sliceInfo.source = {
						(float)basicWall.width * percentAlongWall, // x
						0,
						1,
						(float)basicWall.height
					};
					m_sliceInfo.top = 0;
					m_sliceInfo.bottom = 0;
					m_sliceInfo.left = 0;
					m_sliceInfo.right = 0;

					//pre calculate values
					float height;
					float y;
					float currentAngle = ((float)currentRayIndex / (float)m_winwidth * (float)m_fov - (float)m_fov / 2.0f) * TORADIANS;	//convert the index to be between -0.5 and 0.5
					{
						y = (float)(m_winheight) / 2.0f; //move to halfway down the screen
						
						height = wallSize / (traceDistance * cosf(currentAngle));
						
						y -= height;
					}
					//draw floor
					DrawRectangle(0, m_winheight / 2 + playerHeight, m_winwidth, m_winheight / 2 - playerHeight, {122,122,122,255});
					//draw a wall
					DrawTextureNPatch(
						
						basicWall,		//original texture
						
						m_sliceInfo,	//slice info
						
						{				//destination
							(float)currentRayIndex,
							y + playerHeight,
							//(float)(m_winheight / 2) - wallSize / (traceDistance * std::cosf(((float)currentRayIndex / (float)m_winwidth * (float)m_fov - (float)m_fov / 2.0f) * TORADIANS)),
							1,
							height * 2.0f
							//(float)wallSize / (traceDistance * std::cosf(((float)currentRayIndex / (float)m_winwidth * (float)m_fov - (float)m_fov / 2.0f) * TORADIANS)) * 1.75f
						},
						
						Vector2{ 0,0 },	//origin

						0,				//rotation
						
						{				//tint (darkens based on distance)
							(unsigned char)(255.f * (1.0f - traceDistance / rayDistance)),	//r
							(unsigned char)(255.f * (1.0f - traceDistance / rayDistance)),	//g
							(unsigned char)(255.f * (1.0f - traceDistance / rayDistance)),	//b
							255																//a
						}
					);
				}
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
