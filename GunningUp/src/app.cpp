#include "app.hpp"
#include <string>

//for the giving application, Pa is Raystart, Pb is RayEnd, and Pc is EnemyPos
float calculateHeight(Vector2 Pa, Vector2 Pb, Vector2 Pc) {
	float A, B, C, S, area, h;
	A = Vector2Distance(Pc, Pb);
	B = Vector2Distance(Pa, Pc);
	C = Vector2Distance(Pa, Pb);

	S = (A + B + C) / 2.f;

	area = sqrtf(S * (S - A) * (S - B) * (S - C));

	h = (2.f * area) / C;

	return h;
}



App::App(int winWidth, int winHeight, char* title) :
	m_winwidth(winWidth), m_winheight(winHeight), m_title(title)
{
	m_window = new raylib::Window(winWidth, winHeight, title);

	m_player = new Player(raylib::Vector2(512,512), raylib::Vector2(0,0));

	m_floor = new FloorGenerator(1024,1024);

	for (int i = 0; i < 20 /*enemies*/; i++) {
		Enemy tempEnemy = Enemy{ {0,0}, {0,0} };
		int nodeSelector = GetRandomValue(0, m_floor->getNavigationNodes().size() - 1);
		tempEnemy.setPosition(m_floor->getNavigationNodes()[nodeSelector].getPosition());
		tempEnemy.setNavigationNode(&m_floor->getNavigationNodes()[nodeSelector]);
		tempEnemy.setPlayer(m_player);
		m_enemies.push_back(tempEnemy);
	}

	m_camera = new Camera2D();
	m_camera->zoom = 1.0f;
	m_camera->offset = { GetScreenWidth() / 8.f, GetScreenHeight() / 8.f };
	m_minimapTexture = LoadRenderTexture(m_winwidth/4, m_winwidth/4);

	//load in the wall texture
	TX2D_basicWall = LoadTexture("..\\assets\\textures\\officeWallTexture.png");

	//load in the enemy
	TX2D_basicEnemy = LoadTexture("..\\assets\\textures\\roboguy.png");

	//load in the hand
	TX2D_gunHolding = LoadTexture("..\\assets\\textures\\gun_spriteSheet.png");

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
	UnloadTexture(TX2D_basicWall);
	UnloadTexture(TX2D_basicEnemy);
	UnloadTexture(TX2D_gunHolding);

}

