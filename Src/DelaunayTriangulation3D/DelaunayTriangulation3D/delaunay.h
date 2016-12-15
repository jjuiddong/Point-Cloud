#pragma once

namespace delaunay
{
	class Delaunay
	{
	public:
		const std::vector<Triangle2>& triangulate(std::vector<Vector3> &vertices);
		const std::vector<Triangle2>& getTriangles() const { return _triangles; };
		const std::vector<Edge>& getEdges() const { return _edges; };
		const std::vector<Vector3>& getVertices() const { return _vertices; };
		void clear();

	private:
		std::vector<Triangle2> _triangles;
		std::vector<Edge> _edges;
		std::vector<Vector3> _vertices;
	};

}
