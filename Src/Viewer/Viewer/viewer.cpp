
#include "../../../../Common/Common/common.h"
#include "../../../../Common/Graphic/graphic.h"
#include "../../../../Common/Framework/framework.h"

using namespace common;
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
	bool Read3dFile(const string &fileName);


private:
	vector<graphic::cVertexBuffer*> m_3dDatas;

	POINT m_curPos;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;
	Matrix44 m_rotateTm;

	Vector3 m_boxPos;
};

INIT_FRAMEWORK(cViewer);

const int WINSIZE_X = 800;
const int WINSIZE_Y = 600;

cViewer::cViewer() 
{
	m_windowName = L"Point Cloud";
	const RECT r = { 0, 0, WINSIZE_X, WINSIZE_Y };
	m_windowRect = r;
	m_LButtonDown = false;
	m_RButtonDown = false;
	m_MButtonDown = false;
}

cViewer::~cViewer()
{
	for each (auto &buff in m_3dDatas)
		delete buff;
	m_3dDatas.clear();

	graphic::ReleaseRenderer();
}


bool cViewer::OnInit()
{
	DragAcceptFiles(m_hWnd, TRUE);

	cResourceManager::Get()->SetMediaDirectory("./media/");

	GetMainCamera()->Init(&m_renderer);
	GetMainCamera()->SetCamera(Vector3(10, 10, -10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GetMainCamera()->SetProjection(D3DX_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 1.f, 10000.0f);

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL);
	GetMainLight().SetPosition(Vector3(5, 5, 5));
	GetMainLight().SetDirection(Vector3(1, -1, 1).Normal());
	GetMainLight().Bind(m_renderer, 0);

	m_renderer.GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	m_renderer.GetDevice()->LightEnable(0, true);
	
	//Read3dFile("scan000.3d");
	Read3dFile("scan054.3d");

	return true;
}


bool cViewer::Read3dFile(const string &fileName)
{
	std::ifstream ifs(fileName);
	if (ifs.is_open())
	{
		vector<Vector3> vertices;
		vertices.reserve(10000000);
		string line;
		while (getline(ifs, line))
		{
			stringstream ss(line);
			Vector3 v;
			ss >> v.x >> v.y >> v.z;
			vertices.push_back(v);
			if (vertices.size() > 1000000)
				break;
		}

		graphic::cVertexBuffer *vtxBuffer = new graphic::cVertexBuffer();
		if (vtxBuffer->Create(m_renderer, vertices.size(), sizeof(Vector3), D3DFVF_XYZ))
		{
			if (Vector3 *p = (Vector3*)vtxBuffer->Lock())
			{
				for each (auto &v in vertices)
					*p++ = v;
			}
			vtxBuffer->Unlock();

			m_3dDatas.push_back(vtxBuffer);
		}
	}

	return true;
}


void cViewer::OnUpdate(const float elapseT)
{
	static float incT = 0;
	incT += elapseT;
	if (incT < 0.033f)
		return;

	GetMainCamera()->Update();
	//m_character.Update(incT);

	incT = 0;
}


void cViewer::OnRender(const float elapseT)
{
	//화면 청소
	if (SUCCEEDED(m_renderer.GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(150, 150, 150), 1.0f, 0)))
	{
		m_renderer.GetDevice()->BeginScene();

		m_renderer.GetDevice()->SetRenderState(D3DRS_ZENABLE, 0);
		m_renderer.GetDevice()->SetRenderState(D3DRS_ZENABLE, 1);

		m_renderer.RenderGrid();
		m_renderer.RenderAxis();

		for each (auto &vtxBuffer in m_3dDatas)
			vtxBuffer->RenderPointList(m_renderer);

		//  		GetMainCamera()->SetViewMatrix(m_zealotCameraView);
		//  		m_renderer.GetDevice()->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&m_zealotCameraView);
		//m_character.Render(m_renderer, Matrix44::Identity);

		m_renderer.RenderFPS();

		//랜더링 끝
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
	case WM_DROPFILES:
	{
		HDROP hdrop = (HDROP)wParam;
		char filePath[MAX_PATH];
		const UINT size = DragQueryFileA(hdrop, 0, filePath, sizeof(filePath));
		if (size == 0)
			return;
		Read3dFile(filePath);
	}
	break;

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
			for each (auto &buff in m_3dDatas)
				delete buff;
			m_3dDatas.clear();
			break;
		case VK_TAB:
		{
			static bool flag = false;
			m_renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, flag ? D3DCULL_CCW : D3DCULL_NONE);
			m_renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, flag ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
			flag = !flag;
		}
		break;

		case VK_LEFT: m_boxPos.x -= 10.f; break;
		case VK_RIGHT: m_boxPos.x += 10.f; break;
		case VK_UP: m_boxPos.z += 10.f; break;
		case VK_DOWN: m_boxPos.z -= 10.f; break;
		}
		break;

	case WM_LBUTTONDOWN:
	{
		m_LButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
	}
	break;

	case WM_LBUTTONUP:
		m_LButtonDown = false;
		break;

	case WM_RBUTTONDOWN:
	{
		m_RButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
	}
	break;

	case WM_RBUTTONUP:
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
		if (m_RButtonDown)
		{
			POINT pos = { LOWORD(lParam), HIWORD(lParam) };
			const int x = pos.x - m_curPos.x;
			const int y = pos.y - m_curPos.y;
			m_curPos = pos;

			graphic::GetMainCamera()->Yaw2(x * 0.005f);
			graphic::GetMainCamera()->Pitch2(y * 0.005f);
		}
		else if (m_MButtonDown)
		{
			const POINT point = { LOWORD(lParam), HIWORD(lParam) };
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

