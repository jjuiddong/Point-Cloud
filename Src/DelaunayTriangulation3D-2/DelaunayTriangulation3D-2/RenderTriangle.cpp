
#include "stdafx.h"
#include "RenderTriangle.h"

using namespace graphic;
using namespace delaunay3d;


cRenderTriangle::cRenderTriangle()
{
}

cRenderTriangle::~cRenderTriangle()
{
}


bool cRenderTriangle::Init(graphic::cRenderer &renderer, const cTriangle &tri)
{
	m_vtxBuffer.Create(renderer, 3, sizeof(sVertexNormDiffuse), sVertexNormDiffuse::FVF);
	if (sVertexNormDiffuse *p = (sVertexNormDiffuse*)m_vtxBuffer.Lock())
	{
		for (int i = 0; i < 3; ++i)
		{
			p->p = (*tri.m_vertices)[tri.m_indices[i]];
			p->n = tri.m_normal;
			p->c = D3DXCOLOR(1, 1, 1, 0);
			++p;
		}
		m_vtxBuffer.Unlock();
	}

	return true;
}


void cRenderTriangle::Render(cRenderer &renderer, const Matrix44 &tm)
{
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&tm);

	m_vtxBuffer.Bind(renderer);
	m_vtxBuffer.RenderTriangleList(renderer);
}
