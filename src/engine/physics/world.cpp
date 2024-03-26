//
// Created by 韩昱川 on 3/24/24.
//
#include "engine/physics/world.h"
#include<iostream>

using std::vector;
using std::map;
using std::pair;
typedef map<ArbiterKey, Arbiter>::iterator ArbIter;
typedef pair<ArbiterKey, Arbiter> ArbPair;

void World::addBody(std::shared_ptr<Body> &body) {
	bodies.push_back(std::move(body));
}

void World::step(float dt) {
	float inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;

	// Determine overlapping bodies and update contact points.
	broadPhase();

	// Integrate forces.
	for (auto & b : bodies)
	{
		if (b->invMass == 0.0f)
			continue;

		b->velocity += dt * (gravity + b->invMass * b->force);
		b->angularVelocity += dt * b->invI * b->torque;
	}

	// Perform pre-steps.
	for (auto & arbiter : arbiters)
	{
		arbiter.second.PreStep(inv_dt);
	}

	// Perform iterations
	for (int i = 0; i < iterations; ++i)
	{
		for (auto & arbiter : arbiters)
		{
			arbiter.second.ApplyImpulse();
		}
	}

	for (const auto& b : bodies)
	{
		b->position += dt * b->velocity;
		b->rotation += dt * b->angularVelocity;

		b->force.Set(0.0f, 0.0f);
		b->torque = 0.0f;
	}

//	for(auto &body :bodies){
//		std::cout<<body->angularVelocity<<std::endl;
//	}
//	std::cout<<":::::"<<std::endl;

//	for (auto &body: bodies) {
//		body->addForce(-gravity * body->mass);
//	}
//	for (auto &body: bodies) {
//		body->update(dt);
//	}
}

bool World::accumulateImpulses = true;
bool World::warmStarting = true;
bool World::positionCorrection = true;

World::World() {
	gravity = EngineSettings::gravity;
}

void World::broadPhase() {
	// O(n^2) broad-phase
	for (int i = 0; i < (int) bodies.size(); ++i) {
		auto &bi = bodies[i];

		for (int j = i + 1; j < (int) bodies.size(); ++j) {
			auto &bj = bodies[j];

			if (bi->mass == 0.0f && bj->mass == 0.0f)
				continue;

			Arbiter newArb(bi, bj);
			ArbiterKey key(bi, bj);

			if (newArb.numContacts > 0) {
				auto iter = arbiters.find(key);
				if (iter == arbiters.end()) {
					arbiters.insert(ArbPair(key, newArb));
				} else {
					iter->second.Update(newArb.contacts, newArb.numContacts);
				}
			} else {
				arbiters.erase(key);
			}
		}
	}
}

std::shared_ptr<World> World::getInstance() {
	static std::shared_ptr<World> ret(new World());
	return ret;
}
