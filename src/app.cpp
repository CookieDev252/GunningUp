#include "app.hpp"

App::App(int winWidth, int winHeight, char* title) :
	m_winwidth(winWidth), m_winheight(winHeight), m_title(title)
{
	m_window = new raylib::Window(winWidth, winHeight, title);

	m_player = new Player();
}

App::~App()
{
	delete m_window;
	delete m_player;
}

void App::update(float dt)
{
	//check if the window should close first
	if (m_window->ShouldClose()) {
		running = false;
		return;
	}

	m_player->update(dt);

}

void App::draw()
{
	m_window->BeginDrawing();

	m_window->ClearBackground({0,0,0,0});

	/*
	* all drawing is done here
	*/

	m_player->draw();



	m_window->EndDrawing();
}
