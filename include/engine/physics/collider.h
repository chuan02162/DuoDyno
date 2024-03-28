//
// Created by 韩昱川 on 3/24/24.
//

#ifndef DUODYNO_COLLIDER_H
#define DUODYNO_COLLIDER_H

#include <memory>
#include <vector>

#include "engine/common/math_util.h"
#include "engine/physics/arbiter.h"
#include <iostream>
using namespace std;

enum Axis {
	FACE_A_X,
	FACE_A_Y,
	FACE_B_X,
	FACE_B_Y
};

enum EdgeNumbers {
	NO_EDGE = 0,
	EDGE1,
	EDGE2,
	EDGE3,
	EDGE4
};

struct ClipVertex {
	ClipVertex() { fp.value = 0; }

	Vec2 v;
	FeaturePair fp;
};

void Flip(FeaturePair &fp);
int ClipSegmentToLine(ClipVertex vOut[2], ClipVertex vIn[2], const Vec2 &normal, float offset, char clipEdge);
static void ComputeIncidentEdge(ClipVertex c[2], const Vec2 &h, const Vec2 &pos, const Mat22 &Rot, const Vec2 &normal);
class Collider {
public:
	explicit Collider(Vec2 width);

	virtual bool broadPhase(const unique_ptr<Collider> &x);

	virtual void narrowPhase(vector<shared_ptr<Contact>> &ret, const unique_ptr<Collider> &x);

	Vec2 scale, position;
	float rotation;
};

class SquareCollider : public Collider {

};

#endif //DUODYNO_COLLIDER_H