void App::update(float dt)
{
	switch (currentState)
	{
	case State::SPLASHSCREEN:
	{
		currentState = State::MAINMENU;
	}
	case State::MAINMENU:
	{
		currentState = State::GAMING;
	}
	case State::GAMING:
	{
		//check if the window should close first
		if (m_window->ShouldClose()) {
			running = false;
			return;
		}

		m_player->update(dt);
		m_player->MoveAndCollideWithMap(m_floor->getWalls());
		for (Enemy& enemy : m_enemies) {
			enemy.MoveAndCollideWithMap(m_floor->getWalls());
			for (Bullet& bullet : enemy.getBullets()) {
				if (bullet.CollidesWithCircle(m_player->getPosition(), m_player->getSize())) {
					m_player->Damage(enemy.getDamage());
				}
				else {
					for (Line2D& wall : m_floor->getWalls()) {
						bullet.CollidesWithLine(wall);
					}
				}
			}
			for (Bullet& bullet : m_player->getBullets()) {
				if (bullet.CollidesWithCircle(enemy.getPosition(), enemy.getSize())) {
					enemy.Damage(m_player->getDamage());
				}
				else {
					for (Line2D& wall : m_floor->getWalls()) {
						bullet.CollidesWithLine(wall);
					}
				}
			}
		}

		m_camera->target = m_player->getPosition();
		m_camera->rotation = -m_player->getRotationHorizontals() - 90.f;

		for (Enemy& enemy : m_enemies) {
			enemy.update(dt);
			enemy.MoveAndCollideWithEnemies(m_enemies);
		}
	}
	case State::PAUSE:
	{
		//to be filled in
	}
	case State::DEATH:
	{
		//to be filled in
	}
	default:
	{
		currentState = State::SPLASHSCREEN;
	}

	}
		

	
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

			for (Enemy& enemy : m_enemies) {
				enemy.draw();
				for (Bullet bullet : enemy.getBullets()) {
					//bullet.Draw();
				}
			}

			m_player->draw();
			for (Bullet& bullet : m_player->getBullets()) {
				//bullet.Draw();
			}

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

		//draw floor
		DrawRectangle(0, m_winheight / 2 + playerHeight, m_winwidth, m_winheight / 2 - playerHeight, { 122,122,122,255 });

		for (currentRayIndex = 0; currentRayIndex <= m_winwidth; currentRayIndex++) {
			float angleX = std::cosf(((float)currentRayIndex / (float)m_winwidth * (float)m_fov - (float)m_fov / 2.0f + angle) * TORADIANS);
			float angleY = std::sinf(((float)currentRayIndex / (float)m_winwidth * (float)m_fov - (float)m_fov / 2.0f + angle) * TORADIANS);
			//get the end point of the ray
			endPoint = {
				startPoint.x - ( angleX * rayDistance),
				startPoint.y - ( angleY * rayDistance)
			};
			//pre calculate values
			float height;
			float y;
			float currentAngle = (((float)currentRayIndex / (float)m_winwidth) * 2.0f - 1.0f) / 2.0f;	//convert the index to be between -0.5 and 0.5
			currentAngle *= (float)m_fov;
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
						(float)TX2D_basicWall.width * percentAlongWall, // x
						0,
						1,
						(float)TX2D_basicWall.height
					};
					m_sliceInfo.top = 0;
					m_sliceInfo.bottom = 0;
					m_sliceInfo.left = 0;
					m_sliceInfo.right = 0;

					
					{
						y = (float)(m_winheight) / 2.0f; //move to halfway down the screen
						
						height = wallSize / (traceDistance * cosf(currentAngle * TORADIANS));
						
						y -= height;
					}
					
					//draw a wall
					DrawTextureNPatch(
						
						TX2D_basicWall,		//original texture
						
						m_sliceInfo,	//slice info
						
						{				//destination
							(float)currentRayIndex,
							y + playerHeight,
							1,
							height*2.0f
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
			//time to draw the enemy over the screen
			for (Enemy& enemy : m_enemies) {
				if (CheckCollisionCircleLine(enemy.getPosition(), enemy.getSize()/2.f, startPoint, endPoint)) {
					tempDistance = Vector2Distance(m_player->getPosition(), enemy.getPosition());
					if (tempDistance < traceDistance) {
						{
							y = (float)(m_winheight) / 2.0f; //move to halfway down the screen

							height = enemy.getHeight() / (tempDistance);

							y -= height;

							
							Vector2 A = Vector2Subtract(endPoint, startPoint);
							Vector2 B = Vector2Subtract(enemy.getPosition(), startPoint);

							float D = (A.x * B.y) - (A.y * B.x);

							{//convert it to [-1,1] then add 1 to be [0,2]
								D /= fabsf(D);
								D += 1.f;
							}

							percentAlongWall = calculateHeight(startPoint, endPoint, enemy.getPosition()) * D / enemy.getSize();
						}

						m_sliceInfo.source = {
						(float)TX2D_basicEnemy.width * percentAlongWall, // x
						0,
						1,
						(float)TX2D_basicEnemy.height
						};
						m_sliceInfo.top = 0;
						m_sliceInfo.bottom = 0;
						m_sliceInfo.left = 0;
						m_sliceInfo.right = 0;

						//draw a wall
						DrawTextureNPatch(

							TX2D_basicEnemy,		//original texture

							m_sliceInfo,	//slice info

							{				//destination
								(float)currentRayIndex,
								y + playerHeight,
								1,
								height * 2.0f
							},

							Vector2{ 0,0 },	//origin

							0,				//rotation

							{				//tint (darkens based on distance)
								(unsigned char)(255.f * (1.0f - tempDistance / rayDistance)),	//r
								(unsigned char)(255.f * (1.0f - tempDistance / rayDistance)),	//g
								(unsigned char)(255.f * (1.0f - tempDistance / rayDistance)),	//b
								255																//a
							}
						);

					}
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
	//handDrawing
	{
		if (m_player->getCooldown() > 0)
		{
			DrawTexturePro(
				TX2D_gunHolding,
				{ (float)TX2D_gunHolding.width / 2.f,0,(float)TX2D_gunHolding.width / 2.f,(float)TX2D_gunHolding.height },
				{ (float)m_winwidth / 2.f, (float)m_winheight / 4.f * 3.f, (float)TX2D_gunHolding.width / 2.f * m_gunImageScale, (float)TX2D_gunHolding.height * m_gunImageScale },
				{ 0,0 },
				0,
				{ WHITE }
			);
		}
		else {
			DrawTexturePro(
				TX2D_gunHolding,
				{ 0,0,(float)TX2D_gunHolding.width / 2.f,(float)TX2D_gunHolding.height },
				{ (float)m_winwidth / 2.f, (float)m_winheight / 4.f * 3.f, (float)TX2D_gunHolding.width / 2.f * m_gunImageScale, (float)TX2D_gunHolding.height * m_gunImageScale },
				{ 0,0 },
				0,
				{ WHITE }
			);
		}
		DrawText(std::to_string(m_player->getCooldown()).c_str(), 0, 0, 20, BLACK);
	}

	m_window->EndDrawing();
}
