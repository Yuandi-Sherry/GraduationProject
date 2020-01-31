#ifndef _DELAUNAY_1_H_
#define _DELAUNAY_1_H_
#include <vector>
#include "Triangle.h"
#include "Edge.h"

class Delaunay1
{
public:
	Delaunay1();
	~Delaunay1();
	std::vector<Triangle> triangleVector; // 三角形集合
	std::vector<glm::vec3> pointVector; // 点集合
	std::vector<Edge> edgeVector;

	void initialize(const glm::vec4&);

	/**
	 * 加入新点
	 * @x: 新点x坐标
	 * @y: 新点y坐标
	 */
	void addPoint(const glm::vec2& newPoint);

	/**
	 * 判断像素点是否在三角形中
	 * @x: 圆心横坐标
	 * @y: 圆心纵坐标
	 * @r: 圆半径
	 * @newPoint: 像素点坐标
	 */

	void deleteCommonEdges(const int& badTriangleID, std::vector<Edge>& boundaryEdges);

	void deleteSuperTriangle();
};


#endif // !_DELAUNAY_1_H_
