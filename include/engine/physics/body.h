//
// Created by 韩昱川 on 3/24/24.
//

#ifndef DUODYNO_BODY_H
#define DUODYNO_BODY_H
#include<simd/simd.h>
#include<memory>
#include "collider.h"
#include "engine/common/math_util.h"

enum class BodyType{
	Dynamic,
	Static,
	Kinematic
};

class Body{
public:
	Body();
	void setPosition(Vec2 position);
	void setRotation(float rotate);
	void setScale(Vec2 scale) const;
	void setMass(float mass);
	void addVelocity(Vec2 velocity);
	void addAngularVelocity(float angularVelocity);
	void log() const;
	Vec2 force{};
	Vec2 _velocity{};
	Vec2 position{};
	float rotation;
	float _mass=1, _invMass=1;
	float I, invI;
	float _angularVelocity;
	float torque;
	float friction=0.3;
	BodyType type{BodyType::Dynamic};
	std::unique_ptr<Collider> collider=std::make_unique<Collider>(Vec2(10,10));
};

#endif //DUODYNO_BODY_H
