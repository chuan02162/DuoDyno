//
// Created by 韩昱川 on 3/24/24.
//

#ifndef DUODYNO_BODY_H
#define DUODYNO_BODY_H
#include<simd/simd.h>
#include<memory>
#include "collider.h"
#include "engine/common/math_util.h"

class Body{
public:
	Body();
	void addForce(simd::float2 force);
	void update(float dt);
	void set(simd::float2 position);
	void log();
	Vec2 force{};
	Vec2 velocity{};
	Vec2 position{};
	float rotation;
	float mass=1, invMass=1;
	float I, invI;
	float angularVelocity;
	float torque;
	float friction;
	std::shared_ptr<Collider> collider=std::make_shared<Collider>(Vec2(0.1,0.1));
};

#endif //DUODYNO_BODY_H
