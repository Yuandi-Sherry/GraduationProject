#include "Delaunay.h"
#include <vector>
#include <cstdlib>
#include <algorithm>
using namespace std;

bool comp(glm::vec3 ele1, glm::vec3 ele2) {
	return ele1.x < ele2.x;
}
Delaunay::Delaunay() {
	triangleVector.clear(); // 三角形集合
	pointVector.clear(); // 点集合
	edgeVector.clear();
}
Delaunay::~Delaunay() {

}

void Delaunay::initialize(const glm::vec4 & boundingBox, const std::vector<glm::vec2> & vertices, const int & size) {
	triangleVector.clear(); // 三角形集合
	pointVector.clear(); // 点集合
	edgeVector.clear();
	// ----------------------------- PLAN A -----------------------------------
	// 初始化顶点列表
	/*for (int i = 0; i < vertices.size(); i++) {
		pointVector.push_back(glm::vec3(vertices[i],i+3));
	}
	// 根据x的位置排序
	std::sort(pointVector.begin(), pointVector.end(), comp);

	// 确定超级三角形
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

	pointVector.insert(pointVector.begin(), glm::vec3(p3, 2));
	pointVector.insert(pointVector.begin(), glm::vec3(p2, 1));
	pointVector.insert(pointVector.begin(), glm::vec3(p1, 0));
	
	std::cout << "pointVector.size() " << pointVector.size() << std::endl;
	std::vector<Triangle> tmpTriangles;
	// 将超级三角形保存至未确定三角形列表（temp triangles）
	tmpTriangles.push_back(Triangle(0, 1, 2));
	// 将超级三角形push到triangles列表
	triangleVector.push_back(Triangle(0, 1, 2));
	// 遍历基于indices顺序的vertices中每一个点,顶点则是由x从小到大出现
	for (int i = 3; i < pointVector.size(); i++) {
		// 初始化边缓存数组（edge buffer）
		std::vector<Edge> polygon;
		// 遍历temp triangles中的每一个三角形
		for (int j = 0; j < tmpTriangles.size(); j++) {
			// 计算该三角形的圆心和半径
			tmpTriangles[j].calcCircumCircle(pointVector);
			// 如果该点在外接圆的右侧
			if (pointVector[i].x > tmpTriangles[j].x + tmpTriangles[j].r) {
				// std::cout << "pointVector[i].x " << pointVector[i].x << " tmpTriangles[j].x  " << tmpTriangles[j].x << " tmpTriangles[j].r " << tmpTriangles[j].r  << " 点在外接圆的右侧 " << std::endl;
				// 则该三角形为Delaunay三角形，保存到triangles
				triangleVector.push_back(tmpTriangles[j]);
				// 并在temp里去除掉
				tmpTriangles.erase(tmpTriangles.begin() + j);
				j--;
				continue;

			}
			// 如果该点在外接圆外（即也不是外接圆右侧）
			else if (glm::distance(glm::vec2(pointVector[i]), glm::vec2(tmpTriangles[j].x, tmpTriangles[j].y)) > tmpTriangles[j].r) {
				// 则该三角形为不确定
				// 后面会在问题中讨论
				// 跳过
			}
			// 如果该点在外接圆内
			else {
				// 则该三角形不为Delaunay三角形
				// 将三边保存至edge buffer
				polygon.push_back(Edge(tmpTriangles[j].vertices[0], tmpTriangles[j].vertices[1]));
				polygon.push_back(Edge(tmpTriangles[j].vertices[1], tmpTriangles[j].vertices[2]));
				polygon.push_back(Edge(tmpTriangles[j].vertices[2], tmpTriangles[j].vertices[0]));
				// 在temp中去除掉该三角形
				tmpTriangles.erase(tmpTriangles.begin() + j);
				j--;
				continue;
			}
		}
		// 对edge buffer进行去重
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
			tmpTriangles.push_back(Triangle(i, polygon[j].point1ID, polygon[j].point2ID));
		}
	}
	// todo: 将triangles与temp triangles进行合并
	/*for (int i = 0; i < tmpTriangles.size(); i++) {
		triangleVector.push_back(tmpTriangles[i]);
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
	std::cout << "triangleVector.size() " << triangleVector.size() << std::endl;*/

	// ----------------------------- PLAN B -----------------------------------
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

	pointVector.push_back(glm::vec3(p1, 0.0f));
	pointVector.push_back(glm::vec3(p2, 0.0f));
	pointVector.push_back(glm::vec3(p3, 0.0f));

	for (int i = 0; i < vertices.size(); i++) {
		pointVector.push_back(glm::vec3(vertices[i], 0.0f));

	}

	std::cout << "pointVector.size() " << pointVector.size() << std::endl;

	triangleVector.push_back(Triangle(0, 1, 2));

	for (int i = 3; i < size+3; i++) {
		std::vector<Edge> polygon;
		
		int count1 = 0;
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
		//std::cout << "最初：polygon.size() " << polygon.size() << std::endl;
		// 移除影响三角形：todo - list
		
		for (int j = 0; j < triangleVector.size(); ) {
			if (triangleVector[j].isBad) {
				triangleVector.erase(triangleVector.begin() + j);
			}
			else {
				j++;
			}
		}
		
		std::vector<Edge> tmp;
		std::vector<pair<int,int>> tmpIndex;
		// 查找三角形公共边
		for (int e1 = 0; e1 < polygon.size(); e1++) {
			for (int e2 = e1 + 1; e2 < polygon.size(); e2++) {
				// 如果两个边相等
				if (Edge::equal(polygon[e1], polygon[e2])) {
					if (polygon[e1].isBad || polygon[e2].isBad) {
						tmp.push_back(polygon[e1]);
						tmp.push_back(polygon[e2]);
						tmpIndex.push_back(make_pair(e1,e2));
						count1++;
					}
					polygon[e1].isBad = true;
					polygon[e2].isBad = true;
				}
			}
		}
		if (count1 != 0) {
			std::cout << "重复polygon：" << count1 << std::endl;
			for (int j= 0; j < tmp.size(); j+=2) {
				std::cout << "edge1: "<< tmpIndex[j/2].first << " " << tmp[j].point1ID << " " << tmp[j].point2ID << std::endl;
				std::cout << "edge2: " << tmpIndex[j / 2].second << " " << tmp[j+1].point1ID << " " << tmp[j+1].point2ID << std::endl;

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
	/*for (int i = 0; i < triangleVector.size(); i++) {
		edgeVector.push_back(Edge(triangleVector[i].vertices[0], triangleVector[i].vertices[1]));
		edgeVector.push_back(Edge(triangleVector[i].vertices[1], triangleVector[i].vertices[2]));
		edgeVector.push_back(Edge(triangleVector[i].vertices[2], triangleVector[i].vertices[0]));
	}*/
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