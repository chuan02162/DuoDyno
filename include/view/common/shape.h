//
// Created by 韩昱川 on 2/19/24.
//

#ifndef DUODYNO_SHAPE_H
#define DUODYNO_SHAPE_H

#include<simd/simd.h>
#include<cstring>

class Shape {
public:

	static void writeVertsData(void *dst);

	static void writeIndicesData(void *dst);

	static void setShape(simd::float3 *verts, const uint16_t *indices, size_t vertsSize, size_t indicesSize);

	static size_t getVertsSize();

	static size_t getIndicesSize();

	simd::float4x4 *Transform;

	static simd::float3 *_pVerts;
	static uint16_t *_pIndices;
	static size_t _pVertsSize;
	static size_t _pIndicesSize;
};

class Square : public Shape {
public:
	Square();
};

#endif //DUODYNO_SHAPE_H
