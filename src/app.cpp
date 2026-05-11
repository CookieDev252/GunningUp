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

	m_player->update(dt);
	m_player->MoveAndCollideWithMap(m_floor->getRooms());

	m_camera->target = m_player->getPosition();
}

void App::draw()
{
	if (m_viewMode == RenderMode::TOPDOWN){

		m_window->BeginDrawing();


		BeginMode2D(*m_camera);

		m_window->ClearBackground({ 0,0,0,0 });

		/*
		* all drawing is done here
		*/

		m_floor->draw();

		m_player->draw();

		EndMode2D();

		m_window->EndDrawing();
	}
	else if (m_viewMode == RenderMode::FIRSTPERSON) {
		//start by casting rays into the scene

		for ( )
	}
}
