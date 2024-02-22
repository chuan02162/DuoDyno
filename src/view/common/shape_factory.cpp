//
// Created by 韩昱川 on 2/21/24.
//
#include "view/common/shape_factory.h"
#include <string>

Shape *ShapeFactory::createShape(const std::string &name) {
	if(name=="Square"){
		printf("created\n");
		return new Square();
	}
	else{
		return new Square();
	}
}

ShapeFactory &ShapeFactory::getInstance() {
	static ShapeFactory instance;
	return instance;
}
