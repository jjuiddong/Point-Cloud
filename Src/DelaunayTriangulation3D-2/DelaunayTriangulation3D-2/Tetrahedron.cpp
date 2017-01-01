
#include "stdafx.h"
#include "Tetrahedron.h"
#include "CircumCenter.h"

using namespace delaunay3d;

cTetrahedron::cTetrahedron()
{
}

cTetrahedron::cTetrahedron(vector<Vector3> *vertices,
	const int idx0, const int idx1, const int idx2, const int idx3)
{
	Create(vertices, idx0, idx1, idx2, idx3);
}


bool cTetrahedron::IsContain(const Vector3 &p)
{
	const float len = (m_circumCenter - p).Length();
	return m_radius > len;
}


bool cTetrahedron::IsContainVertex(const Vector3 &p)
{
	return ((*m_vertices)[m_indices[0]] == p) || 
		((*m_vertices)[m_indices[1]] == p) || 
		((*m_vertices)[m_indices[2]] == p) || 
		((*m_vertices)[m_indices[3]] == p);
}


void cTetrahedron::Create(vector<Vector3> *vertices,
	const int idx0, const int idx1, const int idx2, const int idx3)
{
	m_vertices = vertices;
	m_indices[0] = idx0;
	m_indices[1] = idx1;
	m_indices[2] = idx2;
	m_indices[3] = idx3;
	Vector3 p1 = (*vertices)[idx0];
	Vector3 p2 = (*vertices)[idx1];
	Vector3 p3 = (*vertices)[idx2];
	Vector3 p4 = (*vertices)[idx3];

	const Vector3 center = (p1 + p2 + p3 + p4) / 4.f;

	{
		const Vector3 c = (p1 + p2 + p3) / 3.f;
		const Vector3 front = (c - center).Normal() * 10.f + c;
		m_tr[0].Create(vertices, idx0, idx1, idx2, front);
	}

	{
		const Vector3 c = (p2 + p3 + p4) / 3.f;
		const Vector3 front = (c - center).Normal() * 10.f + c;
		m_tr[1].Create(vertices, idx1, idx2, idx3, front);
	}

	{
		const Vector3 c = (p3 + p4 + p1) / 3.f;
		const Vector3 front = (c - center).Normal() * 10.f + c;
		m_tr[2].Create(vertices, idx2, idx3, idx0, front);
	}

	{
		const Vector3 c = (p4 + p1 + p2) / 3.f;
		const Vector3 front = (c - center).Normal() * 10.f + c;
		m_tr[3].Create(vertices, idx3, idx0, idx1, front);
	}

	Vector3 extra;
	TetCircumCenter((float*)&p1, (float*)&p2, (float*)&p3, (float*)&p4,
		(float*)&m_circumCenter, (float*)&extra);

	m_radius = (m_circumCenter - p1).Length();
}


bool cTetrahedron::operator==(const cTetrahedron &rhs) const
{
	if (this != &rhs)
	{
		return ((m_tr[0] == rhs.m_tr[0]) || (m_tr[0] == rhs.m_tr[1]) || (m_tr[0] == rhs.m_tr[2]) || (m_tr[0] == rhs.m_tr[3])) &&
			((m_tr[1] == rhs.m_tr[0]) || (m_tr[1] == rhs.m_tr[1]) || (m_tr[1] == rhs.m_tr[2]) || (m_tr[1] == rhs.m_tr[3])) &&
			((m_tr[2] == rhs.m_tr[0]) || (m_tr[2] == rhs.m_tr[1]) || (m_tr[2] == rhs.m_tr[2]) || (m_tr[2] == rhs.m_tr[3])) &&
			((m_tr[3] == rhs.m_tr[0]) || (m_tr[3] == rhs.m_tr[1]) || (m_tr[3] == rhs.m_tr[2]) || (m_tr[3] == rhs.m_tr[3]));
	}
	return true;
}

