#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "p2List.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"




class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

	//PhysBody
	PhysBody* Flipper_1;
	PhysBody* Flipper_2;
	PhysBody* Flipper_3;
	PhysBody* Flipper_4;
	PhysBody* Ball;

	//Textures
	SDL_Texture* Flipper_LU;
	SDL_Texture* Flipper_LD;
	SDL_Texture* Flipper_RD;
	SDL_Texture* Flipper_RU;
	SDL_Texture* Ball_Texture;



};