#ifndef APP_HPP
#define APP_HPP

#include "GunningUp.h"
#include<string>



enum State {
	MAINMENU,
	SPLASHSCREEN,
	GAMING,
	PAUSE,
	DEATH
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


/** class <App>
*
*/
class App
{
public:
	App(int winWidth, int winHeight, char* title);
	~App();
	void update(float dt);
	void draw();

private:
	//minimap
	RenderTexture m_minimapTexture; ///< a rectangular texture which will have the minimap drawn onto before rendering to the screen
	Texture2D TX2D_basicWall; ///< a texture for the wall
	Texture2D TX2D_basicEnemy; ///< a texture for the enemy
	Texture2D TX2D_gunHolding; ///< a texture for the gun
	float m_gunImageScale{0.75f};
	int m_winwidth{ 700 }; ///< width of the screen
	int m_winheight{ 500 }; ///< height of the screen
	int m_fov{ 90 }; ///< fov of the character
	int m_wallLength{ 8 }; ///< distance along wall before texture repeats (this prevents texture stretching
	float wallSize = 2000.f; ///< height of the wall
	float playerHeight = 10.f;
	float actualAngleDegrees{};
private:
	char* m_title{ "GunningUp" };
	Player* m_player;
	std::vector<Enemy> m_enemies;
	FloorGenerator* m_floor;
	Camera2D* m_camera;
	NPatchInfo m_sliceInfo;
	short int currentstate{};
public:
	bool running{true};
	raylib::Window* m_window;
	State currentState{ State::SPLASHSCREEN };
};


#endif