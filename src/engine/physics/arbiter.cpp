//
// Created by 韩昱川 on 3/25/24.
//

#include "engine/physics/arbiter.h"
#include "engine/physics/body.h"
#include "engine/physics/world.h"
#include<iostream>
using namespace std;

Arbiter::Arbiter(shared_ptr<Body> b1, shared_ptr<Body> b2) {
	if (b1 < b2)
	{
		body1 = std::move(b1);
		body2 = std::move(b2);
	}
	else
	{
		body1 = std::move(b2);
		body2 = std::move(b1);
	}
	auto &c1 = body1->collider;
	auto &c2 = body2->collider;
	if (c1->broadPhase(c2)) {
		c1->narrowPhase(contacts, c2);
	}

	friction = sqrtf(body1->friction * body2->friction);
}

void Arbiter::Update(vector<shared_ptr<Contact>> &newContacts) {
	vector<shared_ptr<Contact>> mergedContacts;
	mergedContacts.resize(newContacts.size());

	for (int i=0;i<newContacts.size();++i) {
		auto &cNew=newContacts[i];
		int k = -1;
		for (int j = 0; j < contacts.size(); ++j) {
			auto &cOld = contacts[j];
			if (cNew->feature.value == cOld->feature.value) {
				k = j;
				break;
			}
		}

		auto &c = mergedContacts[i];
		if (k > -1) {
			auto &cOld = contacts[k];
			c=cNew;
			if (World::warmStarting) {
				c->Pn = cOld->Pn;
				c->Pt = cOld->Pt;
				c->Pnb = cOld->Pnb;
			} else {
				c->Pn = 0.0f;
				c->Pt = 0.0f;
				c->Pnb = 0.0f;
			}
		} else {
			c=newContacts[i];
		}
	}
	contacts.resize(newContacts.size());
	for (int i = 0; i < newContacts.size(); ++i){
		contacts[i]=mergedContacts[i];
		assert(mergedContacts[i]!= nullptr);
	}
}


void Arbiter::PreStep(float inv_dt) {
	const float k_allowedPenetration = 0.01f;
	float k_biasFactor = World::positionCorrection ? 0.2f : 0.0f;
	auto numContacts=contacts.size();

	for (int i = 0; i < numContacts; ++i) {
		auto &c = contacts[i];

		Vec2 r1 = c->position - body1->position;
		Vec2 r2 = c->position - body2->position;

		// Precompute normal mass, tangent mass, and bias.
		float rn1 = Dot(r1, c->normal);
		float rn2 = Dot(r2, c->normal);
		float kNormal = body1->_invMass + body2->_invMass;
		kNormal += body1->invI * (Dot(r1, r1) - rn1 * rn1) + body2->invI * (Dot(r2, r2) - rn2 * rn2);
		c->massNormal = 1.0f / kNormal;

		Vec2 tangent = Cross(c->normal, 1.0f);
		float rt1 = Dot(r1, tangent);
		float rt2 = Dot(r2, tangent);
		float kTangent = body1->_invMass + body2->_invMass;
		kTangent += body1->invI * (Dot(r1, r1) - rt1 * rt1) + body2->invI * (Dot(r2, r2) - rt2 * rt2);
		c->massTangent = 1.0f / kTangent;

		c->bias = -k_biasFactor * inv_dt * Min(0.0f, c->separation + k_allowedPenetration);

		if (World::accumulateImpulses) {
			// Apply normal + friction impulse
			Vec2 P = c->Pn * c->normal + c->Pt * tangent;

			body1->_velocity -= body1->_invMass * P;
			body1->_angularVelocity -= body1->invI * Cross(r1, P);

			body2->_velocity += body2->_invMass * P;
			body2->_angularVelocity += body2->invI * Cross(r2, P);
		}
	}
}

void Arbiter::ApplyImpulse() {
	auto b1 = body1;
	auto b2 = body2;
	auto numContacts=contacts.size();
	for (int i = 0; i < numContacts; ++i) {
		auto c = contacts[i];
		c->r1 = c->position - b1->position;
		c->r2 = c->position - b2->position;

		// Relative velocity at contact
		Vec2 dv = b2->_velocity + Cross(b2->_angularVelocity, c->r2) - b1->_velocity - Cross(b1->_angularVelocity, c->r1);

		// Compute normal impulse
		float vn = Dot(dv, c->normal);

		float dPn = c->massNormal * (-vn + c->bias);

		if (World::accumulateImpulses) {
			// Clamp the accumulated impulse
			float Pn0 = c->Pn;
			c->Pn = Max(Pn0 + dPn, 0.0f);
			dPn = c->Pn - Pn0;
		} else {
			dPn = Max(dPn, 0.0f);
		}

		// Apply contact impulse
		Vec2 Pn = dPn * c->normal;

		b1->_velocity -= b1->_invMass * Pn;
		b1->_angularVelocity -= b1->invI * Cross(c->r1, Pn);

		b2->_velocity += b2->_invMass * Pn;
		b2->_angularVelocity += b2->invI * Cross(c->r2, Pn);

		// Relative velocity at contact
		dv = b2->_velocity + Cross(b2->_angularVelocity, c->r2) - b1->_velocity - Cross(b1->_angularVelocity, c->r1);

		Vec2 tangent = Cross(c->normal, 1.0f);
		float vt = Dot(dv, tangent);
		float dPt = c->massTangent * (-vt);

		if (World::accumulateImpulses) {
			// Compute friction impulse
			float maxPt = friction * c->Pn;

			// Clamp friction
			float oldTangentImpulse = c->Pt;
			c->Pt = Clamp(oldTangentImpulse + dPt, -maxPt, maxPt);
			dPt = c->Pt - oldTangentImpulse;
		} else {
			float maxPt = friction * dPn;
			dPt = Clamp(dPt, -maxPt, maxPt);
		}

		// Apply contact impulse
		Vec2 Pt = dPt * tangent;

		b1->_velocity -= b1->_invMass * Pt;
		b1->_angularVelocity -= b1->invI * Cross(c->r1, Pt);

		b2->_velocity += b2->_invMass * Pt;
		b2->_angularVelocity += b2->invI * Cross(c->r2, Pt);
	}
}

bool Arbiter::operator<(const Arbiter &x) const {
	return body1 < x.body1 or body2 < x.body2;
}
