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
	body2->velocity = Vec2(2, 0);
	body1->position = Vec2{0.0f, 0.65f};
	body2->position = Vec2{-0.4f, 0.7f};
	body3->position=Vec2{0,0.8};

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