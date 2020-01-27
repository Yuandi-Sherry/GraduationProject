#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_
#include <iostream>
#include <cstdlib>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Edge.h"
using namespace std;

class Triangle
{
public:
	glm::vec2 points[3];
	int vertices[3]; // 三个顶点对应的特征点在Morphing的PointSet中的下标
	Edge edges[3]; // 三角形的三条边
	double x; // 外接圆圆心横坐标
	double y; // 外接圆圆心纵坐标
	double r = -1; // 外接圆圆心半径

	bool isBad = false;

	// 三个顶点ID组成的三角形
	Triangle(const int& point1ID, const int& point2ID, const int& point3ID);

	// 三个顶点组成的三角形...？
	Triangle(const glm::vec2& point1, const glm::vec2& point2, const glm::vec2& point3);

	~Triangle();

	/**
	 * 判断点是否在外接圆内
	 */
	bool circumCircleContains(const std::vector<glm::vec3>& pointVector, const int& point);


	void calcCircumCircle(const std::vector<glm::vec3>& pointVector);

	bool containsVertex(const int& v) const;
};
#endif