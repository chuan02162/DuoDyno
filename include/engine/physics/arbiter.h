//
// Created by 韩昱川 on 3/24/24.
//

#ifndef DUODYNO_ARBITER_H
#define DUODYNO_ARBITER_H

#include <simd/simd.h>
#include <memory>
#include <vector>
#include <utility>

#include "engine/common/math_util.h"

using namespace std;

struct Body;

union FeaturePair {
	struct Edges {
		char inEdge1;
		char outEdge1;
		char inEdge2;
		char outEdge2;
	} e;
	int value;
};

struct Contact {
	Contact() : Pn(0.0f), Pt(0.0f), Pnb(0.0f) {}

	Vec2 position;
	Vec2 normal;
	Vec2 r1, r2;
	float separation;
	float Pn;    // accumulated normal impulse
	float Pt;    // accumulated tangent impulse
	float Pnb;    // accumulated normal impulse for position bias
	float massNormal, massTangent;
	float bias;
	FeaturePair feature;
};

struct ArbiterKey {
	ArbiterKey(const shared_ptr<Body> b1, const shared_ptr<Body> b2) {
		if (b1 < b2) {
			body1 = b1;
			body2 = b2;
		} else {
			body1 = b2;
			body2 = b1;
		}
	}

	shared_ptr<Body> body1;
	shared_ptr<Body> body2;
};

struct Arbiter {
	Arbiter(shared_ptr<Body> b1, shared_ptr<Body> b2);

	void Update(vector<shared_ptr<Contact>> &newContacts);

	void PreStep(float inv_dt);

	void ApplyImpulse();

	vector<shared_ptr<Contact>> contacts;

	shared_ptr<Body> body1;
	shared_ptr<Body> body2;

	float friction;

	bool operator<(const Arbiter &x) const;
};

// This is used by set
inline bool operator<(const ArbiterKey &a1, const ArbiterKey &a2) {
	if (a1.body1 < a2.body1)
		return true;

	if (a1.body1 == a2.body1 && a1.body2 < a2.body2)
		return true;

	return false;
}


#endif //DUODYNO_ARBITER_H
