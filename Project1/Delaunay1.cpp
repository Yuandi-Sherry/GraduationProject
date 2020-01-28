#include "Delaunay1.h"
#include <algorithm>

Delaunay1::Delaunay1()
{
}

Delaunay1::~Delaunay1()
{
}

void Delaunay1::initialize(const glm::vec4& boundingBox) {
    triangleVector.clear();
    pointVector.clear();
    edgeVector.clear();
    // ��������������
   /* const double dx = boundingBox.z - boundingBox.x;
    const double dy = boundingBox.w - boundingBox.y;
    const double deltaMax = std::max(dx, dy);

    // �е�����
    const double midx = (boundingBox.x + boundingBox.z) / 2.0f;
    const double midy = (boundingBox.y + boundingBox.w) / 2.0f;
    // ����������������������
    const glm::vec2 p1(midx - 20 * deltaMax, midy - deltaMax);
    const glm::vec2 p2(midx, midy + 20 * deltaMax);
    const glm::vec2 p3(midx + 20 * deltaMax, midy - deltaMax);

	glm::vec2 range = glm::vec2(boundingBox.z - boundingBox.x, boundingBox.w - boundingBox.y);
	// �Ϸ�����:
	glm::vec2 ver1 = glm::vec2((boundingBox.x + boundingBox.z) / 2.0f, boundingBox.w + range.y + 1.0f);
	// ���½Ƕ���
	glm::vec2 ver2 = glm::vec2(boundingBox.x - range.x / 2.0f - 1.0f, boundingBox.y - 1.0f);
	glm::vec2 ver3 = glm::vec2(boundingBox.z + range.x / 2.0f + 1.0f, boundingBox.y - 1.0f);
    
    // �����������εĶ�����붥������
    pointVector.push_back(glm::vec3(ver1, 0.0f));
    pointVector.push_back(glm::vec3(ver2, 0.0f));
    pointVector.push_back(glm::vec3(ver3, 0.0f));*/
	glm::vec4 tmp = boundingBox;
	tmp *= 1.1f;
	glm::vec3 lefttop = glm::vec3(boundingBox.x, boundingBox.y, 0.0f);
	glm::vec3 leftbottom = glm::vec3(boundingBox.x, boundingBox.w, 0.0f);
	glm::vec3 righttop = glm::vec3(boundingBox.z, boundingBox.y, 0.0f);
	glm::vec3 rightbottom = glm::vec3(boundingBox.z, boundingBox.w, 0.0f);
	pointVector.push_back(lefttop);
	pointVector.push_back(leftbottom);
	pointVector.push_back(righttop);
	pointVector.push_back(rightbottom);

	triangleVector.push_back(Triangle(0, 1, 2));
	triangleVector.push_back(Triangle(3, 1, 2));
}

/**
 * �����µ�
 * @x: �µ�x����
 * @y: �µ�y����
 */
