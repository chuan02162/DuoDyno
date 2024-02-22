//
// Created by 韩昱川 on 2/19/24.
//

#include "view/common/shape.h"

simd::float3 *Shape::_pVerts = nullptr;
uint16_t *Shape::_pIndices = nullptr;
size_t Shape::_pVertsSize = 0;
size_t Shape::_pIndicesSize = 0;


void Shape::writeVertsData(void *dst) {
	memcpy(dst, _pVerts, getVertsSize());
}

void Shape::writeIndicesData(void *dst) {
	memcpy(dst, _pIndices, getIndicesSize());
}

void Shape::setShape(simd::float3 *verts, const uint16_t *indices, size_t vertsSize, size_t indicesSize) {
	_pVerts = new simd::float3[vertsSize];
	_pIndices = new uint16_t[indicesSize];
	for (int i = 0; i < vertsSize; ++i)
		_pVerts[i] = verts[i];
	for (int i = 0; i < indicesSize; ++i)
		_pIndices[i] = indices[i];
	_pVertsSize = vertsSize;
	_pIndicesSize = indicesSize;
}

Square::Square() : Shape() {
	static float s = 0.5f;
	static simd::float3 verts[] = {
			{-s, -s, +s},
			{+s, -s, +s},
			{+s, +s, +s},
			{-s, +s, +s}
	};
	static uint16_t indices[] = {
			0, 1, 2,
			2, 3, 0,
	};
	setShape(verts, indices, 4, 6);
}

size_t Shape::getVertsSize() {
	return sizeof(simd::float3) * _pVertsSize;
}
size_t Shape::getIndicesSize() {
	return sizeof(uint16_t) * _pIndicesSize;
}