#ifndef APP_HPP
#define APP_HPP

#include "GunningUp.h"

enum RenderMode
{
	TOPDOWN,
	FIRSTPERSON
};

//holds the data of ray collision in the scene
struct RayData {
	Color color{ 255,255,255,255 }; //default white
	float distance{9999.f};  //default to a very large number

};

class Ray2D {
public:
	Ray2D(Vector2 position, Vector2 direction, float distance){
		m_startPoint = position;
		m_endPoint = {
			position.x + direction.x * distance,
			position.y + direction.y * distance
		};
	}
private:
	Vector2 m_startPoint;
	Vector2 m_endPoint;
};

class App
{
public:
	App(int winWidth, int winHeight, char* title);
	~App();
	void update(float dt);
	void draw();

private:
	//minimap
	RenderTexture m_minimapTexture;
	Texture2D basicWall;
	int m_winwidth{ 700 };
	int m_winheight{ 500 };
	int m_fov{ 90 };
	int m_wallLength{ 8 };
	float wallSize = m_winheight*2.f;
	float playerHeight = wallSize * 0.666f;
	char* m_title{ "GunningUp" };
	Player* m_player;
	FloorGenerator* m_floor;
	Camera2D* m_camera;
	NPatchInfo m_sliceInfo;
public:
	bool running{true};
	raylib::Window* m_window;
	unsigned int m_viewMode{RenderMode::TOPDOWN};
};


#endif