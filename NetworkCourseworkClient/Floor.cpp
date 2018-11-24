#include "Floor.h"

void Floor::update()
{

}

void Floor::init()
{
	//setup floor physics body
	b2BodyDef floorBodyDef;
	floorBodyDef.type = b2_staticBody;
	floorBodyDef.position = b2Vec2(200, 400);
	physicsBody = physicsWorld->CreateBody(&floorBodyDef);

	//setup shape
	b2PolygonShape floorShape;
	floorShape.SetAsBox(400, 50);

	b2FixtureDef floorFixture;
	floorFixture.shape = &floorShape;

	//attach fixture
	physicsBody->CreateFixture(&floorFixture);
	physicsBody->SetUserData(this);
}