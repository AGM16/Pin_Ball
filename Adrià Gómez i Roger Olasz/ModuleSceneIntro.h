#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "p2DynArray.h"


class ModuleSceneIntro;

struct ScorePoints
{
	ScorePoints() : body(NULL), texture(NULL), Hit_timer(0)
	{}

	PhysBody* body;
	SDL_Texture* texture;
	Uint32 Hit_timer;
};

enum lightTypes
{
	Green_Light_Left_1,
	Green_Light_Left_2,
	Green_Light_Up_1,
	Green_Light_Up_2,
	Green_Light_Up_3,
	Green_Light_Up_4,
	Yellow_Light,
	Scorepoint_Down_Left_1,
	Scorepoint_Down_Left_2,
	Scorepoint_Down_Left_3,
	Scorepoint_Down_Right_1,
	Scorepoint_Down_Right_2,
	Scorepoint_Down_Right_3


};

struct Light
{
	Light() : body(NULL), texture(NULL), on(false)
	{}

	Light(ModuleSceneIntro* physics, int x, int y, int width, int height, int angle, lightTypes type, int body_type, int radious);

	lightTypes type;
	PhysBody* body;
	SDL_Texture* texture;
	bool on;
	int x, y;
	int width, height;
    int Angle;
	int Radious;
};

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:
	
	//Board
	p2List<PhysBody*> Board;

	//Score Points
	ScorePoints Scorepoint_1;
	ScorePoints Scorepoint_2;
	ScorePoints Scorepoint_3;
	ScorePoints Scorepoint_4;
	ScorePoints Scorepoint_5;
	ScorePoints Scorepoint_6;
	ScorePoints Scorepoint_7;

	//PhysBody
	PhysBody* Sensor;
	PhysBody* Body_Score_Point_1;
	PhysBody* Body_Score_Point_2;
	PhysBody* Body_Score_Point_3;
	PhysBody* Body_Score_Point_4;
	PhysBody* Body_Score_Point_5;
	PhysBody* Body_Score_Point_6;
	PhysBody* Body_Score_Point_7;
	PhysBody* Sensor_Entrance;
	PhysBody* Box_Entrance;
	PhysBody* Sensor_Box_Entrance;
	PhysBody* Box_Colision_1;
	PhysBody* Box_Colision_2;
	PhysBody* Sensor_Box_Colision_1;
	PhysBody* Sensor_Box_Colision_2;
	PhysBody* Box_Spring;
	PhysBody* Circle_Spring;
	PhysBody* Gear;
	PhysBody* Gear2;

	//SDL Textures
	SDL_Texture* Gear_A;
	SDL_Texture* Img;
	SDL_Texture* Green_Light;
	SDL_Texture* Green_Light_Middle;
	SDL_Texture* Yellow_Light_texture;
	SDL_Texture* Score_Circle_Texture;
	SDL_Texture* Yellow_Box_Entrance;
	SDL_Texture* Light_Circle_Texture;
	SDL_Texture* Spring_Texture;

	//Lights
	lightTypes Light_Type;
	p2DynArray<Light> Lights;
	p2DynArray<Light> Lights_To_Del;

	//Others variables
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;
	bool draw_box;
	int yellow_motor;
	int turns;
	int score;
};
