
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
	m_idxBuffer.Create(renderer, 4);

	// Create Terahedron Polygon
	if (graphic::sVertexNormDiffuse *p = (graphic::sVertexNormDiffuse*)m_vtxBuffer.Lock())
	{
		// Initialize Vertex Buffer
		for (int i = 0; i < 4; ++i) // Tetrahedron Triangle
		{
			for (int k = 0; k < 3; ++k) // Triangle Vertex
			{
				p->p = (*tet.m_vertices)[ tet.m_tr[i].m_indices[k]];
				p->c = D3DXCOLOR(1, 1, 1, 0);
				p++->n = tet.m_tr[i].m_normal;
			}
		}
		m_vtxBuffer.Unlock();

		// Initialize Index Buffer
		if (unsigned short *ip = (unsigned short*)m_idxBuffer.Lock())
		{
			for (int i = 0; i < 12; ++i) // Tetrahedron Triangle Vertex
				*ip++ = i;
			m_idxBuffer.Unlock();
		}
	}

	// To Display Triangle Normal Vector
	m_normalBuffer.Create(renderer, 8, sizeof(graphic::sVertexDiffuse), graphic::sVertexDiffuse::FVF);
	if (graphic::sVertexDiffuse *p = (graphic::sVertexDiffuse*)m_normalBuffer.Lock())
	{
		for (int i = 0; i < 4; ++i)
		{
			const Vector3 center = 
				((*tet.m_vertices)[tet.m_tr[i].m_indices[0]]
				+ (*tet.m_vertices)[tet.m_tr[i].m_indices[1]]
				+ (*tet.m_vertices)[tet.m_tr[i].m_indices[2]]) / 3.f;
			const Vector3 p1 = tet.m_tr[i].m_normal*1 + center;

			p->p = center;
			p++->c = D3DXCOLOR(0, 1, 0, 0);
			p->p = p1;
			p++->c = D3DXCOLOR(0, 1, 0, 0);
		}

		m_normalBuffer.Unlock();
	}

	// To Display Triangle Edge
	m_edgeBuffer.Create(renderer, 24, sizeof(graphic::sVertexDiffuse), graphic::sVertexDiffuse::FVF);
	if (graphic::sVertexDiffuse *p = (graphic::sVertexDiffuse*)m_edgeBuffer.Lock())
	{
		for (int i = 0; i < 4; ++i)
		{
			p->p = (*tet.m_vertices)[tet.m_tr[i].m_indices[0]];
			p++->c = D3DXCOLOR(1, 1, 1, 0);
			p->p = (*tet.m_vertices)[tet.m_tr[i].m_indices[1]];
			p++->c = D3DXCOLOR(1, 1, 1, 0);

			p->p = (*tet.m_vertices)[tet.m_tr[i].m_indices[1]];
			p++->c = D3DXCOLOR(1, 1, 1, 0);
			p->p = (*tet.m_vertices)[tet.m_tr[i].m_indices[2]];
			p++->c = D3DXCOLOR(1, 1, 1, 0);

			p->p = (*tet.m_vertices)[tet.m_tr[i].m_indices[2]];
			p++->c = D3DXCOLOR(1, 1, 1, 0);
			p->p = (*tet.m_vertices)[tet.m_tr[i].m_indices[0]];
			p++->c = D3DXCOLOR(1, 1, 1, 0);
		}

		m_edgeBuffer.Unlock();
	}

	return true;
}


void cRenderTetrahedron::Render(graphic::cRenderer &renderer, const Matrix44 &tm,
	const bool renderNorm, const bool renderEdge)
{
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&tm);
	m_vtxBuffer.Bind(renderer);
	m_idxBuffer.Bind(renderer);
	renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		m_vtxBuffer.GetVertexCount(), 0, m_idxBuffer.GetFaceCount());

	if (renderNorm)
		m_normalBuffer.RenderLineList2(renderer);

	if (renderEdge)
		m_edgeBuffer.RenderLineList2(renderer);
}
