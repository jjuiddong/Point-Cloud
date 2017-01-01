//
// 2016-12-27, jjuiddong
//
//
#pragma once


namespace delaunay3d
{

	class cTriangle
	{
	public:
		cTriangle();
		cTriangle(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, const Vector3 &frontPoint);

		void Create(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, const Vector3 &frontPoint);

		bool operator==(const cTriangle &rhs) const;
		
		Vector3 m_p1, m_p2, m_p3;
		Vector3 m_normal;
	};

}
