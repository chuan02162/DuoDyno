//
// Created by 韩昱川 on 2/25/24.
//
#include "view/task_queue.h"

#include <utility>

TaskQueue *TaskQueue::getInstance() {
	static auto *ins = new TaskQueue();
	return ins;
}

void TaskQueue::addTask(Task *task) {
	std::lock_guard<std::mutex> lck(mtx);
	_pQueue.push(task);
}

bool TaskQueue::empty() {
	std::lock_guard<std::mutex> lck(mtx);
	return _pQueue.empty();
}

void TaskQueue::pop() {
	std::lock_guard<std::mutex> lck(mtx);
	_pQueue.pop();
}

Task *TaskQueue::top() {
	std::lock_guard<std::mutex> lck(mtx);
	return _pQueue.front();
}

void Task::init(std::shared_ptr<void> pData, TaskType pTT, int pIndex) {
	data=std::move(pData);
	tt=pTT;
	index=pIndex;
}

TaskPool::TaskPool(int capacity) {
	tasks.reserve(capacity);
	for (int i = 0; i < capacity; ++i) {
		tasks.push_back(new Task());
	}
}

TaskPool::~TaskPool() {
	for (auto task: tasks) {
		delete task;
	}
}

Task *TaskPool::acquireTask() {
	if (!tasks.empty()) {
		Task* task = tasks.back();
		tasks.pop_back();
		return task;
	} else {
		return nullptr;
	}
}

void TaskPool::releaseTask(Task *task)  {
	tasks.push_back(task);
}

TaskPool *TaskPool::getInstance() {
	static auto *ins=new TaskPool(100);
	return ins;
}
