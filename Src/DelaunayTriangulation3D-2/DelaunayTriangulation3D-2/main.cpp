//
// 2017-01-01, jjuiddong
// Delaunay Triangulation 3D, Indexed Vertext Buffer
//

#include "stdafx.h"
#include "Delaunay3D.h"
#include "RenderTetrahedron.h"
#include "RenderTriangle.h"

using namespace graphic;


class cViewer : public framework::cGameMain
{
public:
	cViewer();
	virtual ~cViewer();

	virtual bool OnInit() override;
	virtual void OnUpdate(const float elapseT) override;
	virtual void OnRender(const float elapseT) override;
	virtual void OnShutdown() override;
	virtual void OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam) override;


private:
	void UpdateDelaunayTriangles();


	vector<Vector3> m_points;
	delaunay3d::cDelaunay3D m_delaunay;
	vector<delaunay3d::cRenderTetrahedron*> m_renderTetrahedrones;
	vector<delaunay3d::cRenderTriangle*> m_renderTriangle;
	
	delaunay3d::cTetrahedron m_tetra;
	delaunay3d::cRenderTetrahedron m_renderTetra;
	
	graphic::cVertexBuffer m_pointVtxBuff;

	// point cloud mesh
	graphic::cVertexBuffer m_cloudVtxBuff;
	graphic::cIndexBuffer m_cloudIdxBuff;

	// optimize point cloud mesh (remove internal triangle)
	graphic::cVertexBuffer m_optVtxBuff;
	graphic::cIndexBuffer m_optIdxBuff;


	bool m_isRenderVertex;
	bool m_isRenderTerahedron;
	bool m_isRenderCloudMesh;
	bool m_isRenderNormal;
	bool m_isRenderEdge;
	bool m_isRenderOptimize;
	bool m_isRenderOptimizeTriangle;
	int m_renderIdx;

	POINT m_curPos;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;

	Matrix44 m_rot;
};

INIT_FRAMEWORK(cViewer);

const int WINSIZE_X = 800;
const int WINSIZE_Y = 600;

cViewer::cViewer()
	: m_isRenderVertex(false)
	, m_isRenderTerahedron(false)
	, m_isRenderCloudMesh(false)
	, m_isRenderNormal(false)
	, m_isRenderEdge(true)
	, m_isRenderOptimize(false)
	, m_isRenderOptimizeTriangle(false)
	, m_renderIdx(0)
{
	m_windowName = L"Delaunay Triangulations 3D Indexed";
	const RECT r = { 0, 0, WINSIZE_X, WINSIZE_Y };
	m_windowRect = r;
	m_LButtonDown = false;
	m_RButtonDown = false;
	m_MButtonDown = false;
}

cViewer::~cViewer()
{
	for (auto &p : m_renderTetrahedrones)
		delete p;
	m_renderTetrahedrones.clear();

	for (auto &p : m_renderTriangle)
		delete p;
	m_renderTriangle.clear();


	graphic::ReleaseRenderer();
}

float RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

bool cViewer::OnInit()
{
	GetMainCamera()->Init(&m_renderer);
	GetMainCamera()->SetCamera(Vector3(30, 30, -30), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GetMainCamera()->SetProjection(D3DX_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 1.f, 10000.0f);

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL);
	GetMainLight().SetPosition(Vector3(5, 5, 5));
	GetMainLight().SetDirection(Vector3(1, -1, 1).Normal());
	GetMainLight().Bind(m_renderer, 0);

	m_renderer.GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	m_renderer.GetDevice()->LightEnable(0, true);

	UpdateDelaunayTriangles();

	return true;
}


