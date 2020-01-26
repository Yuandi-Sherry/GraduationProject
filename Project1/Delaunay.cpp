#include "Delaunay.h"
#include <vector>
#include <cstdlib>
#include <algorithm>
using namespace std;

Delaunay::Delaunay() {

}
Delaunay::~Delaunay() {

}

void Delaunay::initialize(const glm::vec4 & boundingBox, const std::vector<glm::vec2> & vertices) {
	triangleVector.clear(); // 三角形集合
	pointVector.clear(); // 点集合
	edgeVector.clear();
	// 包围盒的尺寸
	const double dx = boundingBox.z - boundingBox.x;
	const double dy = boundingBox.w - boundingBox.y;
	const double deltaMax = max(dx, dy);
	// 中点坐标
	const double midx = (boundingBox.x + boundingBox.z) / 2.0f;
	const double midy = (boundingBox.y + boundingBox.w) / 2.0f;
	// 超级三角形
	const glm::vec2 p1(midx - 20 * deltaMax, midy - deltaMax);
	const glm::vec2 p2(midx, midy + 20 * deltaMax);
	const glm::vec2 p3(midx + 20 * deltaMax, midy - deltaMax);

	pointVector.push_back(p1);
	pointVector.push_back(p2);
	pointVector.push_back(p3);

	pointVector.insert(pointVector.end(), vertices.begin(), vertices.end());

	std::cout << "pointVector.size() " << pointVector.size() << std::endl;

	triangleVector.push_back(Triangle(0, 1, 2));

	for (int i = 3; i < pointVector.size(); i++) {
		std::vector<Edge> polygon;
		for (int j = 0; j < triangleVector.size(); j++) {	
			// 所有外接圆包含p的三角形
			if (triangleVector[j].circumCircleContains(pointVector, i)) {
				triangleVector[j].isBad = true;
				// 将三角形边缘作为空腔
				polygon.push_back(Edge(triangleVector[j].vertices[0], triangleVector[j].vertices[1]));
				polygon.push_back(Edge(triangleVector[j].vertices[1], triangleVector[j].vertices[2]));
				polygon.push_back(Edge(triangleVector[j].vertices[2], triangleVector[j].vertices[0]));
			}
		}
		// 移除影响三角形
		for (int j = 0; j < triangleVector.size(); ) {
			if (triangleVector[j].isBad) {
				triangleVector.erase(triangleVector.begin() + j);
			}
			else {
				j++;
			}
		}
		// 查找三角形公共边
		for (int e1 = 0; e1 < polygon.size(); e1++) {
			for (int e2 = e1 + 1; e2 < polygon.size(); e2++) {
				// 如果两个边相等
				if (Edge::equal(polygon[e1], polygon[e2])) {
					polygon[e1].isBad = true;
					polygon[e2].isBad = true;
				}
			}
		}

		// 删除三角形公共边，形成包含p的空腔
		for (int j = 0; j < polygon.size(); ) {
			if (polygon[j].isBad) {
				polygon.erase(polygon.begin() + j);
			}
			else {
				j++;
			}
		}

		// 将新插入的点和空腔的边相连接
		for (int j = 0; j < polygon.size(); j++) {
			triangleVector.push_back(Triangle(i, polygon[j].point1ID, polygon[j].point2ID));
		}
	}
	
	// 移除包含超级三角形顶点的三角形
	for (int i = 0; i < triangleVector.size(); ) {
		if (triangleVector[i].containsVertex(0) || triangleVector[i].containsVertex(1) || triangleVector[i].containsVertex(2)) {
			triangleVector.erase(triangleVector.begin() + i);
		}
		else {
			i++;
		}
	}

	for (int i = 0; i < triangleVector.size(); i++) {
		edgeVector.push_back(Edge(triangleVector[i].vertices[0], triangleVector[i].vertices[1]));
		edgeVector.push_back(Edge(triangleVector[i].vertices[1], triangleVector[i].vertices[2]));
		edgeVector.push_back(Edge(triangleVector[i].vertices[2], triangleVector[i].vertices[0]));
	}
	//triangleVector.erase()
	//------------------------
	// points
	/*for (int i = 0; i < 4; i++) {
		pointVector.push_back(boundary4Points[i]);
	}
	// edges
	Edge topEdge(0, 1, -1);
	Edge leftEdge(0, 2, -1);
	Edge bottomEdge(2, 3, -1);
	Edge rightEdge(1, 3, -1);
	Edge dia(0, 3, 2);
	edgeVector.push_back(topEdge);
	edgeVector.push_back(leftEdge);
	edgeVector.push_back(bottomEdge);
	edgeVector.push_back(rightEdge);
	edgeVector.push_back(dia);

	// triangles
	Triangle newTriangle1(0, 1, 3);
	Triangle newTriangle2(0, 2, 3);
	newTriangle1.calculateCircle(pointVector);
	newTriangle2.calculateCircle(pointVector);
	triangleVector.push_back(newTriangle1);
	triangleVector.push_back(newTriangle2);*/
}

