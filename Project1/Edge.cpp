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
	//std::cout << "in Edge::equal " << std::endl;
	//std::cout << "e1.point1ID " << e1.point1ID << " e2.point1ID " << e2.point1ID << std::endl;
	//std::cout << "e1.point2ID " << e1.point2ID << " e2.point2ID " << e2.point2ID << std::endl;
	if ((e1.point1ID == e2.point1ID && e1.point2ID == e2.point2ID)
		|| (e1.point1ID == e2.point2ID && e1.point2ID == e2.point1ID)) {
		//std::cout << "--- return true --- " << std::endl;
		return true;
	}
	return false;
}