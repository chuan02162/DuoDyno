#include "view/window.h"
#include "engine/physics/world.h"
#include "engine/physics/body.h"
#include<iostream>
#include<simd/simd.h>

int main() {
	auto world = World::getInstance();
	auto body1 = std::make_shared<Body>();
	auto body2 = std::make_shared<Body>();
	auto body3 = std::make_shared<Body>();
	body2->_velocity = Vec2(10, 0);
//	body2->angularVelocity=2;
	body1->setScale({200,10});
	body2->setScale({10, 20});
	body3->setScale({10, 10});

//	body2->setRotation(-0.1);
//	body2->setRotation(0);
	body1->type=BodyType::Static;
	body1->setMass(1e9);
	body1->setPosition({0,-85});
	body2->setPosition(Vec2{-20.f, 65.f});
	body3->setPosition(Vec2{10.f, 79.9});

	body1->friction=0.5;
	body2->friction=0.5;
	body3->friction=0.5;

	world->addBody(body1);
	world->addBody(body2);
	world->addBody(body3);

	NS::AutoreleasePool *pAutoreleasePool = NS::AutoreleasePool::alloc()->init();

	MyAppDelegate del;

	NS::Application *pSharedApplication = NS::Application::sharedApplication();
	pSharedApplication->setDelegate(&del);

	pSharedApplication->run();
	pAutoreleasePool->release();

	return 0;
}