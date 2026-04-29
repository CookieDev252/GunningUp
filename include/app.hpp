#ifndef APP_HPP
#define APP_HPP

#include "GunningUp.h"

class App
{
public:
	App(int winWidth, int winHeight, char* title);
	~App();
	void update(float dt);
	void draw();

private:
	int m_winwidth{ 700 };
	int m_winheight{ 500 };
	char* m_title{ "test window" };
	Player* m_player;
public:
	bool running{true};
	raylib::Window* m_window;
};


#endif