
#include "stdafx.h"
#include "triangle3.h"


using namespace delaunay3d;

cTriangle::cTriangle()
{
}

cTriangle::cTriangle(vector<Vector3> *vertices,
	const int idx0, const int idx1, const int idx2, const Vector3 &frontPoint)
{
	Create(vertices, idx0, idx1, idx2, frontPoint);
}


void cTriangle::Create(vector<Vector3> *vertices,
	const int idx0, const int idx1, const int idx2, const Vector3 &frontPoint)
{
	m_vertices = vertices;
	const Vector3 p1 = (*vertices)[idx0];
	const Vector3 p2 = (*vertices)[idx1];
	const Vector3 p3 = (*vertices)[idx2];

	const Vector3 center = (p1 + p2 + p3) / 3.f;
	const Vector3 a = (p1 - center).Normal();
	const Vector3 b = (p2 - center).Normal();
	const Vector3 c = (p3 - center).Normal();

	const Vector3 v = (frontPoint - center).Normal();
	if (a.CrossProduct(b).Normal().DotProduct(v) > 0)
	{
		m_indices[0] = idx0;
		m_indices[1] = idx1;
		m_indices[2] = idx2;
		m_normal = a.CrossProduct(b).Normal();
	}
	else
	{
		m_indices[0] = idx0;
		m_indices[1] = idx2;
		m_indices[2] = idx1;
		m_normal = -a.CrossProduct(b).Normal();
	}
}


bool cTriangle::operator==(const cTriangle &rhs) const
{
	if (this != &rhs)
	{
		return	((*m_vertices)[ m_indices[0]] == (*rhs.m_vertices)[ rhs.m_indices[0]] || 
				(*m_vertices)[m_indices[0]] == (*rhs.m_vertices)[ rhs.m_indices[1]] || (*m_vertices)[m_indices[0]] == (*rhs.m_vertices)[ rhs.m_indices[2]]) &&
			((*m_vertices)[m_indices[1]] == (*rhs.m_vertices)[ rhs.m_indices[0]] ||
				(*m_vertices)[m_indices[1]] == (*rhs.m_vertices)[ rhs.m_indices[1]] || (*m_vertices)[m_indices[1]] == (*rhs.m_vertices)[ rhs.m_indices[2]]) &&
			((*m_vertices)[m_indices[2]] == (*rhs.m_vertices)[ rhs.m_indices[0]] || 
				(*m_vertices)[m_indices[2]] == (*rhs.m_vertices)[ rhs.m_indices[1]] || (*m_vertices)[m_indices[2]] == (*rhs.m_vertices)[ rhs.m_indices[2]]);
	}
	return true;
}
