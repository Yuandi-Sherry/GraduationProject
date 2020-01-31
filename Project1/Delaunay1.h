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
	std::vector<Triangle> triangleVector; // �����μ���
	std::vector<glm::vec3> pointVector; // �㼯��
	std::vector<Edge> edgeVector;

	void initialize(const glm::vec4&);

	/**
	 * �����µ�
	 * @x: �µ�x����
	 * @y: �µ�y����
	 */
	void addPoint(const glm::vec2& newPoint);

	/**
	 * �ж����ص��Ƿ�����������
	 * @x: Բ�ĺ�����
	 * @y: Բ��������
	 * @r: Բ�뾶
	 * @newPoint: ���ص�����
	 */

	void deleteCommonEdges(const int& badTriangleID, std::vector<Edge>& boundaryEdges);

	void deleteSuperTriangle();
};


#endif // !_DELAUNAY_1_H_
