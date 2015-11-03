#pragma once
#include "Module.h"
#include "Globals.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}
	~PhysBody();

	void GetPosition(int& x, int &y) const;
	float GetRotation() const;
	void SetLinearSpeed(int x, int y);
	void SetAngularSpeed(float speed);
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;
	double GetAngle() const;
	void Set_Speed_Motor(float impulsive);
	void SetPosition(int x, int y, int angle);

public:
	int width, height;
	b2Body* body;
	Module* listener;
	SDL_Texture* texture;
};
//It's to create groups of filters to use with collisions
enum Fix_Category
{
	BALL = 1,
	GEAR = -1
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener // TODO
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Init();
	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	PhysBody* CreateCircle(int x, int y, int radius, int type, bool isSensor, bool isbullet, int restitution);
	PhysBody* CreateCircle(int x, int y, int radius, int type, bool isSensor, bool isbullet, int restitution, Fix_Category group);
	PhysBody* CreateRectangle(int x, int y, int width, int height, int angle, int type, int restitution);
	PhysBody* CreateRectangle(int x, int y, int width, int height, int angle, int type, int restitution, Fix_Category group);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, int angle);
	PhysBody* CreateChain(int x, int y, int* points, int size, float density, float restitution, bool isSensor, SDL_Texture* texture);
	PhysBody* CreateBoard(int x, int y, int *points, int size);
	PhysBody* CreateFlipper(bool isbullet, int angle_upper, int angle_lower, SDL_Texture* texture, int flipper_pos_x, int flipper_pos_y, int pivot_x, int pivot_y, int radious, int* points, uint size, float density, float restitution, bool ccd, bool isSensor, int localAnchorA_x, int localAnchorA_y, int localAnchorB_x, int localAnchorB_y);
	PhysBody* CreateGear(int x, int y, int width, int height, int radius);

	void CreatePrismaticJoint(PhysBody* body_1, PhysBody* body_2, int x_pivot_1, int y_pivot_1, int x_pivot_2, int y_pivot_2, int x_axis, int y_axis);
	void CreateDistanceJoint(PhysBody* body_1, PhysBody* body_2, int x_pivot_1, int y_pivot_1, int x_pivot_2, int y_pivot_2, float frequency, float damping);

	// b2ContactListener ---
	void DestroyBody(PhysBody* body);

private:

	bool debug;
	b2World* world;
	p2List<PhysBody*> bodies;
	b2MouseJoint* mouse_joint;
	b2Body* click_body;
	b2Body* ground;


};