//
// Created by 韩昱川 on 3/25/24.
//
#include "engine/physics/collider.h"

Collider::Collider(Vec2 width) : scale(width) {

}

void Flip(FeaturePair &fp) {
	Swap(fp.e.inEdge1, fp.e.inEdge2);
	Swap(fp.e.outEdge1, fp.e.outEdge2);
}

int ClipSegmentToLine(ClipVertex vOut[2], ClipVertex vIn[2], const Vec2 &normal, float offset, char clipEdge) {
	int numOut = 0;

	// Calculate the distance of end points to the line
	float distance0 = Dot(normal, vIn[0].v) - offset;
	float distance1 = Dot(normal, vIn[1].v) - offset;

	// If the points are behind the plane
	if (distance0 <= 0.0f) vOut[numOut++] = vIn[0];
	if (distance1 <= 0.0f) vOut[numOut++] = vIn[1];

	// If the points are on different sides of the plane
	if (distance0 * distance1 < 0.0f) {
		// Find intersection point of edge and plane
		float interp = distance0 / (distance0 - distance1);
		vOut[numOut].v = vIn[0].v + interp * (vIn[1].v - vIn[0].v);
		if (distance0 > 0.0f) {
			vOut[numOut].fp = vIn[0].fp;
			vOut[numOut].fp.e.inEdge1 = clipEdge;
			vOut[numOut].fp.e.inEdge2 = NO_EDGE;
		} else {
			vOut[numOut].fp = vIn[1].fp;
			vOut[numOut].fp.e.outEdge1 = clipEdge;
			vOut[numOut].fp.e.outEdge2 = NO_EDGE;
		}
		++numOut;
	}

	return numOut;
}

static void ComputeIncidentEdge(ClipVertex c[2], const Vec2 &h, const Vec2 &pos, const Mat22 &Rot, const Vec2 &normal) {
	// The normal is from the reference box. Convert it
	Mat22 RotT = Rot.Transpose();
	Vec2 n = -(RotT * normal);
	Vec2 nAbs = Abs(n);

	if (nAbs.x > nAbs.y) {
		if (Sign(n.x) > 0.0f) {
			c[0].v.Set(h.x, -h.y);
			c[0].fp.e.inEdge2 = EDGE3;
			c[0].fp.e.outEdge2 = EDGE4;

			c[1].v.Set(h.x, h.y);
			c[1].fp.e.inEdge2 = EDGE4;
			c[1].fp.e.outEdge2 = EDGE1;
		} else {
			c[0].v.Set(-h.x, h.y);
			c[0].fp.e.inEdge2 = EDGE1;
			c[0].fp.e.outEdge2 = EDGE2;

			c[1].v.Set(-h.x, -h.y);
			c[1].fp.e.inEdge2 = EDGE2;
			c[1].fp.e.outEdge2 = EDGE3;
		}
	} else {
		if (Sign(n.y) > 0.0f) {
			c[0].v.Set(h.x, h.y);
			c[0].fp.e.inEdge2 = EDGE4;
			c[0].fp.e.outEdge2 = EDGE1;

			c[1].v.Set(-h.x, h.y);
			c[1].fp.e.inEdge2 = EDGE1;
			c[1].fp.e.outEdge2 = EDGE2;
		} else {
			c[0].v.Set(-h.x, -h.y);
			c[0].fp.e.inEdge2 = EDGE2;
			c[0].fp.e.outEdge2 = EDGE3;

			c[1].v.Set(h.x, -h.y);
			c[1].fp.e.inEdge2 = EDGE3;
			c[1].fp.e.outEdge2 = EDGE4;
		}
	}

	c[0].v = pos + Rot * c[0].v;
	c[1].v = pos + Rot * c[1].v;
}

bool Collider::broadPhase(const std::unique_ptr<Collider> &x) {
	return true;
}

