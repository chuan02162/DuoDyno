//
// Created by 韩昱川 on 3/24/24.
//

#ifndef DUODYNO_WORLD_H
#define DUODYNO_WORLD_H
#include<vector>
#include<simd/simd.h>

#include "engine/common/engine_settings.h"
#include "arbiter.h"
#include "body.h"

#include<memory>
#include<map>

class World {
public:
	World(Vec2 scale);
	static std::shared_ptr<World> getInstance();
	void addBody(std::shared_ptr<Body> &body);
	void step(float dt);
//	void broadPhase();
	std::vector<std::shared_ptr<Body>> bodies;
	static bool accumulateImpulses;
	static bool warmStarting;
	static bool positionCorrection;
	int iterations=10;
	Vec2 scale;
private:
	Vec2 gravity;
	std::map<ArbiterKey, Arbiter> arbiters;
};

#endif //DUODYNO_WORLD_H
