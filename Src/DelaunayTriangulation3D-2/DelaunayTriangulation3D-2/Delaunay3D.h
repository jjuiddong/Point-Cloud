#pragma once

#include "triangle3.h"
#include "Tetrahedron.h"

namespace delaunay3d
{

	class cDelaunay3D
	{
	public:
		void Triangulate(std::vector<Vector3> &vertices);
		void OptimizeTriangle();
		

	public:
		std::vector<Vector3> m_vertices;
		std::vector<cTetrahedron> m_tetrahedrones;
		std::vector<cTriangle> m_tri;
	};

}