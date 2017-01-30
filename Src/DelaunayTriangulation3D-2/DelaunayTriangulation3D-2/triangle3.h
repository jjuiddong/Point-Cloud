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

		bool Intersect(const Vector3& vOrig, const Vector3& vDir,
			float* pfT = NULL, float* pfU = NULL, float* pfV = NULL) const;

		bool Projection(const cTriangle &target);

		bool operator==(const cTriangle &rhs) const;


	public:
		vector<Vector3> *m_vertices;// reference
		Vector3 m_normal;
		Vector3 m_center;
		int m_indices[3];
	};
}
