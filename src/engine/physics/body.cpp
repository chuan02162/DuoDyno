//
// Created by 韩昱川 on 3/24/24.
//
#include "engine/physics/body.h"
#include<iostream>


void Body::addForce(simd::float2 force) {
	force = force;
}

void Body::set(simd::float2 position) {
	position = position;
}

void Body::log() {
	std::cout << position.y << std::endl;
}

Body::Body() {
	auto width = collider->width;
	I = mass * (width.x * width.x + width.y * width.y) / 12.0;
	invI = 1.f / I;
}

