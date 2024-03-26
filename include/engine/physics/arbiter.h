//
// Created by 韩昱川 on 3/24/24.
//

#ifndef DUODYNO_ARBITER_H
#define DUODYNO_ARBITER_H

#include <simd/simd.h>
#include <memory>

#include "engine/common/math_util.h"

struct Body;

union FeaturePair
{
	struct Edges
	{
		char inEdge1;
		char outEdge1;
		char inEdge2;
		char outEdge2;
	} e;
	int value;
};

struct Contact
{
	Contact() : Pn(0.0f), Pt(0.0f), Pnb(0.0f) {}

	Vec2 position;
	Vec2 normal;
	Vec2 r1, r2;
	float separation;
	float Pn;	// accumulated normal impulse
	float Pt;	// accumulated tangent impulse
	float Pnb;	// accumulated normal impulse for position bias
	float massNormal, massTangent;
	float bias;
	FeaturePair feature;
};

struct ArbiterKey
{
	ArbiterKey(std::shared_ptr<Body> b1, std::shared_ptr<Body> b2)
	{
		if (b1 < b2)
		{
			body1 = b1; body2 = b2;
		}
		else
		{
			body1 = b2; body2 = b1;
		}
	}

	std::shared_ptr<Body> body1;
	std::shared_ptr<Body> body2;
};

struct Arbiter
{
	enum {MAX_POINTS = 2};

	Arbiter(std::shared_ptr<Body> &b1, std::shared_ptr<Body> &b2);

	void Update(Contact* contacts, int numContacts);

	void PreStep(float inv_dt);
	void ApplyImpulse();

	Contact contacts[MAX_POINTS];
	int numContacts;

	std::shared_ptr<Body> body1;
	std::shared_ptr<Body> body2;

	// Combined friction
	float friction;
};

// This is used by std::set
inline bool operator < (const ArbiterKey& a1, const ArbiterKey& a2)
{
	if (a1.body1 < a2.body1)
		return true;

	if (a1.body1 == a2.body1 && a1.body2 < a2.body2)
		return true;

	return false;
}

int Collide(Contact* contacts, const std::shared_ptr<Body>& body1, const std::shared_ptr<Body>& body2);

#endif //DUODYNO_ARBITER_H