void Collider::narrowPhase(vector<shared_ptr<Contact>> &ret, const unique_ptr<Collider> &x) {
	ret.clear();
	auto CA = this;
	auto &CB = x;

	// Setup
	Vec2 hA = 0.5 * scale;
	Vec2 hB = 0.5 * CB->scale;

	Vec2 posA = CA->position;
	Vec2 posB = CB->position;

	Mat22 RotA(CA->rotation), RotB(CB->rotation);

	Mat22 RotAT = RotA.Transpose();
	Mat22 RotBT = RotB.Transpose();

	Vec2 dp = posB - posA;
	Vec2 dA = RotAT * dp;
	Vec2 dB = RotBT * dp;

	Mat22 C = RotAT * RotB;
	Mat22 absC = Abs(C);
	Mat22 absCT = absC.Transpose();

	// Box A faces
	Vec2 faceA = Abs(dA) - hA - absC * hB;
	if (faceA.x > 0.0f || faceA.y > 0.0f)
		return;

	// Box B faces
	Vec2 faceB = Abs(dB) - absCT * hA - hB;
	if (faceB.x > 0.0f || faceB.y > 0.0f)
		return;

	// Find best axis
	Axis axis;
	float separation;
	Vec2 normal;

	// Box A faces
	axis = FACE_A_X;
	separation = faceA.x;
	normal = dA.x > 0.0f ? RotA.col1 : -RotA.col1;

	const float relativeTol = 0.95f;
	const float absoluteTol = 0.01f;

	if (faceA.y > relativeTol * separation + absoluteTol * hA.y) {
		axis = FACE_A_Y;
		separation = faceA.y;
		normal = dA.y > 0.0f ? RotA.col2 : -RotA.col2;
	}

	// Box B faces
	if (faceB.x > relativeTol * separation + absoluteTol * hB.x) {
		axis = FACE_B_X;
		separation = faceB.x;
		normal = dB.x > 0.0f ? RotB.col1 : -RotB.col1;
	}

	if (faceB.y > relativeTol * separation + absoluteTol * hB.y) {
		axis = FACE_B_Y;
		separation = faceB.y;
		normal = dB.y > 0.0f ? RotB.col2 : -RotB.col2;
	}

	// Setup clipping plane data based on the separating axis
	Vec2 frontNormal, sideNormal;
	ClipVertex incidentEdge[2];
	float front, negSide, posSide;
	char negEdge, posEdge;

	// Compute the clipping lines and the line segment to be clipped.
	switch (axis) {
		case FACE_A_X: {
			frontNormal = normal;
			front = Dot(posA, frontNormal) + hA.x;
			sideNormal = RotA.col2;
			float side = Dot(posA, sideNormal);
			negSide = -side + hA.y;
			posSide = side + hA.y;
			negEdge = EDGE3;
			posEdge = EDGE1;
			ComputeIncidentEdge(incidentEdge, hB, posB, RotB, frontNormal);
		}
			break;

		case FACE_A_Y: {
			frontNormal = normal;
			front = Dot(posA, frontNormal) + hA.y;
			sideNormal = RotA.col1;
			float side = Dot(posA, sideNormal);
			negSide = -side + hA.x;
			posSide = side + hA.x;
			negEdge = EDGE2;
			posEdge = EDGE4;
			ComputeIncidentEdge(incidentEdge, hB, posB, RotB, frontNormal);
		}
			break;

		case FACE_B_X: {
			frontNormal = -normal;
			front = Dot(posB, frontNormal) + hB.x;
			sideNormal = RotB.col2;
			float side = Dot(posB, sideNormal);
			negSide = -side + hB.y;
			posSide = side + hB.y;
			negEdge = EDGE3;
			posEdge = EDGE1;
			ComputeIncidentEdge(incidentEdge, hA, posA, RotA, frontNormal);
		}
			break;

		case FACE_B_Y: {
			frontNormal = -normal;
			front = Dot(posB, frontNormal) + hB.y;
			sideNormal = RotB.col1;
			float side = Dot(posB, sideNormal);
			negSide = -side + hB.x;
			posSide = side + hB.x;
			negEdge = EDGE2;
			posEdge = EDGE4;
			ComputeIncidentEdge(incidentEdge, hA, posA, RotA, frontNormal);
		}
			break;
	}

	// clip other face with 5 box planes (1 face plane, 4 edge planes)

	ClipVertex clipPoints1[2];
	ClipVertex clipPoints2[2];
	int np;

	// Clip to box side 1
	np = ClipSegmentToLine(clipPoints1, incidentEdge, -sideNormal, negSide, negEdge);

	if (np < 2)
		return;

	// Clip to negative box side 1
	np = ClipSegmentToLine(clipPoints2, clipPoints1, sideNormal, posSide, posEdge);

	if (np < 2)
		return;
	int numContacts = 0;
	for (auto &point: clipPoints2) {
		separation = Dot(frontNormal, point.v) - front;
		if (separation <= 0) {
			ret.push_back(std::make_shared<Contact>());
			ret[numContacts]->separation = separation;
			ret[numContacts]->normal = normal;
			ret[numContacts]->position = point.v - separation * frontNormal;
			ret[numContacts]->feature = point.fp;
			if (axis == FACE_B_X || axis == FACE_B_Y)
				Flip(ret[numContacts]->feature);
			++numContacts;
		}
	}
}
