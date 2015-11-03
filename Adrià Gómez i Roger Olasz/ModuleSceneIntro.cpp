#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "BoardCoordenates.h"
#include "ModulePlayer.h"
#include "ModuleWindow.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	Img = NULL;
	ray_on = false;
	draw_box = false;
	turns = 3;
	score = 0;
	yellow_motor = 0;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//Load Textures----------------------------------------------------------
	Green_Light = App->textures->Load("pinball/Light.png");
	Green_Light_Middle = App->textures->Load("pinball/Light_2.png");
	Yellow_Light_texture = App->textures->Load("pinball/Light_3.png");
	Light_Circle_Texture = App->textures->Load("pinball/Score_Point_Down.png");
	Score_Circle_Texture = App->textures->Load("pinball/Score_Point.png");
	Spring_Texture = App->textures->Load("pinball/Tuberia.png");
	Yellow_Box_Entrance = App->textures->Load("pinball/Box_Restitution.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");
	Img = App->textures->Load("pinball/Tablero_Pinball.png");
	Gear_A = App->textures->Load("pinball/wheelA.png");

	//Creation of the walls of our board----------------------------------------
	Board.add(App->physics->CreateBoard(0, 0, Rectangle1, 12));
	Board.add(App->physics->CreateBoard(0, 0, Rectangle2, 18));
	Board.add(App->physics->CreateBoard(0, 0, Rectangle3, 22));
	Board.add(App->physics->CreateBoard(0, 0, Rectangle4, 20));
	Board.add(App->physics->CreateBoard(0, 0, Rectangle5, 16));
	Board.add(App->physics->CreateBoard(0, 0, Rectangle6, 16));
	Board.add(App->physics->CreateBoard(0, 0, Rectangle7, 90));
	Board.add(App->physics->CreateBoard(0, 0, Rectangle8, 16));
	Board.add(App->physics->CreateBoard(0, 0, Rectangle9, 16));
	Board.add(App->physics->CreateBoard(0, 0, Rectangle10, 16));
	Board.add(App->physics->CreateBoard(0, 0, Rectangle11, 42));
	Board.add(App->physics->CreateBoard(0, 0, Rectangle12, 18));
	Board.add(App->physics->CreateBoard(0, 0, Rectangle13, 18));

	//Creation Lights------------------------------------------------------------
	Lights.PushBack(Light(this, 62, 382, 23, 7, -54, lightTypes::Green_Light_Left_1, 1, 0));
	Lights.PushBack(Light(this, 45, 402, 23, 7, -54, lightTypes::Green_Light_Left_2, 1, 0));
	Lights.PushBack(Light(this, 116, 156, 20, 5, NULL, lightTypes::Green_Light_Up_1, 1, 0));
	Lights.PushBack(Light(this, 192, 156, 20, 5, NULL, lightTypes::Green_Light_Up_2, 1, 0));
	Lights.PushBack(Light(this, 267, 156, 20, 5, NULL, lightTypes::Green_Light_Up_3, 1, 0));
	Lights.PushBack(Light(this, 341, 157, 20, 5, NULL, lightTypes::Green_Light_Up_4, 1, 0));
	Lights.PushBack(Light(this, 153, 133, 13, 25, NULL, lightTypes::Yellow_Light, 1, 0));
	Lights.PushBack(Light(this, 229, 133, 13, 25, NULL, lightTypes::Yellow_Light, 1, 0));
	Lights.PushBack(Light(this, 305, 133, 13, 25, NULL, lightTypes::Yellow_Light, 1, 0));
	Lights.PushBack(Light(this, 305, 133, 13, 25, NULL, lightTypes::Yellow_Light, 1, 0));
	Lights.PushBack(Light(this, 305, 133, 13, 25, NULL, lightTypes::Yellow_Light, 1, 0));
	Lights.PushBack(Light(this, 107, 532, 0, 0, NULL, lightTypes::Scorepoint_Down_Left_1, 2, 9));
	Lights.PushBack(Light(this, 71, 510, 0, 0, NULL, lightTypes::Scorepoint_Down_Left_2, 2, 9));
	Lights.PushBack(Light(this, 41, 486, 0, 0, NULL, lightTypes::Scorepoint_Down_Left_3, 2, 9));
	Lights.PushBack(Light(this, 387, 520, 0, 0, NULL, lightTypes::Scorepoint_Down_Right_1, 2, 9));
	Lights.PushBack(Light(this, 419, 509, 0, 0, NULL, lightTypes::Scorepoint_Down_Right_2, 2, 9));
	Lights.PushBack(Light(this, 448, 490, 0, 0, NULL, lightTypes::Scorepoint_Down_Right_3, 2, 9));

	//Score Points Sensors--------------------------------------------------------------
	Scorepoint_1.body = App->physics->CreateCircle(114, 120, 20, 1, true, false, 0);
	Scorepoint_1.body->listener = this;
	Scorepoint_2.body = App->physics->CreateCircle(190, 120, 20, 1, true, false, 0);
	Scorepoint_2.body->listener = this;
	Scorepoint_3.body = App->physics->CreateCircle(266, 120, 20, 1, true, false, 0);
	Scorepoint_3.body->listener = this;
	Scorepoint_4.body = App->physics->CreateCircle(342, 120, 20, 1, true, false, 0);
	Scorepoint_4.body->listener = this;
	Scorepoint_5.body = App->physics->CreateCircle(142, 210, 20, 1, true, false, 0);
	Scorepoint_5.body->listener = this;
	Scorepoint_6.body = App->physics->CreateCircle(216, 327, 20, 1, true, false, 0);
	Scorepoint_6.body->listener = this;
	Scorepoint_7.body = App->physics->CreateCircle(335, 255, 20, 1, true, false, 0);
	Scorepoint_7.body->listener = this;

	//Texture SocrePoints--------------------------------------------------------------
	Scorepoint_1.texture = Scorepoint_2.texture = Scorepoint_3.texture = Scorepoint_4.texture = Scorepoint_5.texture = Scorepoint_6.texture = Scorepoint_7.texture = Score_Circle_Texture;

	//Score Points Body-----------------------------------------------------------------
	Body_Score_Point_1 = App->physics->CreateCircle(114, 120, 18, 1, false, false, 3);
	Body_Score_Point_1->listener = this;
	Body_Score_Point_2 = App->physics->CreateCircle(190, 120, 18, 1, false, false, 3);
	Body_Score_Point_2->listener = this;
	Body_Score_Point_3 = App->physics->CreateCircle(266, 120, 18, 1, false, false, 3);
	Body_Score_Point_3->listener = this;
	Body_Score_Point_4 = App->physics->CreateCircle(342, 120, 18, 1, false, false, 3);
	Body_Score_Point_4->listener = this;
	Body_Score_Point_5 = App->physics->CreateCircle(142, 210, 17, 1, false, false, 1);
	Body_Score_Point_5->listener = this;
	Body_Score_Point_6 = App->physics->CreateCircle(216, 327, 17, 1, false, false, 1);
	Body_Score_Point_6->listener = this;
	Body_Score_Point_7 = App->physics->CreateCircle(335, 255, 17, 1, false, false, 1);
	Body_Score_Point_7->listener = this;

	//Creation of Boxes with a high restitution-----------------------------------------
    Box_Entrance = App->physics->CreateRectangle(530, 356, 55, 5, -45, 1, 2);
	Box_Entrance->listener = this;
	Box_Colision_1 = App->physics->CreateRectangle(352, 499, 55, 5, -57, 1, 2);
	Box_Colision_1->listener = this;
	Box_Colision_2 = App->physics->CreateRectangle(143, 507, 55, 5, 54, 1, 2);
	Box_Colision_2->listener = this;

	//Create Sensors--------------------------------------------------------------------
	Sensor_Box_Entrance = App->physics->CreateRectangleSensor(533, 355, 63, 10, -45);
	Sensor_Box_Entrance->listener = this;
	Sensor_Entrance = App->physics->CreateRectangleSensor(478, 337, 40, 5, -50);
	Sensor_Entrance->listener = this;
	Sensor_Box_Colision_2 = App->physics->CreateRectangleSensor(144, 508, 59, 11, 53);
	Sensor_Box_Colision_2->listener = this;
	Sensor_Box_Colision_1 = App->physics->CreateRectangleSensor(351, 499, 59, 11, -57);
	Sensor_Box_Colision_1->listener = this;
	

	Sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 10, 0);
	Sensor->listener = this;

	Circle_Spring = App->physics->CreateCircle(483, 799, 4, 1, false, false, 0);
	Box_Spring = App->physics->CreateRectangle(477, 685, 21, 30, 0, 2, 0);
	App->physics->CreatePrismaticJoint(Circle_Spring, Box_Spring, 0, 0, 0, 0, 0, -1);
	App->physics->CreateDistanceJoint(Circle_Spring, Box_Spring, 0, 0, 0, 0, 20.0f, 1.0f);

	//Creating gears
	Gear = App->physics->CreateGear((SCREEN_WIDTH / 2) - 20, 240, 68, 6, 2);
	Gear2 = App->physics->CreateGear((SCREEN_WIDTH / 2) + 60, 380, 68, 6, 2);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{	
	static float spring_push = 0.0f;
	static float speed_motor = 0.0f;
	App->renderer->Blit(Img, 0, 0);

	int x, y;
	Box_Spring->GetPosition(x, y);
	App->renderer->Blit(Spring_Texture, x, y);

	Gear->GetPosition(x, y);
	App->renderer->Blit(Gear_A, x, y, NULL, 1.0f, Gear->GetRotation());
	Gear2->GetPosition(x, y);
	App->renderer->Blit(Gear_A, x, y, NULL, 1.0f, Gear2->GetAngle());

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if (App->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
	{
		speed_motor += 10;
		Gear->Set_Speed_Motor(speed_motor);
	}
	else
	{
		speed_motor = 0.0f;
	}

	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
	{
		speed_motor += 10;
		Gear2->Set_Speed_Motor(speed_motor);
	}
	else
	{
		speed_motor = 0.0f;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		spring_push += 70.f;
		Box_Spring->body->ApplyForceToCenter(b2Vec2(0, spring_push), true);
	}
	else
	{
		spring_push = 0.0f;
	}


	for (uint i = 0; i < Lights.Count(); ++i)
	{
		if (Lights[i].on == true)
		{
			//Draw the Lights of the Left----------------------------------------------
			if (Lights[i].type == Green_Light_Left_1)
			{
				App->renderer->Blit(Lights[i].texture, Lights[i].x - 10, Lights[i].y - 12);
			}

			if (Lights[i].type == Green_Light_Left_2)
			{
				App->renderer->Blit(Lights[i].texture, Lights[i].x - 8, Lights[i].y - 11);
			}

			//Draw the lights of the middle--------------------------------------------
			if (Lights[i].type == Green_Light_Up_1)
				App->renderer->Blit(Lights[i].texture, Lights[i].x - 10, Lights[i].y - 1);

			if (Lights[i].type == Green_Light_Up_2)
				App->renderer->Blit(Lights[i].texture, Lights[i].x - 10, Lights[i].y - 1);

			if (Lights[i].type == Green_Light_Up_3)
				App->renderer->Blit(Lights[i].texture, Lights[i].x - 10, Lights[i].y - 1);

			if (Lights[i].type == Green_Light_Up_4)
				App->renderer->Blit(Lights[i].texture, Lights[i].x - 10, Lights[i].y - 1);

			//Draw the lights in the midel of the boxes--------------------------------------------
			if (Lights[i].type == Yellow_Light)
			{
				App->renderer->Blit(Lights[i].texture, Lights[i].x - 7, Lights[i].y - 12);
			}

			if (Lights[i].type == Scorepoint_Down_Left_1)
			{
				App->renderer->Blit(Lights[i].texture, 94, 520);
				App->audio->PlayFx(bonus_fx);
				Lights[i].on = false;
			}

			if (Lights[i].type == Scorepoint_Down_Left_2)
			{
				App->renderer->Blit(Lights[i].texture, 60, 500);
				App->audio->PlayFx(bonus_fx);
				Lights[i].on = false;
			}

			if (Lights[i].type == Scorepoint_Down_Left_3)
			{
				App->renderer->Blit(Lights[i].texture, 29, 474);
				App->audio->PlayFx(bonus_fx);
				Lights[i].on = false;
			}

			if (Lights[i].type == Scorepoint_Down_Right_1)
			{
				App->renderer->Blit(Lights[i].texture, 376, 510);
				App->audio->PlayFx(bonus_fx);
				Lights[i].on = false;
			}

			if (Lights[i].type == Scorepoint_Down_Right_2)
			{
				App->renderer->Blit(Lights[i].texture, 408, 497);
				App->audio->PlayFx(bonus_fx);
				Lights[i].on = false;
			}	

			if (Lights[i].type == Scorepoint_Down_Right_3)
			{
				App->renderer->Blit(Lights[i].texture, 437, 478);
				App->audio->PlayFx(bonus_fx);
				Lights[i].on = false;
			}

		}
     
		
	}

	for (uint i = 0; i < Lights.Count(); i++)
	{
		if (Yellow_Light == Lights[i].type)
		{
			if (Lights[i].on == true)
			{
				yellow_motor++;

				if (yellow_motor == 3)
				{
					for (uint a = 0; a < Lights.Count(); a++)
					{
						if (Lights[a].on == true)
						{
							Lights[a].on = false;
						}
					}

					Gear->Set_Speed_Motor(10);
					Gear2->Set_Speed_Motor(10);
					yellow_motor = 0;
				}
			}

		}

	}

	if (Scorepoint_1.Hit_timer > 0)
	{
		if (SDL_TICKS_PASSED(SDL_GetTicks(), Scorepoint_1.Hit_timer) == false)
		{
			App->renderer->Blit(Scorepoint_1.texture, 0, 0);
		}
		else
		{
			Scorepoint_1.Hit_timer = 0;
		}
	}

	if (Scorepoint_2.Hit_timer > 0)
	{
		if (SDL_TICKS_PASSED(SDL_GetTicks(), Scorepoint_2.Hit_timer) == false)
		{
			App->renderer->Blit(Scorepoint_2.texture, 76, 0);
		}
		else
		{
			Scorepoint_2.Hit_timer = 0;
		}
	}

	if (Scorepoint_3.Hit_timer > 0)
	{
		if (SDL_TICKS_PASSED(SDL_GetTicks(), Scorepoint_3.Hit_timer) == false)
		{
			App->renderer->Blit(Scorepoint_3.texture, 153, 0);
		}
		else
		{
			Scorepoint_3.Hit_timer = 0;
		}
	}

	if (Scorepoint_4.Hit_timer > 0)
	{
		if (SDL_TICKS_PASSED(SDL_GetTicks(), Scorepoint_4.Hit_timer) == false)
		{
			App->renderer->Blit(Scorepoint_4.texture, 227, 0);
		}
		else
		{
			Scorepoint_4.Hit_timer = 0;
		}
	}

	if (Scorepoint_5.Hit_timer > 0)
	{
		if (SDL_TICKS_PASSED(SDL_GetTicks(), Scorepoint_5.Hit_timer) == false)
		{
			App->renderer->Blit(Scorepoint_5.texture, 29, 90);
		}
		else
		{
			Scorepoint_5.Hit_timer = 0;
		}
	}

	if (Scorepoint_6.Hit_timer > 0)
	{
		if (SDL_TICKS_PASSED(SDL_GetTicks(), Scorepoint_6.Hit_timer) == false)
		{
			App->renderer->Blit(Scorepoint_6.texture, 101, 209);
		}
		else
		{
			Scorepoint_6.Hit_timer = 0;
		}
	}

	if (Scorepoint_7.Hit_timer > 0)
	{
		if (SDL_TICKS_PASSED(SDL_GetTicks(), Scorepoint_7.Hit_timer) == false)
		{
			App->renderer->Blit(Scorepoint_7.texture, 221, 135);
		}
		else
		{
			Scorepoint_7.Hit_timer = 0;
		}
	}


	if (draw_box == true)
	{
		App->renderer->Blit(Yellow_Box_Entrance, 458, 332);
		Box_Entrance->SetPosition(480, 356, -51);
		Sensor_Box_Entrance->SetPosition(479, 355, -51);
		Sensor_Entrance->SetPosition(530, 337, -51);
	}

	

	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	char title[50];
	sprintf_s(title, "Balls: %d Score: %d", turns, score);
	App->window->SetTitle(title);


	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	
	if (Scorepoint_1.body == bodyA)
	{
		Scorepoint_1.Hit_timer = SDL_GetTicks() + 200;
		App->audio->PlayFx(bonus_fx);
		score += 100;
		return;
	}

	if (Scorepoint_2.body == bodyA)
	{
		Scorepoint_2.Hit_timer = SDL_GetTicks() + 200;
		App->audio->PlayFx(bonus_fx);
		score += 100;
		return;
	}

	if (Scorepoint_3.body == bodyA)
	{
		Scorepoint_3.Hit_timer = SDL_GetTicks() + 200;
		App->audio->PlayFx(bonus_fx);
		score += 100;
		return;
	}

	if (Scorepoint_4.body == bodyA)
	{
		Scorepoint_4.Hit_timer = SDL_GetTicks() + 200;
		App->audio->PlayFx(bonus_fx);
		score += 100;
		return;
	}

	if (Scorepoint_5.body == bodyA)
	{
		Scorepoint_5.Hit_timer = SDL_GetTicks() + 200;
		App->audio->PlayFx(bonus_fx);
		score += 100;
		return;
	}

	if (Scorepoint_6.body == bodyA)
	{
		Scorepoint_6.Hit_timer = SDL_GetTicks() + 200;
		App->audio->PlayFx(bonus_fx);
		score += 100;
		return;
	}

	if (Scorepoint_7.body == bodyA)
	{
		Scorepoint_7.Hit_timer = SDL_GetTicks() + 200;
		App->audio->PlayFx(bonus_fx);
		score += 100;
		return;
	}


	if (Sensor_Entrance == bodyA)
	{
		draw_box = true;
		return;
	}

	if (Sensor_Box_Entrance == bodyA)
	{
		App->audio->PlayFx(bonus_fx);
		return;
	}

	if (Sensor_Box_Colision_1 == bodyA)
	{
		App->audio->PlayFx(bonus_fx);
		return;
	}

	if (Sensor_Box_Colision_2 == bodyA)
	{
		App->audio->PlayFx(bonus_fx);
		return;
	}

	


	
	if (Sensor == bodyA)
	{
		App->player->Ball->SetLinearSpeed(0, 0);
		App->player->Ball->SetAngularSpeed(0);
		App->player->Ball->SetPosition(477, 675, 0);

		draw_box = false;
		Box_Entrance->SetPosition(530, 356, -51);
		Sensor_Box_Entrance->SetPosition(530, 355, -51);
		Sensor_Entrance->SetPosition(478, 337, -51);
		
		App->audio->PlayFx(bonus_fx);
	
		if (Lights_To_Del.Count() != NULL)
		{
			for (uint i = 0; i < Lights.Count(); ++i)
			{

				if (Lights_To_Del[Lights_To_Del.Count() - 1].type == Lights[i].type)
				{

					if (Lights[i].on == true)
					{
						Lights[i].on = false;
						Lights_To_Del.Pop();
						
					}
					
				}
			}
		}

		turns--;
		if (turns == 0)
		{
			turns = 3;
			score = 0;
		}
	
	}


	for (uint i = 0; i < Lights.Count(); ++i)
	{

		if (bodyA == Lights[i].body)
		{
			if (Lights[i].on == false)
			{
				Lights[i].on = true;

				if (Lights[i].type != Yellow_Light)
					Lights_To_Del.PushBack(Lights[i]);
			}
             
			return;
			}
			
		}
}




Light::Light(ModuleSceneIntro* scene, int x, int y, int width, int height, int angle, lightTypes type, int body_type, int radious)
{
	this->type = type;
	this->x = x;
	this->y = y;
	this->Radious = radious;
	this->width = width;
	this->height = height;
	this->Angle = angle;

	int radius;

	switch (type)
	{
	case Green_Light_Left_1:
		texture = scene->Green_Light;
		break;
	case Green_Light_Left_2:
		texture = scene->Green_Light;
		break;

	case Green_Light_Up_1:
		texture = scene->Green_Light_Middle;
		break;

	case Green_Light_Up_2:
		texture = scene->Green_Light_Middle;
		break;

	case Green_Light_Up_3:
		texture = scene->Green_Light_Middle;
		break;

	case Green_Light_Up_4:
		texture = scene->Green_Light_Middle;
		break;

	case Yellow_Light:
		texture = scene->Yellow_Light_texture;
		break;

	case Scorepoint_Down_Left_1:
		texture = scene->Light_Circle_Texture;
		break;

	case Scorepoint_Down_Left_2:
		texture = scene->Light_Circle_Texture;
		break;

	case Scorepoint_Down_Left_3:
		texture = scene->Light_Circle_Texture;
		break;

	case Scorepoint_Down_Right_1:
		texture = scene->Light_Circle_Texture;
		break;

	case Scorepoint_Down_Right_2:
		texture = scene->Light_Circle_Texture;
		break;

	case Scorepoint_Down_Right_3:
		texture = scene->Light_Circle_Texture;
		break;

	}

	assert(body_type > 0 && body_type < 3);

	if (body_type == 1)
	{
       body = scene->App->physics->CreateRectangleSensor(x, y, width, height, angle);
	   body->listener = scene;
	}
	else if (body_type == 2)
	{
		body = scene->App->physics->CreateCircle(x, y, radious, 1, true, false, 0);
		body->listener = scene;
	}
	
	on = false;
}