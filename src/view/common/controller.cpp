//
// Created by 韩昱川 on 2/27/24.
//
#include <simd/simd.h>
#include <iostream>
#include <thread>

#include "view/controller.h"
#include "view/common/shape_factory.h"
#include "view/task_queue.h"
#include "engine/physics/world.h"

Controller *Controller::getInstance() {
	static auto *ins = new Controller();
	return ins;
}

void Controller::beforeDraw() {
	using simd::float4x4;
	using simd::float4;
	auto queueIns = TaskQueue::getInstance();
	auto poolIns = TaskPool::getInstance();
	auto world = World::getInstance();
	auto sf = ShapeFactory::getInstance();
	world->step(0.01);
	for (int i = 0; i < world->bodies.size(); ++i) {
		auto &body = world->bodies[i];
//		body->log();
		auto x_offset = body->position.x;
		auto y_offset = body->position.y;
		if (i >= _pBodyCount) {
			auto sq = sf.createShape("Square");
			sq->transform = std::make_shared<float4x4>(
					(float4) {0, 0.1, 0.f, 0.f},
					(float4) {0.1, 0, 0.f, 0.f},
					(float4) {0.f, 0.f, 0.1, 0.f},
					(float4) {x_offset, y_offset, 0.f, 1.f});
			auto task = poolIns->acquireTask();
			if (task != nullptr) {
				task->init(sq);
				queueIns->addTask(task);
			}
			_pBodyCount += 1;
		} else {
			auto trans = std::make_shared<float4x4>(
					float4x4{(float4) {0, 0.1, 0.f, 0.f},
							 (float4) {0.1, 0, 0.f, 0.f},
							 (float4) {0.f, 0.f, 0.1, 0.f},
							 (float4) {x_offset, y_offset, 0.f, 1.f}});
			auto theta = -body->rotation;
			std::cout<<theta<<std::endl;
			auto rotation = std::make_shared<float4x4>(
					(float4) {cos(theta), sin(theta), 0.f, 0.f},
					(float4) {-sin(theta), cos(theta), 0.f, 0.f},
					(float4) {0,0,0,0},
					(float4) {0,0,0,1}
			);
			*(trans)*=*rotation;
			auto task = poolIns->acquireTask();
			if (task != nullptr) {
				task->init(trans, TaskType::Update, i);
				queueIns->addTask(task);
			}
		}
	}
}
