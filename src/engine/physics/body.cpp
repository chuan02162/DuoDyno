//
// Created by 韩昱川 on 3/24/24.
//
#include "engine/physics/body.h"
#include<iostream>

void Body::setPosition(Vec2 pos) {
	position = pos;
	collider->position = pos;
}

void Body::setRotation(float rot) {
	rotation = rot;
	collider->rotation = rot;
}

void Body::log() const {
	std::cout << position.y << std::endl;
}

Body::Body() {
	setMass(1);
}

void Body::setMass(float mass) {
	_mass = mass;
	_invMass = 1.f / mass;
	auto width = collider->scale;
	I = mass * (width.x * width.x + width.y * width.y) / 12.0f;
	invI = 1.f / I;
}

void Body::setScale(Vec2 scale) const {
	collider->scale = scale;
}

void Body::addVelocity(Vec2 velocity) {
	if(type==BodyType::Static)
		return;
	_velocity+=velocity;
}

void Body::addAngularVelocity(float angularVelocity){
	if(type==BodyType::Static)
		return;
	_angularVelocity+=angularVelocity;
}