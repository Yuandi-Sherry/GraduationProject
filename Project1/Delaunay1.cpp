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
	superTriNum = pointVector.size();
}

/**
 * �����µ�
 * @x: �µ�x����
 * @y: �µ�y����
 */
void Delaunay1::addPoint(const glm::vec2 & newPoint) {
	// ��¼��ǻ
	std::list<Edge> polygon;
	int index = pointVector.size();
	pointVector.push_back(glm::vec3(newPoint, 0.0f));

	for (std::list<Triangle>::iterator it = triangleVector.begin(); it != triangleVector.end(); it++) {
		// �������Բ����p��������
		if (it->circumCircleContains(pointVector, index)) {
			it->isBad = true;

			// �������α�Ե��Ϊ��ǻ
			polygon.push_back(Edge(it->vertices[0], it->vertices[1]));
			polygon.push_back(Edge(it->vertices[1], it->vertices[2]));
			polygon.push_back(Edge(it->vertices[2], it->vertices[0]));
		}
	}
	// �Ƴ�Ӱ��������
	for (std::list<Triangle>:: iterator it = triangleVector.begin(); it != triangleVector.end();) {
		if (it->isBad) {
			it = triangleVector.erase(it);
		}
		else {
			it++;
		}
	}
	deleteCommonEdges(polygon);
	

	// ���²���ĵ�Ϳ�ǻ�ı�������
	for (std::list<Edge>::iterator it = polygon.begin(); it != polygon.end(); it++) {
		triangleVector.push_back(Triangle(index, it->point1ID, it->point2ID));
	}
}

/**
 * �ж����ص��Ƿ�����������
 * @x: Բ�ĺ�����
 * @y: Բ��������
 * @r: Բ�뾶
 * @newPoint: ���ص�����
 */

void Delaunay1::deleteCommonEdges(std::list<Edge>& polygon) {
	// ���������ι�����
	for (std::list<Edge>::iterator e1 = polygon.begin(); e1 != polygon.end(); e1++) {
		std::list<Edge>::iterator e2 = e1;
		e2++;
		for (; e2 != polygon.end(); e2++) {
			if (Edge::equal(*e1, *e2)) {
				e1->isBad = true;
				e2->isBad = true;
			}
		}
	}

	// ɾ�������ι����ߣ��γɰ���p�Ŀ�ǻ
	for (std::list<Edge>::iterator it = polygon.begin(); it != polygon.end(); ) {
		if (it->isBad) {
			it = polygon.erase(it);
		}
		else {
			it++;
		}
	}
}


void Delaunay1::deleteSuperTriangle() {
	for (std::list<Triangle>::iterator it = triangleVector.begin(); it != triangleVector.end(); ) {
		if (it->containsVertex(0) || it->containsVertex(1)
			|| it->containsVertex(2) || it->containsVertex(3)) {
			it = triangleVector.erase(it);
		}
		else {
			it++;
		}
	}

}