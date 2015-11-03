#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	Flipper_LU = App->textures->Load("Game/pinball/Flipper.png");
	Flipper_LD = App->textures->Load("Game/pinball/Flipper2.png");
	Flipper_RD = App->textures->Load("Game/pinball/Flipper3.png");
	Flipper_RU = App->textures->Load("Game/pinball/Flipper4.png");
	Ball_Texture = App->textures->Load("Game/pinball/ball.png");

	int Flipper[14] = {
		5, 1,
		15, 0,
		57, 6,
		55, 9,
		15, 15,
		5, 13,
		1, 6
	};


	int Flipper2[14] = {
		0, 12,
		6, 17,
		28, 19,
		73, 13,
		73, 9,
		30, 2,
		5, 0
	};

	int Flipper3[14] = {
		1, 16,
		46, 23,
		62, 22,
		71, 13,
		64, 4,
		36, 7,
		2, 13
	};

	int Flipper4[14] = {
		58, 8,
		54, 1,
		30, 2,
		2, 5,
		1, 8,
		32, 14,
		49, 15
	};


	//Creation of Bodys with revolution
	Flipper_1 = App->physics->CreateFlipper(false, 22, -30, NULL, 145, 589, 145, 589, 5, Flipper, 14, 2.0f, 0.3f, false, false, 6, 6, 0, 0);
	Flipper_2 = App->physics->CreateFlipper(false, 30, -30, NULL, 172, 654, 172, 654, 5, Flipper2, 14, 2.0f, 0.3f, false, false, 6, 7, 0, 0);
	Flipper_3 = App->physics->CreateFlipper(false, 30, -30, NULL, 305, 652, 305, 652, 5, Flipper3, 14, 2.0f, 0.3f, false, false, 63, 12, 0, 0);
	Flipper_4 = App->physics->CreateFlipper(false, 30, -30, NULL, 331, 585, 328, 582, 5, Flipper4, 14, 2.0f, 0.3f, false, false, 50, 6, 0, 0);

	//Creation Ball
	Ball = App->physics->CreateCircle(477, 665, 8, 2, false, true, 0.3f);
	Ball->listener = this;


	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	App->textures->Unload(Flipper_LU);
	App->textures->Unload(Flipper_LD);
	App->textures->Unload(Flipper_RD);
	App->textures->Unload(Flipper_RU);
	App->textures->Unload(Ball_Texture);

	App->physics->DestroyBody(Flipper_1);
	App->physics->DestroyBody(Flipper_2);
	App->physics->DestroyBody(Flipper_3);
	App->physics->DestroyBody(Flipper_4);
	App->physics->DestroyBody(Ball);

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	//Blit Textures
	int x, y;
	Flipper_1->GetPosition(x, y);

	App->renderer->Blit(Flipper_LU, x, y, NULL, 1.0f, Flipper_1->GetAngle(), 0, 0);

	Flipper_2->GetPosition(x, y);
	App->renderer->Blit(Flipper_LD, x, y, NULL, 1.0f, Flipper_2->GetAngle(), 0, 0);

	Flipper_3->GetPosition(x, y);
	App->renderer->Blit(Flipper_RD, x, y, NULL, 1.0f, Flipper_3->GetAngle(), 0, 0);

	Flipper_4->GetPosition(x, y);
	App->renderer->Blit(Flipper_RU, x, y, NULL, 1.0f, Flipper_4->GetAngle(), 0, 0);

	Ball->GetPosition(x, y);
	App->renderer->Blit(Ball_Texture, x, y, NULL, 1.0f, Ball->GetRotation());


	//Controls
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		Flipper_1->Set_Speed_Motor(-300.0f);
		Flipper_2->Set_Speed_Motor(-300.0f);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		Flipper_3->Set_Speed_Motor(300.0f);
		Flipper_4->Set_Speed_Motor(300.0f);
	}

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		Ball->body->ApplyForceToCenter(b2Vec2(0, -200), true);
	}

	return UPDATE_CONTINUE;
}


