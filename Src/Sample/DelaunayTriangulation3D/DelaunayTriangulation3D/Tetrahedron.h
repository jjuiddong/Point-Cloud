//
// 2016-12-27, jjuiddong
// Tetrahedron (사면체) 을 표현하는 클래스
//
#pragma once

#include "triangle3.h"

namespace delaunay3d
{

	class cTetrahedron
	{
	public:
		cTetrahedron();
		cTetrahedron(const Vector3 &p1,
			const Vector3 &p2,
			const Vector3 &p3,
			const Vector3 &p4);

		void Create(const Vector3 &p1,
			const Vector3 &p2,
			const Vector3 &p3,
			const Vector3 &p4);

		bool IsContain(const Vector3 &p);
		bool IsContainVertex(const Vector3 &p);

		bool operator==(const cTetrahedron &rhs) const;


	public:
		cTriangle m_tr[4];
		Vector3 m_p[4];
		Vector3 m_circumCenter;
		float m_radius;
	};

}
