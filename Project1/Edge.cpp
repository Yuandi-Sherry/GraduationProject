#include "Edge.h"
#include <iostream>

Edge::Edge() {
	isBad = false;
}

Edge::Edge(const int& point1, const int& point2, const int& count) {
	point1ID = point1;
	point2ID = point2;
	this->count = count;
	isBad = false;
}

Edge::~Edge() {

}

bool Edge::equal(const Edge& e1, const Edge& e2) {
	if ((e1.point1ID == e2.point1ID && e1.point2ID == e2.point2ID)
		|| (e1.point1ID == e2.point2ID && e1.point2ID == e2.point1ID)) {
		return true;
	}
	//std::cout << "--- return false --- " << std::endl;
	return false;
}