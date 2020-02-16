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
    // 构建超级三角形
   /* const double dx = boundingBox.z - boundingBox.x;
    const double dy = boundingBox.w - boundingBox.y;
    const double deltaMax = std::max(dx, dy);

    // 中点坐标
    const double midx = (boundingBox.x + boundingBox.z) / 2.0f;
    const double midy = (boundingBox.y + boundingBox.w) / 2.0f;
    // 超级三角形三个顶点坐标
    const glm::vec2 p1(midx - 20 * deltaMax, midy - deltaMax);
    const glm::vec2 p2(midx, midy + 20 * deltaMax);
    const glm::vec2 p3(midx + 20 * deltaMax, midy - deltaMax);

	glm::vec2 range = glm::vec2(boundingBox.z - boundingBox.x, boundingBox.w - boundingBox.y);
	// 上方顶点:
	glm::vec2 ver1 = glm::vec2((boundingBox.x + boundingBox.z) / 2.0f, boundingBox.w + range.y + 1.0f);
	// 左下角顶点
	glm::vec2 ver2 = glm::vec2(boundingBox.x - range.x / 2.0f - 1.0f, boundingBox.y - 1.0f);
	glm::vec2 ver3 = glm::vec2(boundingBox.z + range.x / 2.0f + 1.0f, boundingBox.y - 1.0f);
    
    // 将超级三角形的顶点放入顶点数组
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
 * 加入新点
 * @x: 新点x坐标
 * @y: 新点y坐标
 */
void Delaunay1::addPoint(const glm::vec2 & newPoint) {
	// 记录空腔
	std::list<Edge> polygon;
	int index = pointVector.size();
	pointVector.push_back(glm::vec3(newPoint, 0.0f));

	for (std::list<Triangle>::iterator it = triangleVector.begin(); it != triangleVector.end(); it++) {
		// 所有外接圆包含p的三角形
		if (it->circumCircleContains(pointVector, index)) {
			it->isBad = true;

			// 将三角形边缘作为空腔
			polygon.push_back(Edge(it->vertices[0], it->vertices[1]));
			polygon.push_back(Edge(it->vertices[1], it->vertices[2]));
			polygon.push_back(Edge(it->vertices[2], it->vertices[0]));
		}
	}
	// 移除影响三角形
	for (std::list<Triangle>:: iterator it = triangleVector.begin(); it != triangleVector.end();) {
		if (it->isBad) {
			it = triangleVector.erase(it);
		}
		else {
			it++;
		}
	}
	deleteCommonEdges(polygon);
	

	// 将新插入的点和空腔的边相连接
	for (std::list<Edge>::iterator it = polygon.begin(); it != polygon.end(); it++) {
		triangleVector.push_back(Triangle(index, it->point1ID, it->point2ID));
	}
}

/**
 * 判断像素点是否在三角形中
 * @x: 圆心横坐标
 * @y: 圆心纵坐标
 * @r: 圆半径
 * @newPoint: 像素点坐标
 */

void Delaunay1::deleteCommonEdges(std::list<Edge>& polygon) {
	// 查找三角形公共边
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

	// 删除三角形公共边，形成包含p的空腔
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