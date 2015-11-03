#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "p2Point.h"
#include "math.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif


PhysBody::~PhysBody()
{
	body->GetWorld()->DestroyBody(body);
	body = NULL;
	listener = NULL;
}

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Init()
{
	//LOG("Creating Physics environment");
	bool ret = true;

	return ret;
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{

	float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	world->Step(timeStep, velocityIterations, positionIterations);

	// Check for collisions on sensors (collision listener does not work for sensors)
	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if (c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if (pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if (!debug)
		return UPDATE_CONTINUE;

	b2Vec2 mouse_position(0, 0);
	click_body = NULL;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();
				App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
			}
			break;

			// Draw polygons ------------------------------------------------
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
				int32 count = polygonShape->GetVertexCount();
				b2Vec2 prev, v;

				for (int32 i = 0; i < count; ++i)
				{
					v = b->GetWorldPoint(polygonShape->GetVertex(i));
					if (i > 0)
						App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->GetVertex(0));
				App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
			}
			break;

			// Draw chains contour -------------------------------------------
			case b2Shape::e_chain:
			{
				b2ChainShape* shape = (b2ChainShape*)f->GetShape();
				b2Vec2 prev, v;

				for (int32 i = 0; i < shape->m_count; ++i)
				{
					v = b->GetWorldPoint(shape->m_vertices[i]);
					if (i > 0)
						App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
					prev = v;
				}

				v = b->GetWorldPoint(shape->m_vertices[0]);
				App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
			}
			break;
			}

			// MOUSE JOINT	
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				mouse_position.x = PIXEL_TO_METERS(App->input->GetMouseX());
				mouse_position.y = PIXEL_TO_METERS(App->input->GetMouseY());
				if (f->GetShape()->TestPoint(b->GetTransform(), mouse_position) == true)
				{
					click_body = f->GetBody();
					LOG("body_clicked\n");
				}
			}
		}
	}

	// creation of the joint
	if (click_body != NULL)
	{
		b2MouseJointDef def;
		def.bodyA = ground;
		def.bodyB = click_body;
		def.target = mouse_position;
		def.dampingRatio = 0.5f;
		def.frequencyHz = 20.0f;
		def.maxForce = 100.0f * click_body->GetMass();

		mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
	}

	if (mouse_joint != NULL && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		b2Vec2 posA, posB;
		posA = mouse_joint->GetAnchorA();
		posB.x = PIXEL_TO_METERS(App->input->GetMouseX());
		posB.y = PIXEL_TO_METERS(App->input->GetMouseY());
		mouse_joint->SetTarget(posB);

	}

	if (mouse_joint != NULL && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
	{
		world->DestroyJoint(mouse_joint);
		mouse_joint = NULL;
	}

	return UPDATE_CONTINUE;
}


bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	p2List_item<PhysBody*>* item = bodies.getFirst();

	while (item != NULL)
	{
		delete item->data;
		item = item->next;
	}

	bodies.clear();

	// Delete the whole physics world!
	delete world;

	return true;
}



PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, int type, bool isSensor, bool isbullet, int restitution)
{
	b2BodyDef body;
	if (type == 1)
	{
		body.type = b2_staticBody;
	}
	else if (type == 2)
	{
		body.type = b2_dynamicBody;
	}
	else if (type == 3)
	{
		body.type = b2_kinematicBody;
	}
	else
	{
		assert(type >= 3 || type < 1);
	}

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = 0.0f;
	body.bullet = isbullet;
	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;
	fixture.restitution = restitution;
	fixture.isSensor = isSensor;
	fixture.filter.groupIndex = BALL;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;
	bodies.add(pbody);


	return pbody;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, int type, bool isSensor, bool isbullet, int restitution, Fix_Category group)
{
	b2BodyDef body;
	if (type == 1)
	{
		body.type = b2_staticBody;
	}
	else if (type == 2)
	{
		body.type = b2_dynamicBody;
	}
	else if (type == 3)
	{
		body.type = b2_kinematicBody;
	}
	else
	{
		assert(type >= 3 || type < 1);
	}

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = 0.0f;
	body.bullet = isbullet;
	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;
	fixture.restitution = restitution;
	fixture.isSensor = isSensor;
	fixture.filter.groupIndex = group;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;
	bodies.add(pbody);


	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, int angle, int type, int restitution, Fix_Category group)
{
	b2BodyDef body;
	if (type == 1)
	{
		body.type = b2_staticBody;
	}
	else if (type == 2)
	{
		body.type = b2_dynamicBody;
	}
	else if (type == 3)
	{
		body.type = b2_kinematicBody;
	}
	else
	{
		assert(type >= 3 || type < 1);
	}
	body.angle = DEGTORAD * angle;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.restitution = restitution;
	fixture.density = 1.0f;
	fixture.filter.groupIndex = group;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;
	bodies.add(pbody);

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, int angle, int type, int restitution)
{
	b2BodyDef body;
	if (type == 1)
	{
		body.type = b2_staticBody;
	}
	else if (type == 2)
	{
		body.type = b2_dynamicBody;
	}
	else if (type == 3)
	{
		body.type = b2_kinematicBody;
	}
	else
	{
		assert(type >= 3 || type < 1);
	}
	body.angle = DEGTORAD * angle;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.restitution = restitution;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;
	bodies.add(pbody);

	return pbody;
}


PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height, int angle)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = DEGTORAD * angle;
	body.bullet = false;

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;
	bodies.add(pbody);

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, float density, float restitution, bool isSensor, SDL_Texture* texture)
{
	b2BodyDef Flipper_Def;
	Flipper_Def.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	Flipper_Def.type = b2_dynamicBody;
	b2Body* flipper = world->CreateBody(&Flipper_Def);

	b2PolygonShape Flipper_Shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	Flipper_Shape.Set(p, size / 2);

	b2FixtureDef Box_Fixture;
	Box_Fixture.shape = &Flipper_Shape;
	Box_Fixture.density = density;
	Box_Fixture.restitution = restitution;
	Box_Fixture.isSensor = isSensor;

	flipper->CreateFixture(&Box_Fixture);



	PhysBody* ret = new PhysBody();
	ret->body = flipper;
	ret->texture = texture;
	flipper->SetUserData(ret);
	ret->width = ret->height = 0;
	ret->width = ret->height = 0;
	bodies.add(ret);

	delete[]p;
	return ret;
}


PhysBody* ModulePhysics::CreateBoard(int x, int y, int *points, int size)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = 0.0f;

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (int i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);



	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->body->SetUserData(pbody);
	pbody->width = pbody->height = 0;
	bodies.add(pbody);
	delete[]p;
	return pbody;
}

// Called before quitting

void PhysBody::GetPosition(int& x, int &y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}


PhysBody* ModulePhysics::CreateFlipper(bool isbullet, int angle_upper, int angle_lower, SDL_Texture* texture, int flipper_pos_x, int flipper_pos_y, int pivot_pos_x, int pivot_pos_y, int radious, int* points, uint size, float density, float restitution, bool ccd, bool isSensor, int localAnchorA_x, int localAnchorA_y, int localAnchorB_x, int localAnchorB_y)
{

	PhysBody* Circle = CreateCircle(pivot_pos_x, pivot_pos_y, radious, 1, isSensor, isbullet, 0);
	PhysBody* Flipper = CreateChain(flipper_pos_x, flipper_pos_y, points, size, density, restitution, isSensor, texture);

	//JOINT
	b2RevoluteJointDef Revolute_Joint_Def;
	Revolute_Joint_Def.bodyA = Flipper->body;
	Revolute_Joint_Def.bodyB = Circle->body;
	Revolute_Joint_Def.collideConnected = false;

	//revolute_joint_def.enableMotor = true;
	//revolute_joint_def.motorSpeed = 0.0f;
	//revolute_joint_def.maxMotorTorque = 100.0f;

	if (angle_upper != INT_MAX && angle_lower != INT_MIN)
	{
		Revolute_Joint_Def.enableLimit = true;
		Revolute_Joint_Def.lowerAngle = DEGTORAD * angle_lower;
		Revolute_Joint_Def.upperAngle = DEGTORAD * angle_upper;
	}

	Revolute_Joint_Def.localAnchorA.Set(PIXEL_TO_METERS(localAnchorA_x), PIXEL_TO_METERS(localAnchorA_y));
	Revolute_Joint_Def.localAnchorB.Set(PIXEL_TO_METERS(localAnchorB_x), PIXEL_TO_METERS(localAnchorB_y));

	world->CreateJoint(&Revolute_Joint_Def);


	return Flipper;

}


