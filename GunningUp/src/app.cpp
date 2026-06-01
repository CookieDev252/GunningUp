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

	InitAudioDevice();

	shootingSound = raylib::Sound{  };
	
	m_player = new Player(raylib::Vector2(512,512), raylib::Vector2(0,0));
	m_player->setShootSound("..\\assets\\audio\\shootSound.wav");
	m_player->setHurtSound("..\\assets\\audio\\hitHurt.wav");

	m_floor = new FloorGenerator(1024,1024);

	for (int i = 0; i < 20 /*enemies*/; i++) {
		Enemy tempEnemy = Enemy{ {0,0}, {0,0} };
		int nodeSelector = GetRandomValue(0, m_floor->getNavigationNodes().size() - 1);
		tempEnemy.setPosition(m_floor->getNavigationNodes()[nodeSelector].getPosition());
		tempEnemy.setNavigationNode(&m_floor->getNavigationNodes()[nodeSelector]);
		tempEnemy.setPlayer(m_player);
		tempEnemy.setShootSound("..\\assets\\audio\\shootSound.wav");
		tempEnemy.setHurtSound("..\\assets\\audio\\hitHurt.wav");
	
		m_enemies.push_back(tempEnemy);
	}

	m_camera = new Camera2D();
	m_camera->zoom = 1.0f;
	m_camera->offset = { GetScreenWidth() / 8.f, GetScreenHeight() / 8.f };
	m_minimapTexture = LoadRenderTexture(m_winwidth/4, m_winwidth/4);
	m_screenShot.height = m_winheight;
	m_screenShot.width = m_winwidth;

	//load in the wall texture
	TX2D_basicWall = LoadTexture("..\\assets\\textures\\officeWallTexture.png");

	//load in the enemy
	TX2D_basicEnemy = LoadTexture("..\\assets\\textures\\roboguy.png");

	//load in the hand
	TX2D_skybox = LoadTexture("..\\assets\\textures\\skybox.png");
	
	//load in the hand
	TX2D_gunHolding = LoadTexture("..\\assets\\textures\\gun_spriteSheet.png");

	//load in the Title card
	TX2D_Title = LoadTexture("..\\assets\\textures\\TitleCard.png");

	backgroundMusic = LoadMusicStream("..\\assets\\audio\\SplashScreen.mp3");
	backgroundMusic.looping = true;
	PlayMusicStream(backgroundMusic);

	explosionSound = LoadSound("..\\assets\\audio\\explosion.wav");


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
	UnloadTexture(m_screenShot); // unload the screenshot texture
	UnloadTexture(TX2D_basicWall); // unload the wall texture
	UnloadTexture(TX2D_basicEnemy); // unload the enemy texture
	UnloadTexture(TX2D_gunHolding); //  
	UnloadTexture(TX2D_skybox);
	UnloadTexture(TX2D_Title);
	UnloadMusicStream(backgroundMusic);
	CloseAudioDevice();

}

