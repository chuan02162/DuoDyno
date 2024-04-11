//
// Created by 韩昱川 on 3/28/24.
//

#include "controller/adapter.h"

class ShapeFactory {
public:
	std::shared_ptr<Adapter> createShape(const std::string &name) {
		if (name == "Square") {
			return std::make_shared<SquareAdapter>();
		} else {
			return std::make_shared<SquareAdapter>();
		}
	}
};

std::shared_ptr<Adapter> getShape(const std::string &name) {
	static ShapeFactory shapeFactory;
	return shapeFactory.createShape(name);
}


void SquareAdapter::init(std::shared_ptr<Body> body) {
	using simd::float4x4, simd::float4;
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