void cViewer::UpdateDelaunayTriangles()
{
	//srand((unsigned int)time(NULL));
	const int numberPoints = (int)roundf(RandomFloat(8, 8));

	m_points.clear();
	for (int i = 0; i < numberPoints; i++)
		m_points.push_back(Vector3(RandomFloat(-10, 10), RandomFloat(-10, 10), RandomFloat(-10, 10)));

	m_delaunay.Triangulate(m_points);
	m_delaunay.OptimizeTriangle();

	// Vertex List
	m_pointVtxBuff.Create(m_renderer, numberPoints, sizeof(graphic::sVertexPoint), graphic::sVertexPoint::FVF);
	if (graphic::sVertexPoint *p = (graphic::sVertexPoint*)m_pointVtxBuff.Lock())
	{
		for (auto pt : m_points)
		{
			p->p = pt;
			p->c = D3DXCOLOR(1.0f, 0, 0, 0);
			p->size = 1;
			++p;
		}
		m_pointVtxBuff.Unlock();
	}

	// Create Rendering Tetrahedron
	for (auto &p : m_renderTetrahedrones)
		delete p;
	m_renderTetrahedrones.clear();

	for (auto &tet : m_delaunay.m_tetrahedrones)
	{
		delaunay3d::cRenderTetrahedron *renderTet = new delaunay3d::cRenderTetrahedron();
		renderTet->Init(m_renderer, tet);
		m_renderTetrahedrones.push_back(renderTet);
	}

	//---------------------------------------------------------------------------------------------------------
	// Create Point Cloud Mesh
	m_cloudVtxBuff.Create(m_renderer, 
		m_delaunay.m_tetrahedrones.size() * 12,
		sizeof(graphic::sVertexNormDiffuse), graphic::sVertexNormDiffuse::FVF);

	m_cloudIdxBuff.Create(m_renderer, m_delaunay.m_tetrahedrones.size() * 12);

	if (graphic::sVertexNormDiffuse *p = (graphic::sVertexNormDiffuse*)m_cloudVtxBuff.Lock())
	{
		if (unsigned short *ip = (unsigned short*)m_cloudIdxBuff.Lock())
		{
			int idx = 0;
			for (auto &tet : m_delaunay.m_tetrahedrones)
			{
				for (int i = 0; i < 4; ++i) // Tetrahedron Triangle
				{
					for (int k = 0; k < 3; ++k) // Triangle Vertex
					{
						// Initialize Vertex Buffer
						p->p = (*tet.m_vertices)[tet.m_tr[i].m_indices[k]];
						p->c = D3DXCOLOR(1, 1, 1, 0);
						p++->n = tet.m_tr[i].m_normal;

						// Initialize Index Buffer
						*ip++ = idx++;
					}
				}
			}
		}

		m_cloudVtxBuff.Unlock();
		m_cloudIdxBuff.Unlock();
	}


	//---------------------------------------------------------------------------------------------------------
	// Create Optimize Point Cloud Mesh
	m_optVtxBuff.Create(m_renderer,
		m_delaunay.m_tri.size() * 3,
		sizeof(graphic::sVertexNormDiffuse), graphic::sVertexNormDiffuse::FVF);

	m_optIdxBuff.Create(m_renderer, m_delaunay.m_tri.size() * 3);

	if (graphic::sVertexNormDiffuse *p = (graphic::sVertexNormDiffuse*)m_optVtxBuff.Lock())
	{
		if (unsigned short *ip = (unsigned short*)m_optIdxBuff.Lock())
		{
			int idx = 0;
			for (auto &tri : m_delaunay.m_tri)
			{
				for (int k = 0; k < 3; ++k) // Triangle Vertex
				{
					// Initialize Vertex Buffer
					p->p = (*tri.m_vertices)[tri.m_indices[k]];
					p->c = D3DXCOLOR(1, 1, 1, 0);
					p++->n = tri.m_normal;

					// Initialize Index Buffer
					*ip++ = idx++;
				}
			}
		}

		m_optVtxBuff.Unlock();
		m_optIdxBuff.Unlock();
	}

	// Create Rendering Triangle
	for (auto &p : m_renderTriangle)
		delete p;
	m_renderTriangle.clear();

	for (auto &tri : m_delaunay.m_tri)
	{
		delaunay3d::cRenderTriangle *p = new delaunay3d::cRenderTriangle();
		p->Init(m_renderer, tri);
		m_renderTriangle.push_back(p);
	}


	common::dbg::Log("Optimize triangle count = %d\n", m_delaunay.m_tri.size());
	common::dbg::Log("Tetrahedron count = %d\n", m_delaunay.m_tetrahedrones.size());
}


void cViewer::OnUpdate(const float elapseT)
{
	static float incT = 0;
	incT += elapseT;
	if (incT < 0.033f)
		return;

	// keyboard
	const float vel = 200*elapseT;
	if (GetAsyncKeyState('W'))
		GetMainCamera()->MoveFront(vel);
	else if (GetAsyncKeyState('A'))
		GetMainCamera()->MoveRight(-vel);
	else if (GetAsyncKeyState('D'))
		GetMainCamera()->MoveRight(vel);
	else if (GetAsyncKeyState('S'))
		GetMainCamera()->MoveFront(-vel);
	else if (GetAsyncKeyState('E'))
		GetMainCamera()->MoveUp(vel);
	else if (GetAsyncKeyState('C'))
		GetMainCamera()->MoveUp(-vel);

	GetMainCamera()->Update();

	incT = 0;
}


