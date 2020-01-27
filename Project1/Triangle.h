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
	int vertices[3]; // ���������Ӧ����������Morphing��PointSet�е��±�
	Edge edges[3]; // �����ε�������
	double x; // ���ԲԲ�ĺ�����
	double y; // ���ԲԲ��������
	double r = -1; // ���ԲԲ�İ뾶

	bool isBad = false;

	// ��������ID��ɵ�������
	Triangle(const int& point1ID, const int& point2ID, const int& point3ID);

	// ����������ɵ�������...��
	Triangle(const glm::vec2& point1, const glm::vec2& point2, const glm::vec2& point3);

	~Triangle();

	/**
	 * �жϵ��Ƿ������Բ��
	 */
	bool circumCircleContains(const std::vector<glm::vec3>& pointVector, const int& point);


	void calcCircumCircle(const std::vector<glm::vec3>& pointVector);

	bool containsVertex(const int& v) const;
};
#endif