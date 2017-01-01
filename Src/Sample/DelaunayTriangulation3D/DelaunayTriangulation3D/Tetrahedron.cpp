
#include "stdafx.h"
#include "Tetrahedron.h"
#include "CircumCenter.h"

using namespace delaunay3d;

cTetrahedron::cTetrahedron()
{
}

cTetrahedron::cTetrahedron(const Vector3 &p1,
	const Vector3 &p2,
	const Vector3 &p3,
	const Vector3 &p4)
{
	Create(p1, p2, p3, p4);
}


bool cTetrahedron::IsContain(const Vector3 &p)
{
	const float len = (m_circumCenter - p).Length();
	return m_radius > len;
}


bool cTetrahedron::IsContainVertex(const Vector3 &p)
{
	return (m_p[0] == p) || (m_p[1] == p) || (m_p[2] == p) || (m_p[3] == p);
}


void cTetrahedron::Create(const Vector3 &p1,
	const Vector3 &p2,
	const Vector3 &p3,
	const Vector3 &p4)
{
	const Vector3 center = (p1 + p2 + p3 + p4) / 4.f;

	{
		const Vector3 c = (p1 + p2 + p3) / 3.f;
		const Vector3 front = (c - center).Normal() * 10.f + c;
		m_tr[0].Create(p1, p2, p3, front);
	}

	{
		const Vector3 c = (p2 + p3 + p4) / 3.f;
		const Vector3 front = (c - center).Normal() * 10.f + c;
		m_tr[1].Create(p2, p3, p4, front);
	}

	{
		const Vector3 c = (p3 + p4 + p1) / 3.f;
		const Vector3 front = (c - center).Normal() * 10.f + c;
		m_tr[2].Create(p3, p4, p1, front);
	}

	{
		const Vector3 c = (p4 + p1 + p2) / 3.f;
		const Vector3 front = (c - center).Normal() * 10.f + c;
		m_tr[3].Create(p4, p1, p2, front);
	}

	m_p[0] = p1;
	m_p[1] = p2;
	m_p[2] = p3;
	m_p[3] = p4;

	Vector3 extra;
	TetCircumCenter((float*)&p1, (float*)&p2, (float*)&p3, (float*)&p4,
		(float*)&m_circumCenter, (float*)&extra);

	m_radius = (m_circumCenter - p1).Length();
}


bool cTetrahedron::operator==(const cTetrahedron &rhs) const
{
	if (this != &rhs)
	{
		return m_p[0] == rhs.m_p[0];
	}
	return true;
}

