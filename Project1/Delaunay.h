#ifndef _DELAUNAY_H_
#define _DELAUNAY_H_
#include <vector>
#include "Triangle.h"
#include "Edge.h"



class Delaunay
{
public:
	std::vector<Triangle> triangleVector; // 三角形集合
	std::vector<glm::vec2> pointVector; // 点集合
	std::vector<Edge> edgeVector;

	Delaunay();

	~Delaunay();

	void initialize(const glm::vec4 &, const std::vector < glm::vec2>& vertices);

	/**
	 * 加入新点
	 * @x: 新点x坐标
	 * @y: 新点y坐标
	 */
	void addPoint(int x, int y);

	/**
	 * 判断像素点是否在三角形中
	 * @x: 圆心横坐标
	 * @y: 圆心纵坐标
	 * @r: 圆半径
	 * @newPoint: 像素点坐标
	 */
	bool isInCircle(const double& x, const double& y, const double& r, const glm::vec2& newPoint);

	void deleteCommonEdges(const int& badTriangleID, std::vector<Edge>& boundaryEdges);

	const std::vector<Triangle>& getTriangles();
};
#endif