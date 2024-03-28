//
// Created by 韩昱川 on 3/24/24.
//
#include "engine/physics/world.h"
#include<iostream>

using std::vector;
using std::map;
using std::pair;
typedef pair<ArbiterKey, Arbiter> ArbPair;

World::World(Vec2 scale) : scale(scale), gravity(EngineSettings::gravity) {};

void World::addBody(std::shared_ptr<Body> &body) {
	bodies.push_back(body);
}

void World::step(float dt) {
	float inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;

	for (int i = 0; i < bodies.size(); ++i) {
		auto &bi = bodies[i];
		for (int j = i + 1; j < bodies.size(); ++j) {
			auto &bj = bodies[j];
			Arbiter newArb(bi, bj);
			ArbiterKey key(bi, bj);
			if (!newArb.contacts.empty()) {
				auto iter = arbiters.find(key);
				if (iter == arbiters.end()) {
					arbiters.insert(ArbPair(key, newArb));
				} else {
					iter->second.Update(newArb.contacts);
				}
			} else {
				arbiters.erase(key);
			}
		}
	}

	for (auto &b: bodies) {
		if (b->_invMass == 0.0f)
			continue;
		b->addVelocity(dt * (gravity + b->_invMass * b->force));
		b->addAngularVelocity(dt * b->invI * b->torque);
	}

	// Perform pre-steps.
	for (auto &arbiter: arbiters) {
		arbiter.second.PreStep(inv_dt);
	}

	// Perform iterations
	for (int i = 0; i < iterations; ++i) {
		for (auto &arbiter: arbiters) {
			arbiter.second.ApplyImpulse();
		}
	}

	for (const auto &b: bodies) {
		if(b->type!=BodyType::Static){
			b->setPosition(b->position + dt * b->_velocity);
			b->setRotation(b->rotation + dt * b->_angularVelocity);

		}
		b->force.Set(0.0f, 0.0f);
		b->torque = 0.0f;
	}
}

bool World::accumulateImpulses = true;
bool World::warmStarting = true;
bool World::positionCorrection = true;

std::shared_ptr<World> World::getInstance() {
	static std::shared_ptr<World> ret(new World(Vec2(100, 100)));
	return ret;
}
