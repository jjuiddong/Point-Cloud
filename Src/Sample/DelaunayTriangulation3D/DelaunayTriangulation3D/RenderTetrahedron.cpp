
#include "stdafx.h"
#include "RenderTetrahedron.h"

using namespace delaunay3d;


cRenderTetrahedron::cRenderTetrahedron()
{
}

cRenderTetrahedron::~cRenderTetrahedron()
{
}


bool cRenderTetrahedron::Init(graphic::cRenderer &renderer, const cTetrahedron &tet)
{
	m_tet = tet;

	m_vtxBuffer.Create(renderer, 12, sizeof(graphic::sVertexNormDiffuse), graphic::sVertexNormDiffuse::FVF);

	// Create Terahedron Polygon
	if (graphic::sVertexNormDiffuse *p = (graphic::sVertexNormDiffuse*)m_vtxBuffer.Lock())
	{
		for (int i = 0; i < 4; ++i)
		{
			p->p = tet.m_tr[i].m_p1;
			p->c = D3DXCOLOR(1, 1, 1, 0);
			p++->n = tet.m_tr[i].m_normal;
			p->p = tet.m_tr[i].m_p2;
			p->c = D3DXCOLOR(1, 1, 1, 0);
			p++->n = tet.m_tr[i].m_normal;
			p->p = tet.m_tr[i].m_p3;
			p->c = D3DXCOLOR(1, 1, 1, 0);
			p++->n = tet.m_tr[i].m_normal;
		}
		m_vtxBuffer.Unlock();
	}

	// To Display Triangle Normal Vector
	m_normalBuffer.Create(renderer, 8, sizeof(graphic::sVertexDiffuse), graphic::sVertexDiffuse::FVF);
	if (graphic::sVertexDiffuse *p = (graphic::sVertexDiffuse*)m_normalBuffer.Lock())
	{
		for (int i = 0; i < 4; ++i)
		{
			const Vector3 center = (tet.m_tr[i].m_p1 + tet.m_tr[i].m_p2 + tet.m_tr[i].m_p3) / 3.f;
			const Vector3 p1 = tet.m_tr[i].m_normal*1 + center;

			p->p = center;
			p++->c = D3DXCOLOR(0, 1, 0, 0);
			p->p = p1;
			p++->c = D3DXCOLOR(0, 1, 0, 0);
		}

		m_normalBuffer.Unlock();
	}

	// To Display Triangle Edge Vector
	m_edgeBuffer.Create(renderer, 24, sizeof(graphic::sVertexDiffuse), graphic::sVertexDiffuse::FVF);
	if (graphic::sVertexDiffuse *p = (graphic::sVertexDiffuse*)m_edgeBuffer.Lock())
	{
		for (int i = 0; i < 4; ++i)
		{
			p->p = tet.m_tr[i].m_p1;
			p++->c = D3DXCOLOR(1, 1, 1, 0);
			p->p = tet.m_tr[i].m_p2;
			p++->c = D3DXCOLOR(1, 1, 1, 0);

			p->p = tet.m_tr[i].m_p2;
			p++->c = D3DXCOLOR(1, 1, 1, 0);
			p->p = tet.m_tr[i].m_p3;
			p++->c = D3DXCOLOR(1, 1, 1, 0);

			p->p = tet.m_tr[i].m_p3;
			p++->c = D3DXCOLOR(1, 1, 1, 0);
			p->p = tet.m_tr[i].m_p1;
			p++->c = D3DXCOLOR(1, 1, 1, 0);
		}

		m_edgeBuffer.Unlock();
	}

	return true;
}


void cRenderTetrahedron::Render(graphic::cRenderer &renderer, const bool renderNorm, const bool renderEdge)
{
	m_vtxBuffer.RenderTriangleList(renderer);

	if (renderNorm)
		m_normalBuffer.RenderLineList2(renderer);

	if (renderEdge)
		m_edgeBuffer.RenderLineList2(renderer);
}
