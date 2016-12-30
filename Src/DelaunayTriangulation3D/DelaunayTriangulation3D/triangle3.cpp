
#include "stdafx.h"
#include "triangle3.h"


using namespace delaunay3d;

cTriangle::cTriangle()
{
}


cTriangle::cTriangle(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3)
	: m_p1(p1), m_p2(p2), m_p3(p3)
{
}


cTriangle::cTriangle(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3,
	const Vector3 &frontPoint)
{
	Create(p1, p2, p3, frontPoint);
}


void cTriangle::Create(const Vector3 &p1, const Vector3 &p2,
	const Vector3 &p3, const Vector3 &frontPoint)
{
	const Vector3 center = (p1 + p2 + p3) / 3.f;
	const Vector3 a = (p1 - center).Normal();
	const Vector3 b = (p2 - center).Normal();
	const Vector3 c = (p3 - center).Normal();

	const Vector3 v = (frontPoint - center).Normal();
	if (a.CrossProduct(b).Normal().DotProduct(v) > 0)
	{
		m_p1 = p1;
		m_p2 = p2;
		m_p3 = p3;
		m_normal = a.CrossProduct(b).Normal();
		m_plane.Init(p1, p2, p3);
	}
	else
	{
		m_p1 = p1;
		m_p2 = p3;
		m_p3 = p2;
		m_normal = -a.CrossProduct(b).Normal();
		m_plane.Init(p1, p3, p2);
	}
}


bool cTriangle::operator==(const cTriangle &rhs) const
{
	if (this != &rhs)
	{
		return	(m_p1 == rhs.m_p1 || m_p1 == rhs.m_p2 || m_p1 == rhs.m_p3) &&
			(m_p2 == rhs.m_p1 || m_p2 == rhs.m_p2 || m_p2 == rhs.m_p3) &&
			(m_p3 == rhs.m_p1 || m_p3 == rhs.m_p2 || m_p3 == rhs.m_p3);
	}
	return true;
}
