//
// Created by 韩昱川 on 3/28/24.
//

#ifndef DUODYNO_ADAPTER_H
#define DUODYNO_ADAPTER_H

#include "engine/physics/body.h"
#include "view/common/shape.h"
#include "engine/physics/world.h"
#include <simd/simd.h>

class Adapter : public Shape {
public:
	virtual void init(std::shared_ptr<Body> body) = 0;
};

class SquareAdapter : public Adapter {
public:
	void init(std::shared_ptr<Body> body) override;
};

std::shared_ptr<Adapter> getShape(const std::string &name);

#endif //DUODYNO_ADAPTER_H