void App::update(float dt)
{
	if (currentState == MAINMENU)
	{
		UpdateMusicStream(backgroundMusic);

		//return back to gameplay
		if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
			if (m_currentSelectionUI == 0)
			{
				m_floor = new FloorGenerator(m_floor->getWidth(), m_floor->getHeight());
				m_player->Reset();
				m_player->setPosition(m_floor->getNavigationNodes()[GetRandomValue(0, m_floor->getNavigationNodes().size() - 1)].getPosition());
				for (Enemy& enemy : m_enemies) {
					enemy.Reset();
					enemy.setPosition(m_floor->getNavigationNodes()[GetRandomValue(0, m_floor->getNavigationNodes().size() - 1)].getPosition());
				}
				UnloadMusicStream(backgroundMusic);
				backgroundMusic = LoadMusicStream("..\\assets\\audio\\gameTheme.mp3");
				backgroundMusic.looping = true;
				PlayMusicStream(backgroundMusic);
				currentState = GAMING;
			}

			else if (m_currentSelectionUI == 2)
				currentState = running = false;
		}
		if (IsKeyPressed(KEY_UP) ||
			IsKeyPressed(KEY_W) ||
			IsKeyPressed(KEY_D) ||
			GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > m_leftStickDeadzoneY ||
			IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
			m_currentSelectionUI++;
			if (m_currentSelectionUI == 3)m_currentSelectionUI = 0;
		}
		if (IsKeyPressed(KEY_DOWN) ||
			IsKeyPressed(KEY_S) ||
			IsKeyPressed(KEY_A) ||
			GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < -m_leftStickDeadzoneY ||
			IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
			m_currentSelectionUI--;
			if (m_currentSelectionUI == -1)m_currentSelectionUI = 2;
		}
		return;
	}
	else if (currentState == SPLASHSCREEN)
	{
		UpdateMusicStream(backgroundMusic);
		timer += dt;
		if(timer > 5.f)
		{
			timer = 0;
			currentState = MAINMENU;
			UnloadMusicStream(backgroundMusic);
			backgroundMusic = LoadMusicStream("..\\assets\\audio\\mainmenuTheme.mp3");
			backgroundMusic.looping = true;
			PlayMusicStream(backgroundMusic);
		}
		return;
	}
	else if (currentState == GAMING)
	{
		UpdateMusicStream(backgroundMusic);
		for (Enemy& enemy : m_enemies) {
			if (enemy.getAliveStatus())
			{
				enemy.update(dt);
				enemy.MoveAndCollideWithEnemies(m_enemies);
			}
		}

		m_player->update(dt);
		m_player->MoveAndCollideWithMap(m_floor->getWalls());
		for (Enemy& enemy : m_enemies) {
			if (enemy.getAliveStatus())
			{
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
						enemy.setHit(true);
						if (!enemy.getAliveStatus())
							PlaySound(explosionSound);
					}
					else {
						for (Line2D& wall : m_floor->getWalls()) {
							bullet.CollidesWithLine(wall);
						}
					}
				}
			}
		}

		m_camera->target = m_player->getPosition();
		m_camera->rotation = -m_player->getRotationHorizontals() - 90.f;

		return;
	}
	else if (currentState == PAUSE)
	{
		//return back to gameplay
		if (IsKeyPressed(KEY_ENTER) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) { 
			if (m_currentSelectionUI == 0)
				currentState = GAMING;
			else if (m_currentSelectionUI == 2)
			{
				UnloadMusicStream(backgroundMusic);
				backgroundMusic = LoadMusicStream("..\\assets\\audio\\mainmenuTheme.mp3");
				backgroundMusic.looping = true;
				PlayMusicStream(backgroundMusic);
				currentState = MAINMENU;
			}
		}
		if (IsKeyPressed(KEY_UP) ||	
			IsKeyPressed(KEY_W) ||
			IsKeyPressed(KEY_D) ||
			GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > m_leftStickDeadzoneY ||
			IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
			m_currentSelectionUI++;
			if (m_currentSelectionUI == 3)m_currentSelectionUI = 0;
		}
		if (IsKeyPressed(KEY_DOWN) ||
			IsKeyPressed(KEY_S) ||
			IsKeyPressed(KEY_A) ||
			GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) < -m_leftStickDeadzoneY ||
			IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
			m_currentSelectionUI--;
			if (m_currentSelectionUI == -1)m_currentSelectionUI = 2;
		}

		return;
	}
	else
	{
		UnloadMusicStream(backgroundMusic);
		backgroundMusic = LoadMusicStream("..\\assets\\audio\\mainmenuTheme.mp3");
		backgroundMusic.looping = true;
		PlayMusicStream(backgroundMusic);
		currentState = MAINMENU;
		//to be filled in
	}
}