void cViewer::OnRender(const float elapseT)
{
	//화면 청소
	if (SUCCEEDED(m_renderer.GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(150, 150, 150), 1.0f, 0)))
	{
		m_renderer.GetDevice()->BeginScene();

		m_renderer.RenderGrid();
		m_renderer.RenderAxis();

		if (m_isRenderTerahedron)
		{
			if ((int)m_renderTetrahedrones.size() > m_renderIdx)
				m_renderTetrahedrones[m_renderIdx]->Render(m_renderer, m_rot, m_isRenderNormal, m_isRenderEdge);
		}
		else
		{
			if (m_isRenderOptimize)
			{
				m_renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&m_rot);
				m_optVtxBuff.Bind(m_renderer);
				m_optIdxBuff.Bind(m_renderer);
				m_renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
					m_optVtxBuff.GetVertexCount(), 0, m_optIdxBuff.GetFaceCount());
			}
			else if (m_isRenderOptimizeTriangle)
			{
				for (auto tri : m_renderTriangle)
					tri->Render(m_renderer, m_rot);
			}
			else if (m_isRenderCloudMesh)
			{
				m_renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&m_rot);
				m_cloudVtxBuff.Bind(m_renderer);
				m_cloudIdxBuff.Bind(m_renderer);
				m_renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
					m_cloudVtxBuff.GetVertexCount(), 0, m_cloudIdxBuff.GetFaceCount());
			}
			else
			{
				for (auto p : m_renderTetrahedrones)
					p->Render(m_renderer, m_rot, m_isRenderNormal, m_isRenderEdge);
			}
		}

		if (m_isRenderVertex)
			m_pointVtxBuff.RenderPointList2(m_renderer);

		m_renderTetra.Render(m_renderer);
		m_renderer.RenderFPS();

		m_renderer.GetDevice()->EndScene();
		m_renderer.GetDevice()->Present(NULL, NULL, NULL, NULL);
	}
}


void cViewer::OnShutdown()
{
}


void cViewer::OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEWHEEL:
	{
		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		dbg::Print("%d %d", fwKeys, zDelta);

		const float len = graphic::GetMainCamera()->GetDistance();
		float zoomLen = (len > 100) ? 50 : (len / 4.f);
		if (fwKeys & 0x4)
			zoomLen = zoomLen / 10.f;

		graphic::GetMainCamera()->Zoom((zDelta < 0) ? -zoomLen : zoomLen);
	}
	break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_BACK:
			UpdateDelaunayTriangles();
			break;
		case VK_TAB:
		{
			static bool flag = false;
			m_renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, flag ? D3DCULL_CCW : D3DCULL_NONE);
			m_renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, flag ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
			flag = !flag;
		}
		break;

		case '1': m_isRenderVertex = !m_isRenderVertex; break;
		case '2': m_isRenderTerahedron = !m_isRenderTerahedron; break;
		case '3': m_isRenderNormal = !m_isRenderNormal; break;
		case '4': m_isRenderEdge = !m_isRenderEdge; break;
		case '5': m_isRenderCloudMesh = !m_isRenderCloudMesh; break;
		case '6': m_isRenderOptimize = !m_isRenderOptimize; break;
			break;

		case VK_LEFT:
			--m_renderIdx;
			if (0 > m_renderIdx)
				m_renderIdx = (int)m_renderTetrahedrones.size() - 1;
			break;

		case VK_RIGHT:
			++m_renderIdx;
			if ((int)m_renderTetrahedrones.size() <= m_renderIdx)
				m_renderIdx = 0;
			break;
		}
		break;

	case WM_LBUTTONDOWN:
	{
		SetCapture(m_hWnd);
		m_LButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
	}
	break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		m_LButtonDown = false;
		break;

	case WM_RBUTTONDOWN:
	{
		SetCapture(m_hWnd);
		m_RButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
	}
	break;

	case WM_RBUTTONUP:
		ReleaseCapture();
		m_RButtonDown = false;
		break;

	case WM_MBUTTONDOWN:
		m_MButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
		break;

	case WM_MBUTTONUP:
		m_MButtonDown = false;
		break;

	case WM_MOUSEMOVE:
	{
		const POINT point = { LOWORD(lParam), HIWORD(lParam) };

		if (m_LButtonDown)
		{
			const int x = point.x - m_curPos.x;
			const int y = point.y - m_curPos.y;
			m_curPos = point;

			const Quaternion q1(graphic::GetMainCamera()->GetUpVector(), x * -0.005f);
			const Quaternion q2(graphic::GetMainCamera()->GetRight(), y * -0.005f);
			m_rot *= q1.GetMatrix();
			m_rot *= q2.GetMatrix();
		}
		else if (m_RButtonDown)
		{
			const int x = point.x - m_curPos.x;
			const int y = point.y - m_curPos.y;
			m_curPos = point;

			graphic::GetMainCamera()->Yaw2(x * 0.005f);
			graphic::GetMainCamera()->Pitch2(y * 0.005f);
		}
		else if (m_MButtonDown)
		{
			const POINT pos = { point.x - m_curPos.x, point.y - m_curPos.y };
			m_curPos = point;

			const float len = graphic::GetMainCamera()->GetDistance();
			graphic::GetMainCamera()->MoveRight(-pos.x * len * 0.001f);
			graphic::GetMainCamera()->MoveUp(pos.y * len * 0.001f);
		}

	}
	break;
	}
}
