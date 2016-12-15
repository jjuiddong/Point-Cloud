#pragma once

namespace delaunay
{
	class Edge
	{
	public:
		Edge(const Vector3 &p1, const Vector3 &p2) : p1(p1), p2(p2) {};
		Edge(const Edge &e) : p1(e.p1), p2(e.p2) {};

		Vector3 p1;
		Vector3 p2;
	};

	//inline std::ostream &operator << (std::ostream &str, Edge const &e)
	//{
	//	return str << "Edge " << e.p1 << ", " << e.p2;
	//}

	inline bool operator == (const Edge & e1, const Edge & e2)
	{
		return 	(e1.p1 == e2.p1 && e1.p2 == e2.p2) ||
			(e1.p1 == e2.p2 && e1.p2 == e2.p1);
	}
}
