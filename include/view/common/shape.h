//
// Created by 韩昱川 on 2/19/24.
//

#ifndef DUODYNO_SHAPE_H
#define DUODYNO_SHAPE_H

#include<simd/simd.h>
#include<iostream>
#include<cstring>
#include<vector>

struct ShapeData{
	std::shared_ptr<std::vector<simd::float3>> _pVerts;
	std::shared_ptr<std::vector<uint16_t>> _pIndices;
	ShapeData(simd::float3 verts[], uint16_t indices[], size_t vertsSize, size_t indicesSize);
};

class Shape {
public:
	std::shared_ptr<simd::float4x4> transform;
};

class Square : public Shape {
public:
	Square();
	static std::shared_ptr<ShapeData> getData();
};

#endif //DUODYNO_SHAPE_H
