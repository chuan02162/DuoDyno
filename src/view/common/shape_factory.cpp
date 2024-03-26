//
// Created by 韩昱川 on 2/21/24.
//
#include "view/common/shape_factory.h"
#include <string>

std::shared_ptr<Shape> ShapeFactory::createShape(const std::string &name) {
	if(name=="Square"){
		printf("created\n");
		return std::make_shared<Square>();
	}
	else{
		return std::make_shared<Square>();
	}
}

ShapeFactory &ShapeFactory::getInstance() {
	static ShapeFactory instance;
	return instance;
}
