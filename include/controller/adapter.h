//
// Created by 韩昱川 on 3/28/24.
//

#ifndef DUODYNO_ADAPTER_H
#define DUODYNO_ADAPTER_H

#include "engine/physics/body.h"
#include "view/common/shape.h"
#include "engine/physics/world.h"
#include <simd/simd.h>

class SquareAdapter : public Square{
public:
	SquareAdapter(std::shared_ptr<Body> &body);
private:
	std::shared_ptr<Body> _body;
};

#endif //DUODYNO_ADAPTER_H
