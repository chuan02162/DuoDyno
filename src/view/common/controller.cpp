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
	world->step(0.05);
	for (int i = 0; i < world->bodies.size(); ++i) {
		auto &body = world->bodies[i];
		auto shape= getShape("Square");
		shape->init(body);
		auto task = poolIns->acquireTask();
		if (i >= _pBodyCount) {
			if (task != nullptr) {
				task->init(shape);
				_pBodyCount += 1;
			}
		} else {
			if (task != nullptr) {
				task->init(shape->transform, TaskType::Update, i);
			}
		}
		queueIns->addTask(task);
	}
}
