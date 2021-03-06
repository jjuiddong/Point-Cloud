
#include "stdafx.h"
#include "Delaunay3D.h"

using namespace delaunay3d;


void cDelaunay3D::Triangulate(std::vector<Vector3> &vertices)
{
	// Store the vertices localy
	m_vertices = vertices;

	// Determinate the super Tetrahedron
	float minX = vertices[0].x;
	float minY = vertices[0].y;
	float minZ = vertices[0].z;
	float maxX = minX;
	float maxY = minY;
	float maxZ = minZ;

	for (std::size_t i = 0; i < vertices.size(); ++i)
	{
		if (vertices[i].x < minX) minX = vertices[i].x;
		if (vertices[i].y < minY) minY = vertices[i].y;
		if (vertices[i].z < minZ) minZ = vertices[i].z;
		if (vertices[i].x > maxX) maxX = vertices[i].x;
		if (vertices[i].y > maxY) maxY = vertices[i].y;
		if (vertices[i].z > maxZ) maxZ = vertices[i].z;
	}

	const float dx = maxX - minX;
	const float dy = maxY - minY;
	const float dz = maxZ - minZ;
	const float deltaMax = max(dx, max(dy,dz));
	const float midx = (minX + maxX) / 2.f;
	const float midy = (minY + maxY) / 2.f;
	const float midz = (minZ + maxZ) / 2.f;

	const Vector3 p1(midx - 20 * deltaMax, midy - 20 * deltaMax, midz - 20 * deltaMax);
	const Vector3 p2(midx, midy + 20 * deltaMax, midz - 20 * deltaMax);
	const Vector3 p3(midx + 20 * deltaMax, midy - 20 * deltaMax, midz - 20 * deltaMax);
	const Vector3 p4(midx, midy, midz + 20 * deltaMax);

	m_tetrahedrones.clear();
	m_tetrahedrones.push_back(cTetrahedron(p1, p2, p3, p4));

	std::vector<cTriangle> allPolygon;
	
	for (auto &p : vertices)
	{
		std::vector<cTriangle> polygon;

		for (int i = (int)m_tetrahedrones.size()-1; i>=0; --i)
		{
			if (m_tetrahedrones[i].IsContain(p))
			{
				for (int k = 0; k < 4; ++k)
				{
					// 중복이 아닐때 만, 추가
					if (polygon.end() == find(polygon.begin(), polygon.end(), m_tetrahedrones[i].m_tr[k]))
						polygon.push_back(m_tetrahedrones[i].m_tr[k]);
					//if (allPolygon.end() == find(allPolygon.begin(), allPolygon.end(), m_tetrahedrones[i].m_tr[k]))
					//{
					//	allPolygon.push_back(m_tetrahedrones[i].m_tr[k]);
					//	polygon.push_back(m_tetrahedrones[i].m_tr[k]);
					//}
				}

				common::rotatepopvector(m_tetrahedrones, i);
			}
		}

		for (auto &tr : polygon)
		{
			m_tetrahedrones.push_back(
				cTetrahedron(tr.m_p1, tr.m_p2, tr.m_p3, p));
		}
	}

	for (int i = (int)m_tetrahedrones.size() - 1; i >= 0; --i)
	{
		if (m_tetrahedrones[i].IsContainVertex(p1)
			|| m_tetrahedrones[i].IsContainVertex(p2)
			|| m_tetrahedrones[i].IsContainVertex(p3)
			|| m_tetrahedrones[i].IsContainVertex(p4))
		{
			common::rotatepopvector(m_tetrahedrones, i);
		}
	}
}

