//
// Created by 韩昱川 on 2/25/24.
//

#ifndef DUODYNO_TASK_QUEUE_H
#define DUODYNO_TASK_QUEUE_H

#include <queue>
#include <mutex>

#include "view/common/shape.h"

enum class TaskType {
	Add,
	Update
};

struct Task {
public:

	void init(std::shared_ptr<void> data = nullptr, TaskType pTT = TaskType::Add, int pIndex = 0);

	std::shared_ptr<void> data;
	TaskType tt;
	int index;
};

class TaskQueue {
public:
	static TaskQueue *getInstance();

	void addTask(Task *task);

	bool empty();

	Task *top();

	void pop();

	std::mutex mtx;
private:
	std::queue<Task *> _pQueue;
};

class TaskPool {
public:
	explicit TaskPool(int capacity);

	~TaskPool();

	static TaskPool *getInstance();

	Task *acquireTask();

	void releaseTask(Task *task);

private:
	std::vector<Task *> tasks;
	size_t _pSize = 100;
};

#endif //DUODYNO_TASK_QUEUE_H