const std::vector<Triangle>& Delaunay::getTriangles() {
	return triangleVector;
}
/**
 * 加入新点
 * @x: 新点x坐标
 * @y: 新点y坐标
 */
void Delaunay::addPoint(int x, int y) {
	/*std::vector<Edge> boundaryEdges;
	glm::vec2 newPoint(x, y);
	pointVector.push_back(newPoint);
	std::vector<int> affectingTriangles;// 所有影响三角形的id
	for (int i = 0; i < triangleVector.size(); i++) {
		if (isInCircle(triangleVector[i].x, triangleVector[i].y, triangleVector[i].r, newPoint)) {
			// 影响三角形: 将影响三角形在vector中的id加入
			affectingTriangles.push_back(i);
		}
	}

	// 删除影响三角形的边
	for (int i = 0; i < affectingTriangles.size(); i++) {
		deleteCommonEdges(affectingTriangles[i], boundaryEdges);
		// 修正id
		for (int j = i + 1; j < affectingTriangles.size(); j++) {
			affectingTriangles[j]--;
		}
	}

	// 将插入点同影响三角形的全部顶点连接起来
	for (int i = 0; i < boundaryEdges.size(); i++) {
		Triangle newTriangle(boundaryEdges[i].point1ID, boundaryEdges[i].point2ID, pointVector.size() - 1);
		newTriangle.calculateCircle(pointVector);
		// 插入新生成的边
		for (int j = 0; j < 3; j++) {
			bool flag = 1;
			for (int k = 0; k < edgeVector.size(); k++) {
				if (newTriangle.edges[j].point1ID == edgeVector[k].point1ID
					&& newTriangle.edges[j].point2ID == edgeVector[k].point2ID
					&& edgeVector[k].count == -1) {
					// 边缘已经存
					flag = 0;
				}
				else if (newTriangle.edges[j].point1ID == edgeVector[k].point1ID
					&& newTriangle.edges[j].point2ID == edgeVector[k].point2ID
					&& edgeVector[k].count != -1) {
					flag = 0;
					edgeVector[k].count++;
				}
			}
			if (flag == 1) {
				edgeVector.push_back(newTriangle.edges[j]);
			}
		}
		triangleVector.push_back(newTriangle);
	}*/
}

bool Delaunay::isInCircle(const double& x, const double& y, const double& r, const glm::vec2& newPoint) {
	// 新点到圆心的距离小于圆半径即在圆内
	if (sqrt(pow(newPoint.x - x, 2) + pow(newPoint.y - y, 2)) < r) {
		return true;
	}
	return false;
}

void Delaunay::deleteCommonEdges(const int& badTriangleID, std::vector<Edge>& boundaryEdges) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < edgeVector.size(); j++) {
			// 在边缘数组中找到该影响三角形的边
			if (edgeVector[j].point1ID == triangleVector[badTriangleID].edges[i].point1ID
				&& edgeVector[j].point2ID == triangleVector[badTriangleID].edges[i].point2ID) {
				if (edgeVector[j].count == -1) {
					boundaryEdges.push_back(edgeVector[j]);
				}
				else if (edgeVector[j].count == 2) {
					// 删掉三角形剩下的边
					edgeVector[j].count = 1;
					boundaryEdges.push_back(edgeVector[j]);
				}
				else if (edgeVector[j].count == 1) {
					// 将该边彻底删除
					for (int p = 0; p < boundaryEdges.size(); p++) {
						if (boundaryEdges[p].point1ID == edgeVector[j].point1ID
							&& boundaryEdges[p].point2ID == edgeVector[j].point2ID) {
							// 先将边缘从boundaryEdges中删去
							boundaryEdges.erase(boundaryEdges.begin() + p);
							break;
						}
					}
					// 再将边缘从edgesVector中删去
					edgeVector.erase(edgeVector.begin() + j);
					j--;
				}
				break;
			}
		}
	}
	triangleVector.erase(triangleVector.begin() + badTriangleID);
}