void ModulePhysics::CreatePrismaticJoint(PhysBody* body_1, PhysBody* body_2, int x_pivot_1, int y_pivot_1, int x_pivot_2, int y_pivot_2, int x_axis, int y_axis)
{
	b2PrismaticJointDef def;

	//Joint creation
	def.bodyA = body_1->body;
	def.bodyB = body_2->body;
	def.collideConnected = false;

	def.localAxisA.Set(PIXEL_TO_METERS(x_axis), PIXEL_TO_METERS(y_axis));

	def.localAnchorA.Set(PIXEL_TO_METERS(x_pivot_1), PIXEL_TO_METERS(y_pivot_1));
	def.localAnchorB.Set(PIXEL_TO_METERS(x_pivot_2), PIXEL_TO_METERS(y_pivot_2));

	def.enableLimit = true;
	def.upperTranslation = PIXEL_TO_METERS(100);
	def.lowerTranslation = PIXEL_TO_METERS(60);

	/*def.enableMotor = true;
	def.maxMotorForce = 500;
	def.motorSpeed = PIXEL_TO_METERS(600);*/

	(b2PrismaticJoint*)world->CreateJoint(&def);
}

void ModulePhysics::CreateDistanceJoint(PhysBody* body_1, PhysBody* body_2, int x_pivot_1, int y_pivot_1, int x_pivot_2, int y_pivot_2, float frequency, float damping)
{
	b2DistanceJointDef def;

	def.bodyA = body_1->body;
	def.bodyB = body_2->body;
	def.collideConnected = false;
	def.length = PIXEL_TO_METERS(100.f);

	def.localAnchorA.Set(PIXEL_TO_METERS(x_pivot_1), PIXEL_TO_METERS(y_pivot_1));
	def.localAnchorB.Set(PIXEL_TO_METERS(x_pivot_2), PIXEL_TO_METERS(y_pivot_2));

	def.dampingRatio = damping; // 0 ... 1
	def.frequencyHz = frequency; // < 30.0f

	world->CreateJoint(&def);
}

PhysBody* ModulePhysics::CreateGear(int x, int y, int width, int height, int radius)
{
	PhysBody* tmp1;
	PhysBody* tmp2;

	tmp1 = CreateCircle(x, y, radius, 1, false, false, 0, GEAR);
	tmp2 = CreateRectangle(x, y, width, height, 0, 2, 0, GEAR);

	b2RevoluteJointDef defr;

	defr.bodyA = tmp1->body;
	defr.bodyB = tmp2->body;
	defr.localAnchorA = b2Vec2(0, 0);
	defr.localAnchorB = b2Vec2(0, 0);
	defr.collideConnected = false;
	defr.enableMotor = true;
	defr.maxMotorTorque = 0.1f;

	world->CreateJoint(&defr);

	return tmp2;
}

void PhysBody::Set_Speed_Motor(float impulsive)
{
	body->ApplyAngularImpulse(DEGTORAD * impulsive, true);
}

double PhysBody::GetAngle() const
{
	return RADTODEG * body->GetAngle();
}

void  ModulePhysics::DestroyBody(PhysBody* body)
{
	assert(body);
	bodies.del(bodies.findNode(body));
	delete body;
}

void PhysBody::SetPosition(int x, int y, int angle)
{
	body->SetTransform(b2Vec2(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y)), angle);
}

void PhysBody::SetLinearSpeed(int x, int y)
{
	body->SetLinearVelocity(b2Vec2(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y)));
}

void PhysBody::SetAngularSpeed(float speed)
{
	body->SetAngularVelocity(speed * DEGTORAD);
}