void Delaunay1::addPoint(const glm::vec2 & newPoint) {
	int count1 = 0;
	std::cout << "---------------------------------------" << std::endl;
	// ��¼��ǻ
	std::vector<Edge> polygon;
	int index = pointVector.size();
	pointVector.push_back(glm::vec3(newPoint, 0.0f));

	for (int i = 0; i < triangleVector.size(); i++) {
		// �������Բ����p��������
		if (triangleVector[i].circumCircleContains(pointVector, index)) {
			triangleVector[i].isBad = true;

			// �������α�Ե��Ϊ��ǻ
			polygon.push_back(Edge(triangleVector[i].vertices[0], triangleVector[i].vertices[1]));
			polygon.push_back(Edge(triangleVector[i].vertices[1], triangleVector[i].vertices[2]));
			polygon.push_back(Edge(triangleVector[i].vertices[2], triangleVector[i].vertices[0]));
		}
	}
	//std::cout << "�����triangleVector.size() " << triangleVector.size() << std::endl;
	//std::cout << "�����polygon.size() " << polygon.size() << std::endl;
	// �Ƴ�Ӱ�������Σ�todo - list

	for (int i = 0; i < triangleVector.size(); ) {
		if (triangleVector[i].isBad) {
			triangleVector.erase(triangleVector.begin() + i);
		}
		else {
			i++;
		}
	}
	//std::cout << "ɾ����triangleVector.size() " << triangleVector.size() << std::endl;
	std::vector<Edge> tmp;
	std::vector<pair<int, int>> tmpIndex;
	// ���������ι�����
	for (int e1 = 0; e1 < polygon.size(); e1++) {
		for (int e2 = e1 + 1; e2 < polygon.size(); e2++) {
			// ������������
			if (Edge::equal(polygon[e1], polygon[e2])) {
				if (polygon[e1].isBad || polygon[e2].isBad) {
					tmp.push_back(polygon[e1]);
					tmp.push_back(polygon[e2]);
					tmpIndex.push_back(make_pair(e1, e2));
					count1++;
				}
				polygon[e1].isBad = true;
				polygon[e2].isBad = true;
			}
		}
	}
	/*if (count1 != 0) {
		std::cout << "�ظ�polygon��" << count1 << std::endl;
		for (int j = 0; j < tmp.size(); j += 2) {
			std::cout << "edge1: " << tmpIndex[j / 2].first << " " << tmp[j].point1ID << " " << tmp[j].point2ID << std::endl;
			std::cout << "edge2: " << tmpIndex[j / 2].second << " " << tmp[j + 1].point1ID << " " << tmp[j + 1].point2ID << std::endl;
		}
	}*/


	// ɾ�������ι����ߣ��γɰ���p�Ŀ�ǻ
	for (int i = 0; i < polygon.size(); ) {
		if (polygon[i].isBad) {
			polygon.erase(polygon.begin() + i);
		}
		else {
			i++;
		}
	}
	//std::cout << "ɾ����polygon.size() " << polygon.size() << std::endl;
	// ���²���ĵ�Ϳ�ǻ�ı�������
	for (int i = 0; i < polygon.size(); i++) {
		int count2 = 0;
		// �¼���������������֮ǰ�������������������г��������ظ��ߣ����ظ�
		for (int j = 0; j < triangleVector.size(); j++) {
			if (Edge::equal(Edge(polygon[i].point1ID, polygon[i].point2ID), 
				Edge(triangleVector[j].vertices[0], triangleVector[j].vertices[1]))) {
				count2++;
			}
			else if (Edge::equal(Edge(polygon[i].point1ID, polygon[i].point2ID),
				Edge(triangleVector[j].vertices[0], triangleVector[j].vertices[2]))) {
				count2++;
			}
			else if (Edge::equal(Edge(polygon[i].point1ID, polygon[i].point2ID),
				Edge(triangleVector[j].vertices[1], triangleVector[j].vertices[2]))) {
				count2++;
			}
		}
		if (count2 > 1) {
			std::cout << "ID1�� " << polygon[i].point1ID << "  ID2: " << polygon[i].point2ID << std::endl;
		}
		triangleVector.push_back(Triangle(index, polygon[i].point1ID, polygon[i].point2ID));
	}
	// debug
	/*if (count1 != 0) {
		for (int i = 0; i < triangleVector.size(); i++) {
			std::cout << "triangle: " << i << " - ";
			for (int j = 0; j < 3; j++) {
				std::cout << triangleVector[i].vertices[j] << " ";
			}
			std::cout << std::endl;
		}
	}*/
	

}

/**
 * �ж����ص��Ƿ�����������
 * @x: Բ�ĺ�����
 * @y: Բ��������
 * @r: Բ�뾶
 * @newPoint: ���ص�����
 */

void Delaunay1::deleteCommonEdges(const int& badTriangleID, std::vector<Edge>& boundaryEdges) {

}