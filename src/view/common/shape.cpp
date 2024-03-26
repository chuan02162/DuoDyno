//
// Created by 韩昱川 on 2/19/24.
//

#include "view/common/shape.h"

#include <memory>

Square::Square() : Shape() {
}

std::shared_ptr<ShapeData> Square::getData() {
	static float s = 0.5f;
	static size_t vertsSize = 4;
	static size_t indicesSize = 6;
	static simd::float3 verts[] = {{-s, -s, +s},
								   {+s, -s, +s},
								   {+s, +s, +s},
								   {-s, +s, +s}};
	static uint16_t indices[] = {
			0, 1, 2,
			2, 3, 0,
	};
	static std::shared_ptr<ShapeData> ret(new ShapeData(verts, indices, vertsSize, indicesSize));
	return ret;
}

ShapeData::ShapeData(simd::float3 verts[], uint16_t indices[], size_t vertsSize, size_t indicesSize) {
	_pVerts= std::make_shared<std::vector<simd::float3>>();
	_pIndices=std::make_shared<std::vector<uint16_t>>();
	for(int i=0;i<vertsSize;++i){
		_pVerts->push_back(verts[i]);
	}
	for(int i=0;i<indicesSize;++i){
		_pIndices->push_back(indices[i]);
	}
}
