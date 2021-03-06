
#include "stdafx.h"
#include "cube.h"

using namespace graphic;


cCube::cCube()
{
}

cCube::cCube(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax)
{
	InitCube(renderer);
	SetCube(renderer, vMin, vMax);
}


void cCube::InitCube(cRenderer &renderer)
{
	if (m_vtxBuff.GetVertexCount() > 0)
		return;

	//        4 --- 5
	//      / |  |  /|
	//   0 --- 1   |
	//   |   6-|- -7
	//   | /     | /
	//   2 --- 3
	//
	Vector3 vertices[8] = {
		Vector3(-1,1,-1), Vector3(1,1,-1), Vector3(-1,-1,-1), Vector3(1,-1,-1),
		Vector3(-1,1, 1), Vector3(1,1, 1), Vector3(-1,-1,1), Vector3(1,-1,1),
	};
	WORD indices[36] = {
		// front
		0, 3, 2,
		0 ,1, 3,
		// back
		5, 6, 7,
		5, 4, 6,
		// top
		4, 1, 0,
		4, 5, 1,
		// bottom
		2, 7, 6,
		2, 3, 7,
		// left
		4, 2, 6,
		4, 0, 2,
		// right
		1, 7, 3,
		1, 5, 7,
	};

	m_vtxBuff.Create(renderer, 8, sizeof(sVertexDiffuse), sVertexDiffuse::FVF);
	m_idxBuff.Create(renderer, 12);

	sVertexDiffuse *vbuff = (sVertexDiffuse*)m_vtxBuff.Lock();
	WORD *ibuff = (WORD*)m_idxBuff.Lock();

	for (int i=0; i < 8; ++i)
	{
		vbuff[ i].p = vertices[ i];
		vbuff[ i].c = 0;
	}

	for (int i=0; i < 36; ++i)
		ibuff[ i] = indices[ i];

	m_vtxBuff.Unlock();
	m_idxBuff.Unlock();
}


void cCube::SetCube(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax)
{
	if (m_vtxBuff.GetVertexCount() <= 0)
		InitCube(renderer);

	//        4 --- 5
	//      / |  |  /|
	//   0 --- 1   |
	//   |   6-|- -7
	//   | /     | /
	//   2 --- 3
	//
	// min = index 2
	// max = index 5

	Vector3 vertices[8] = {
		Vector3(vMin.x, vMax.y, vMin.z), 
		Vector3(vMax.x, vMax.y, vMin.z), 
		Vector3(vMin.x, vMin.y, vMin.z), 
		Vector3(vMax.x, vMin.y, vMin.z),
		Vector3(vMin.x, vMax.y, vMax.z), 
		Vector3(vMax.x, vMax.y, vMax.z),
		Vector3(vMin.x, vMin.y, vMax.z), 
		Vector3(vMax.x, vMin.y, vMax.z),
	};

	sVertexDiffuse *vbuff = (sVertexDiffuse*)m_vtxBuff.Lock();
	for (int i=0; i < 8; ++i)
		vbuff[ i].p = vertices[ i];
	m_vtxBuff.Unlock();

	m_min = vMin;
	m_max = vMax;
}


void cCube::SetCube(cRenderer &renderer, const cCube &cube)
{
	SetCube(renderer, cube.GetMin(), cube.GetMax());
	m_tm = cube.GetTransform();
}


void cCube::SetColor( DWORD color )
{
	sVertexDiffuse *vbuff = (sVertexDiffuse*)m_vtxBuff.Lock();
	for (int i=0; i < m_vtxBuff.GetVertexCount(); ++i)
	{
		vbuff[ i].c = color;
	}
	m_vtxBuff.Unlock();
}


void cCube::Render(cRenderer &renderer, const Matrix44 &tm)
{
	RET(m_vtxBuff.GetVertexCount() <= 0);

	DWORD cullMode;
	DWORD fillMode;
	DWORD lightMode;
	renderer.GetDevice()->GetRenderState(D3DRS_CULLMODE, &cullMode);
	renderer.GetDevice()->GetRenderState(D3DRS_FILLMODE, &fillMode);
	renderer.GetDevice()->GetRenderState(D3DRS_LIGHTING, &lightMode);

	renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, FALSE);
	renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	renderer.GetDevice()->SetTexture(0, NULL);
	
	Matrix44 mat = m_tm * tm;
	renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&mat);
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);
	renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		m_vtxBuff.GetVertexCount(), 0, 12);

	renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, cullMode);
	renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, fillMode);
	renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, lightMode);
}