void App::draw()
{


	if(currentState == SPLASHSCREEN) {
		m_window->BeginDrawing();
		m_window->ClearBackground(WHITE);
		unsigned char greyscale = (1.f - timer / 5.f) * 255.f;
		{
			DrawRectangle(0, 0, m_winwidth, m_winheight, GREEN);
			DrawText("Gunning Up", m_winwidth / 2 - 100, m_winheight / 2 - 20, 40, BLACK);
		}
		DrawRectangle(0, 0, m_winwidth, m_winheight, {255,255,255,greyscale});
		m_window->EndDrawing();
		return;
	}
	else if (currentState == MAINMENU)
	{
		m_window->BeginDrawing();
		m_window->ClearBackground(BEIGE);
		DrawTexturePro(TX2D_Title, { 0,0,(float)TX2D_Title.width, (float)TX2D_Title.height }, {m_winwidth/4.f,m_winheight/8.f,m_winwidth/2.f,m_winheight/4.f}, { 0,0 }, sin(GetTime()), WHITE);

		for (int i = 0; i < 3; i++) {
			{
				if (m_currentSelectionUI == i)
				{
					DrawRectangle(m_startPosUI[0], m_startPosUI[1] + m_sizeOfUI[1] * i * 1.5, m_sizeOfUI[0], m_sizeOfUI[1], { 255, 215, 0, 125 });
				}
				else
				{
					DrawRectangle(m_startPosUI[0], m_startPosUI[1] + m_sizeOfUI[1] * i * 1.5, m_sizeOfUI[0], m_sizeOfUI[1], { 0,82,172,125 });
				}
				DrawText(m_mainMenuOptions[i], m_winwidth / 2.f, m_startPosUI[1] + m_sizeOfUI[i] * i * 1.5 + (m_sizeOfUI[1] / 2), m_sizeOfUI[1] / 2, WHITE);
			}
		}

		m_window->EndDrawing();
		return;
	}
	else if (currentState == DEATH)
	{
		//something
		
		return;
	}
	else if (currentState == PAUSE)
	{
		m_window->BeginDrawing();
		m_window->ClearBackground(BEIGE);
		

		for (int i = 0; i < 3; i++) {
			{
				if (m_currentSelectionUI == i) 
				{
					DrawRectangle(m_startPosUI[0], m_startPosUI[1] + m_sizeOfUI[1] * i * 1.5, m_sizeOfUI[0], m_sizeOfUI[1], { 255, 215, 0, 125});
				}else
				{
					DrawRectangle(m_startPosUI[0], m_startPosUI[1]+m_sizeOfUI[1]*i*1.5, m_sizeOfUI[0], m_sizeOfUI[1], {0,82,172,125});
				}
				DrawText(m_pauseOptions[i], m_winwidth / 2.f, m_startPosUI[1] + m_sizeOfUI[i] * i * 1.5 + (m_sizeOfUI[1] / 2), m_sizeOfUI[1] / 2, GREEN);
			}
		}

		m_window->EndDrawing();
		return;
	}
	//gaming
	else
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
					if (enemy.getAliveStatus())
						enemy.draw();
				}

				m_player->draw();

				EndMode2D();
			}
			EndTextureMode();
		}

		//Draw Rays
		{
			m_window->ClearBackground(WHITE);
			//draw skybox
			DrawTexturePro(TX2D_skybox, { ((m_player->getRotationHorizontals() - m_fov / 2) / 360.f * (float)TX2D_skybox.width),0, (float)TX2D_skybox.width * (m_fov/360.f),(float)TX2D_skybox.height }, { 0,0,(float)m_winwidth,(float)m_winheight }, { 0,0 }, 0, WHITE);


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
					startPoint.x - (angleX * rayDistance),
					startPoint.y - (angleY * rayDistance)
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
					//takes a sample of the texture for the wall and transforms it into the correct shape for the wall
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
								height * 2.0f
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
					if (enemy.getAliveStatus())
					{
						if (CheckCollisionCircleLine(enemy.getPosition(), enemy.getSize() / 2.f, startPoint, endPoint)) {
							tempDistance = Vector2Distance(m_player->getPosition(), enemy.getPosition());
							if (tempDistance < traceDistance) {
								{
									y = (float)(m_winheight) / 2.0f + wallSize / (tempDistance); //move to halfway down the screen

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
								(float)TX2D_basicEnemy.width * percentAlongWall/2.f + 
									((enemy.isShootingPlayer() * (enemy.getCooldown()<0.5f) + 
									(!enemy.isShootingPlayer() * ((int)(GetTime()*1000.f) % 2 == 1) * TX2D_basicEnemy.width/2.f))*TX2D_basicEnemy.width/2.f
								), // x
								enemy.isShootingPlayer()*TX2D_basicEnemy.height/2.f,
								1,
								(float)TX2D_basicEnemy.height/2.f
								};
								m_sliceInfo.top = 0;
								m_sliceInfo.bottom = 0;
								m_sliceInfo.left = 0;
								m_sliceInfo.right = 0;

								unsigned char ColourTint = (unsigned char)(255.f * (1.0f - tempDistance / rayDistance) * (float)!enemy.isHit());

								//draw a wall
								DrawTextureNPatch(

									TX2D_basicEnemy,		//original texture

									m_sliceInfo,	//slice info

									{				//destination
										(float)currentRayIndex,
										y ,
										1,
										height
									},

									Vector2{ 0,0 },	//origin

									0,				//rotation

									{				//tint (darkens based on distance)
										(unsigned char)(ColourTint + (125 * enemy.isHit())), //r
										ColourTint,	//g
										ColourTint,	//b
										255			//a
									}
								);
								if ((float)enemy.getHealth() / (float)enemy.getMaxHealth() > percentAlongWall)
								{
									DrawRectangle(currentRayIndex, y - 50.f / tempDistance, 1, 25.f / tempDistance, GREEN);
								}
								else {
									DrawRectangle(currentRayIndex, y - 50.f / tempDistance, 1, 25.f / tempDistance, RED);
								}
							}
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
			DrawRectangle(m_winwidth / 8 * 5, m_winheight * 0.125, m_winwidth / 4 + 4, m_winheight / 4 + 4, BLACK);
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
					{ (float)m_winwidth / 8.f*3.f, (float)m_winheight / 4.f * 3.f, m_winwidth/4.f, m_winheight/4.f },
					{ 0,0 },
					0,
					{ WHITE }
				);
			}
			else {
				DrawTexturePro(
					TX2D_gunHolding,
					{ 0,0,(float)TX2D_gunHolding.width / 2.f,(float)TX2D_gunHolding.height },
					{ (float)m_winwidth / 8.f * 3.f, (float)m_winheight / 4.f * 3.f, m_winwidth / 4.f, m_winheight / 4.f },
					{ 0,0 },
					0,
					{ WHITE }
				);
			}

		}
		//HUD
		{
			DrawRectangle(20, m_winheight - 60, m_winwidth - 40, 40, YELLOW);
			//amount of healh
			DrawText(("Health : " + std::to_string(m_player->getHealth())).c_str(), 30, m_winheight - 38, 8, BLACK);
			//amount of bullets
			int numberOfRoundsLeft = 0;
			for (Bullet& bullet : m_player->getBullets()) {
				numberOfRoundsLeft += bullet.CanBeFired();
			}
			//number of enemies alive
			DrawText(("Ammo : " + std::to_string(numberOfRoundsLeft)).c_str(), m_winwidth - 80, m_winheight - 38, 8, BLACK);
			int numberOfEnemiesLeft = 0;
			for (Enemy& enemy : m_enemies) {
				numberOfEnemiesLeft += enemy.getAliveStatus();
			}
			DrawText(("EnemiesLeft : " + std::to_string(numberOfEnemiesLeft) + " Out Of " + std::to_string(m_enemies.size())).c_str(), 40, 40, 20, RED);

			//DrawCrosshair
			int length = 10;
			int thickness = 2;
			DrawRectangle(m_winwidth / 2 - (length+thickness), m_winheight / 2 - thickness, length, thickness*2, BLACK); //left
			DrawRectangle(m_winwidth / 2 - (length+thickness/2), m_winheight / 2 - thickness/2, length, thickness, WHITE); //inner left
			DrawRectangle(m_winwidth / 2 + thickness, m_winheight / 2 - thickness, length, thickness*2, BLACK); //right
			DrawRectangle(m_winwidth / 2 + thickness/2, m_winheight / 2 - thickness/2, length, thickness, WHITE); //inner right
			DrawRectangle(m_winwidth / 2 - thickness, m_winheight / 2 - (length+thickness), thickness*2, length, BLACK); //up
			DrawRectangle(m_winwidth / 2 - thickness/2, m_winheight / 2 - (length+thickness/2), thickness, length, WHITE); //inner up
			DrawRectangle(m_winwidth / 2 - thickness, m_winheight / 2 + thickness, thickness * 2, length, BLACK); //down
			DrawRectangle(m_winwidth / 2 - thickness/2, m_winheight / 2 + thickness/2, thickness, length, WHITE); //inner down

		}


		m_window->EndDrawing();

		if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) || IsKeyPressed(KEY_ESCAPE)) {
			currentState = PAUSE;
			return;
		}

	

		currentState = MAINMENU;
		for (Enemy& enemy : m_enemies) {
			if (enemy.getAliveStatus())
			{
				currentState = GAMING;
				break;
			}

		}
		/*if (currentState = MAINMENU) {
			m_floor = new FloorGenerator(m_floor->getWidth(), m_floor->getHeight());
			m_player->Reset();
			m_player->setPosition(m_floor->getNavigationNodes()[GetRandomValue(0, m_floor->getNavigationNodes().size()-1)].getPosition());
			NavigationNode* NavNode;
			for (Enemy& enemy : m_enemies) {
				NavNode = &m_floor->getNavigationNodes()[GetRandomValue(0, m_floor->getNavigationNodes().size()-1)];
				enemy.setPosition(NavNode->getPosition());
				enemy.setNavigationNode(NavNode);
				enemy.Reset();
			}
		}*/

		if (!m_player->getAliveStatus()) {
			currentState = DEATH;
			return;
		}

		return;
	}
}
