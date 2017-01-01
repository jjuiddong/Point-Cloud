//
// 2017-01-01, jjuiddong
//
//
#pragma once


namespace delaunay3d
{

	class cTriangle
	{
	public:
		cTriangle();
		cTriangle(vector<Vector3> *vertices,
			const int idx0, const int idx1, const int idx2, const Vector3 &frontPoint);

		void Create(vector<Vector3> *vertices,
			const int idx0, const int idx1, const int idx2, const Vector3 &frontPoint);

		bool operator==(const cTriangle &rhs) const;
		
		vector<Vector3> *m_vertices;// reference
		Vector3 m_normal;
		int m_indices[3];
	};

}
