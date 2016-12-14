
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "DelaunayApp.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int range = 1000;
#define colVertex	(RGB(255, 128, 0))


inline int Int(REAL r) { return (int)floor(r + 0.5f); }	// properly round of a REAL

														// Function object to draw a vertex; to be used in for_each algorithm.
class drawVertex
{
public:
	drawVertex(CDC& dc, REAL hScale, REAL vScale) : m_DC(dc), m_hScale(hScale), m_vScale(vScale) {}
	void operator()(const vertex& v) const
	{
		const int halfSize = 2;

		CRect rc;
		rc.SetRectEmpty();
		rc.InflateRect(halfSize, halfSize);
		rc.OffsetRect(Int(m_hScale * v.GetX()), Int(m_vScale * v.GetY()));
		m_DC.FillSolidRect(&rc, colVertex);
	}
protected:
	CDC& m_DC;
	REAL m_hScale;
	REAL m_vScale;
};

// Function object to draw an edge.
class drawEdge
{
public:
	drawEdge(CDC& dc, REAL hScale, REAL vScale) : m_DC(dc), m_hScale(hScale), m_vScale(vScale) {}
	void operator()(const edge& e) const
	{
		m_DC.MoveTo(Int(m_hScale * e.m_pV0->GetX()), Int(m_vScale * e.m_pV0->GetY()));
		m_DC.LineTo(Int(m_hScale * e.m_pV1->GetX()), Int(m_vScale * e.m_pV1->GetY()));
	}
protected:
	CDC& m_DC;
	REAL m_hScale;
	REAL m_vScale;
};

// Function object to draw a triangle.
class drawTriangle
{
public:
	drawTriangle(CDC& dc, REAL hScale, REAL vScale) : m_DC(dc), m_hScale(hScale), m_vScale(vScale) {}
	void operator()(const triangle& tri) const
	{
		const vertex * v0 = tri.GetVertex(0);
		m_DC.MoveTo(Int(m_hScale * v0->GetX()), Int(m_vScale * v0->GetY()));
		const vertex * v1 = tri.GetVertex(1);
		m_DC.LineTo(Int(m_hScale * v1->GetX()), Int(m_vScale * v1->GetY()));
		const vertex * v2 = tri.GetVertex(2);
		m_DC.LineTo(Int(m_hScale * v2->GetX()), Int(m_vScale * v2->GetY()));
		m_DC.LineTo(Int(m_hScale * v0->GetX()), Int(m_vScale * v0->GetY()));
	}
protected:
	CDC& m_DC;
	REAL m_hScale;
	REAL m_vScale;
};



// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
}

