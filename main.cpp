#include "view/window.h"
#include "engine/physics/world.h"
#include "engine/physics/body.h"
#include<iostream>
#include<simd/simd.h>

int main() {
	auto world = World::getInstance();
	vector<shared_ptr<Body>> bodies;
	int n = 5;
	for (int i = 1; i <= n; ++i) {
		for (int j = 1; j <= n - i + 1; ++j) {
			auto body = make_shared<Body>();
			body->setPosition({float(-70 + 10 * i + j * 10),float(-85+(i-1)*10)});
			body->setScale({10,10});
			world->addBody(body);
		}
	}
	auto floor = std::make_shared<Body>();
	auto bullet = std::make_shared<Body>();
	bullet->setPosition({-90,-80});
	bullet->setMass(100);
	bullet->setScale({20,20});
	bullet->friction=0.1;
	bullet->_velocity=Vec2(25,0);
	floor->setScale({200, 10});

	floor->type = BodyType::Static;
	floor->setMass(1e9);
	floor->setPosition({0, -95});

	world->addBody(floor);
	world->addBody(bullet);

	NS::AutoreleasePool *pAutoreleasePool = NS::AutoreleasePool::alloc()->init();

	MyAppDelegate del;

	NS::Application *pSharedApplication = NS::Application::sharedApplication();
	pSharedApplication->setDelegate(&del);

	pSharedApplication->run();
	pAutoreleasePool->release();

	return 0;
}