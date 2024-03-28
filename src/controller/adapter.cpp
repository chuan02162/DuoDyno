//
// Created by 韩昱川 on 3/28/24.
//

#include "controller/adapter.h"

SquareAdapter::SquareAdapter(std::shared_ptr<Body> &body) {
	using simd::float4x4, simd::float4;
	_body = body;
	auto world = World::getInstance();
	auto pos = body->position;
	auto scale = body->collider->scale;
	pos.x /= world->scale.x;
	pos.y /= world->scale.y;
	scale.x /= world->scale.x * 2;
	scale.y /= world->scale.y * 2;
	auto theta = body->rotation;
	auto zoom = float4x4{
			(float4) {scale.x, 0, 0.f, 0.f},
			(float4) {0, scale.y, 0.f, 0.f},
			(float4) {0.f, 0.f, 1, 0.f},
			(float4) {0, 0, 0.f, 1.f}
	};
	auto displacement = float4x4{
			(float4) {1, 0, 0.f, 0.f},
			(float4) {0, 1, 0.f, 0.f},
			(float4) {0.f, 0.f, 1, 0.f},
			(float4) {pos.x, pos.y, 0.f, 1.f}
	};
	auto rotation = float4x4{
			(float4) {cos(theta), sin(theta), 0.f, 0.f},
			(float4) {-sin(theta), cos(theta), 0.f, 0.f},
			(float4) {0, 0, 0, 0},
			(float4) {0, 0, 0, 1}
	};
	transform = make_shared<float4x4>(displacement * rotation * zoom);
}
//[0.0353694111,-0.0353412591,0,0],[0.0706825182,0.0707388222,0,0],[0,0,0,0],[0,0,0,1];