#ifndef _DELAUNAY_H_
#define _DELAUNAY_H_
#include <vector>
#include "Triangle.h"
#include "Edge.h"



class Delaunay
{
public:
	std::vector<Triangle> triangleVector; // �����μ���
	std::vector<glm::vec2> pointVector; // �㼯��
	std::vector<Edge> edgeVector;

	Delaunay();

	~Delaunay();

	void initialize(const glm::vec4 &, const std::vector < glm::vec2>& vertices);

	/**
	 * �����µ�
	 * @x: �µ�x����
	 * @y: �µ�y����
	 */
	void addPoint(int x, int y);

	/**
	 * �ж����ص��Ƿ�����������
	 * @x: Բ�ĺ�����
	 * @y: Բ��������
	 * @r: Բ�뾶
	 * @newPoint: ���ص�����
	 */
	bool isInCircle(const double& x, const double& y, const double& r, const glm::vec2& newPoint);

	void deleteCommonEdges(const int& badTriangleID, std::vector<Edge>& boundaryEdges);

	const std::vector<Triangle>& getTriangles();
};
#endif