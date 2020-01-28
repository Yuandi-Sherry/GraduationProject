#include "Triangle.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <cstdlib>
#include "Edge.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
int compare(const void * a, const void * b) {
	int *pa = (int*)a;
    int *pb = (int*)b;
    return (*pa )- (*pb);  //从小到大排序
}


Triangle::Triangle(const int & point1ID, const int & point2ID, const int & point3ID) {
	vertices[0] = point1ID;
	vertices[1] = point2ID;
	vertices[2] = point3ID;
	qsort(vertices, 3, sizeof(vertices[0]),compare);
	Edge temp1(point1ID, point2ID, 1);
	Edge temp2(point1ID, point3ID, 1);
	Edge temp3(point2ID, point3ID, 1);
	edges[0] = temp1;
	edges[1] = temp2;
	edges[2] = temp3;
	isBad = false;
}

Triangle::Triangle(const glm::vec2 & point1, const glm::vec2 & point2, const glm::vec2 & point3) {
	points[0] = point1;
	points[1] = point2;
	points[2] = point3;
	isBad = false;
}
Triangle::~Triangle() {

}
/**
 * 计算三角形的外接圆
 */
bool Triangle::circumCircleContains(const std::vector<glm::vec3> & pointVector, const int& point) {
	if (r == -1) {
		calcCircumCircle(pointVector);
	}
	if (glm::distance(glm::vec2(x, y), glm::vec2(pointVector[point])) <= r) {
		return true;
	}
	return false;
}


void Triangle::calcCircumCircle(const std::vector<glm::vec3>& pointVector) {
	if (r == -1) {
		float a = 2 * (pointVector[vertices[1]].x - pointVector[vertices[0]].x);
		float b = 2 * (pointVector[vertices[1]].y - pointVector[vertices[0]].y);
		float c = pointVector[vertices[1]].x * pointVector[vertices[1]].x + pointVector[vertices[1]].y * pointVector[vertices[1]].y - pointVector[vertices[0]].x * pointVector[vertices[0]].x - pointVector[vertices[0]].y * pointVector[vertices[0]].y;
		float d = 2 * (pointVector[vertices[2]].x - pointVector[vertices[1]].x);
		float e = 2 * (pointVector[vertices[2]].y - pointVector[vertices[1]].y);
		float f = pointVector[vertices[2]].x * pointVector[vertices[2]].x + pointVector[vertices[2]].y * pointVector[vertices[2]].y - pointVector[vertices[1]].x * pointVector[vertices[1]].x - pointVector[vertices[1]].y * pointVector[vertices[1]].y;
		x = (b * f - e * c) / (b * d - e * a);
		y = (d * c - a * f) / (b * d - e * a);
		r = glm::distance(glm::vec2(x, y), glm::vec2(pointVector[vertices[0]]));
	}
}
/**
 * 三角形是否包含某一顶点
 */
bool Triangle::containsVertex(const int & v) const
{
	// return p1 == v || p2 == v || p3 == v;
	return vertices[0] == v || vertices[1] == v || vertices[2] == v;
}