//
// Created by 韩昱川 on 2/27/24.
//
#include <simd/simd.h>
#include <iostream>
#include <thread>

#include "controller/controller.h"
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
	world->step(0.05);
	for (int i = 0; i < world->bodies.size(); ++i) {
		auto &body = world->bodies[i];
		auto shape=make_shared<SquareAdapter>(body);
		if (i >= _pBodyCount) {
			auto task = poolIns->acquireTask();
			if (task != nullptr) {
				task->init(shape);
				queueIns->addTask(task);
			}
			_pBodyCount += 1;
		} else {
			auto task = poolIns->acquireTask();
			if (task != nullptr) {
				task->init(shape->transform, TaskType::Update, i);
				queueIns->addTask(task);
			}
		}
	}
}
