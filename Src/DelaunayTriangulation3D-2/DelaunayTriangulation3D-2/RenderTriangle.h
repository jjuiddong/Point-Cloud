#pragma once

#include "triangle3.h"

namespace delaunay3d
{

	class cRenderTriangle
	{
	public:
		cRenderTriangle();
		virtual ~cRenderTriangle();

		bool Init(graphic::cRenderer &renderer, const cTriangle &tri);
		void Render(graphic::cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity);


	public:
		graphic::cVertexBuffer m_vtxBuffer;
	};


}
