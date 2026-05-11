#ifndef APP_HPP
#define APP_HPP

#include "GunningUp.h"

enum RenderMode
{
	TOPDOWN,
	FIRSTPERSON
};

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
	int m_fov{ 90 };
	char* m_title{ "test window" };
	Player* m_player;
	FloorGenerator* m_floor;
	Camera2D* m_camera;
public:
	bool running{true};
	raylib::Window* m_window;
	unsigned int m_viewMode{RenderMode::TOPDOWN};
};


#endif