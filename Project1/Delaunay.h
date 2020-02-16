#ifndef _DELAUNAY_1_H_
#define _DELAUNAY_1_H_
#include <list>
#include "Triangle.h"
#include "Edge.h"

class Delaunay
{
public:
	Delaunay();
	~Delaunay();
	int superTriNum = 0;
	std::list<Triangle> triangleVector; // �����μ���
	std::vector<glm::vec3> pointVector; // �㼯��
	std::list<Edge> edgeVector;

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

	void deleteCommonEdges(std::list<Edge>& polygon);

	void deleteSuperTriangle();
};


#endif // !_DELAUNAY_1_H_
