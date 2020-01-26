#ifndef _EDGE_H_
#define _EDGE_H_
class Edge
{
public:
	Edge();
	Edge(const int& point1, const int& point2, const int& count = 1);
	~Edge();
	int point1ID, point2ID, count;
	bool static equal(const Edge & e1, const Edge& e2);
	bool isBad = false;
};
#endif