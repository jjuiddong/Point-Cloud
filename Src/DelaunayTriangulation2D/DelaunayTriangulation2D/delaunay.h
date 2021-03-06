#pragma once

namespace delaunay
{
	class Delaunay
	{
	public:
		const std::vector<Triangle2>& triangulate(std::vector<Vec2f> &vertices);
		const std::vector<Triangle2>& getTriangles() const { return _triangles; };
		const std::vector<Edge>& getEdges() const { return _edges; };
		const std::vector<Vec2f>& getVertices() const { return _vertices; };
		void clear();

	private:
		std::vector<Triangle2> _triangles;
		std::vector<Edge> _edges;
		std::vector<Vec2f> _vertices;
	};

}
