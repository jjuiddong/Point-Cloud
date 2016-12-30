
#include "stdafx.h"
#include "Delaunay3D.h"

using namespace delaunay3d;


void cDelaunay3D::Triangulate(std::vector<Vector3> &vertices)
{
	// Store the vertices localy
	_vertices = vertices;

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

	float dx = maxX - minX;
	float dy = maxY - minY;
	float dz = maxZ - minZ;
	float deltaMax = max(dx, max(dy,dz));
	float midx = (minX + maxX) / 2.f;
	float midy = (minY + maxY) / 2.f;
	float midz = (minZ + maxZ) / 2.f;

	Vector3 p1(midx - 20 * deltaMax, midy - 20 * deltaMax, midz - 20 * deltaMax);
	Vector3 p2(midx, midy + 20 * deltaMax, midz - 20 * deltaMax);
	Vector3 p3(midx + 20 * deltaMax, midy - 20 * deltaMax, midz - 20 * deltaMax);
	Vector3 p4(midx, midy, midz + 20 * deltaMax);

	_tetrahedrones.clear();
	_tetrahedrones.push_back(cTetrahedron(p1, p2, p3, p4));
	
	for (auto &p : vertices)
	{
		std::vector<cTriangle> polygon;

		for (int i = (int)_tetrahedrones.size()-1; i>=0; --i)
		{
			if (_tetrahedrones[i].IsContain(p))
			{
				for (int k = 0; k < 4; ++k)
				{
					// 중복이 아닐때 만, 추가
					if (polygon.end() == find(polygon.begin(), polygon.end(), _tetrahedrones[i].m_tr[k]))
						polygon.push_back(_tetrahedrones[i].m_tr[k]);
				}

				common::rotatepopvector(_tetrahedrones, i);
			}
		}

		for (auto &tr : polygon)
		{
			_tetrahedrones.push_back(
				cTetrahedron(tr.m_p1, tr.m_p2, tr.m_p3, p));
		}
	}

	for (int i = (int)_tetrahedrones.size() - 1; i >= 0; --i)
	{
		if (_tetrahedrones[i].IsContainVertex(p1)
			|| _tetrahedrones[i].IsContainVertex(p2)
			|| _tetrahedrones[i].IsContainVertex(p3)
			|| _tetrahedrones[i].IsContainVertex(p4))
		{
			common::rotatepopvector(_tetrahedrones, i);
		}
	}
}

