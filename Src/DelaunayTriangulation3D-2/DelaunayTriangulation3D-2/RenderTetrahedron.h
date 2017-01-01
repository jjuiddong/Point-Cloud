#pragma once

#include "Tetrahedron.h"

namespace delaunay3d
{

	class cRenderTetrahedron
	{
	public:
		cRenderTetrahedron();
		virtual ~cRenderTetrahedron();

		bool Init(graphic::cRenderer &renderer, const cTetrahedron &tet);
		void Render(graphic::cRenderer &renderer, const Matrix44 &tm=Matrix44::Identity, 
			const bool renderNorm=false, const bool renderEdge=false);

		cTetrahedron m_tet;
		graphic::cVertexBuffer m_vtxBuffer;
		graphic::cIndexBuffer m_idxBuffer;
		graphic::cVertexBuffer m_normalBuffer;
		graphic::cVertexBuffer m_edgeBuffer;
	};

}
