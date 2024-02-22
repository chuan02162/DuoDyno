//
// Created by 韩昱川 on 2/20/24.
//

#ifndef DUODYNO_SHAPE_FACTORY_H
#define DUODYNO_SHAPE_FACTORY_H

#endif //DUODYNO_SHAPE_FACTORY_H

#include "shape.h"

class ShapeFactory {
public:
	static ShapeFactory& getInstance();
	Shape* createShape(const std::string &name);
};
