#include "utils.h"
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include "Delaunay.h"
#include <cmath>

std::vector<glm::vec3> myUtils::movement26 = {
		{-1,-1,-1}, {-1,-1, 0},{-1,-1, 1},
		{-1, 0,-1}, {-1, 0, 0},{-1, 0, 1},
		{-1, 1,-1}, {-1, 1, 0},{-1, 1, 1},
		{ 0,-1,-1}, { 0,-1, 0},{ 0,-1, 1},
		{ 0, 0,-1}, /*{0,0,0}*/{ 0, 0, 1},
		{ 0, 1,-1}, { 0, 1, 0},{ 0, 1, 1},
		{ 1,-1,-1}, { 1,-1, 0},{ 1,-1, 1},
		{ 1, 0,-1}, { 1, 0, 0},{ 1, 0, 1},
		{ 1, 1,-1}, { 1, 1, 0},{ 1, 1, 1}
};

void myUtils::neighbors26(const glm::vec3 curPos, std::vector <glm::vec3>& result) {
	if (result.size() > 0) {
		std::cout << "in neighbors26: result.size() !=0 " << std::endl;
	}
	for (int i = 0; i < 26; i++) {
		if ((curPos + movement26[i]).x >= 0 && (curPos + movement26[i]).y >= 0 && (curPos + movement26[i]).z >= 0)
			result.push_back(curPos + movement26[i]);
	}
}

void myUtils::neighbors26(const glm::vec3 curPos, glm::vec3 resolution, std::vector <int>& result) {
	std::vector <glm::vec3> tmp;
	neighbors26(curPos, tmp);
	for (int i = 0; i < tmp.size(); i++) {
		int ans = tmp[i].x + tmp[i].y * resolution.x * resolution.z + tmp[i].z * resolution.x;
		if (ans < resolution.y * resolution.x * resolution.z) {
			//std::cout << "intput: " << ans << std::endl;
			//std::cout << "intput: " << tmp[i].x << " " << tmp[i].y << " " << tmp[i].z << " index: " << ans << std::endl;
			//int iy = ans / (resolution.x * resolution.z);
			//int iz = (ans - iy * resolution.x * resolution.z) / (resolution.x);
			//int ix = ans - iy * resolution.x * resolution.z - iz * resolution.x;
			//std::cout << "output: " << ix << " " << iy << " " << iz << " ans: " << ans << std::endl;
			//glm::vec3 coor = tumor.boxMin + glm::vec3(ix * tumor.getStep(), iy * tumor.getStep(), iz * tumor.get
			result.push_back(ans);
		}

	}
}

int myUtils::neighbors26(const glm::vec3 curPos, glm::vec3 resolution, const int* markVoxel) {
	int count = 0;
	std::vector <glm::vec3> tmp;
	neighbors26(curPos, tmp);
	for (int i = 0; i < tmp.size(); i++) {
		int ans = tmp[i].x + tmp[i].y * resolution.x * resolution.z + tmp[i].z * resolution.x;
		if (ans < resolution.y * resolution.x * resolution.z) {
			if (markVoxel[ans] == 1) {
				count++;
			}
		}
	}
	return count;
}

/*void myUtils::delaunay(const std::vector<glm::vec3> & vertices, const std::vector<glm::vec3>& voxels) {
	// 计算包围盒
	std::vector<glm::vec3> tmpVertices;
	tmpVertices.assign(vertices.begin(), vertices.end());
	tmpVertices.insert(vertices.end(), voxels.begin(), voxels.end());
	glm::vec4 boundingbox = getBoundingBox(tmpVertices);

	// 计算超级三角形
	glm::vec2 range = glm::vec2(boundingbox.z - boundingbox.x, boundingbox.w - boundingbox.y);
	// 上方顶点:
	glm::vec2 ver1 = glm::vec2((boundingbox.x + boundingbox.z)/2.0f, boundingbox.w + range.y+1.0f);
	// 左下角顶点
	glm::vec2 ver2 = glm::vec2(boundingbox.x - range.x / 2.0f - 1.0f, boundingbox.y - 1.0f);
	glm::vec2 ver3 = glm::vec2(boundingbox.z + range.x / 2.0f + 1.0f, boundingbox.y - 1.0f);
	// 将超级三角形加入顶点数组
	tmpVertices.insert(tmpVertices.begin(), glm::vec3(ver3, -10.0f));
	tmpVertices.insert(tmpVertices.begin(), glm::vec3(ver2, -10.0f));
	tmpVertices.insert(tmpVertices.begin(), glm::vec3(ver1, -10.0f));
	std::vector<Triangle> tmpTriangles;
	Triangle superTri(ver1, ver2, ver3);
	tmpTriangles.push_back(superTri);
	// 超级三角形外接圆
	Circumcircle superCircumcircle = getCircumcircle(superTri);
	std::vector<glm::vec2> edgeIndexPair;

	std::vector<glm::vec2> tmpResult = getEdges(0, 1, 2);
	edgeIndexPair.insert(edgeIndexPair.end(), tmpResult.begin(), tmpResult.end());
	// 遍历顶点
	for (int i = 3; i < tmpVertices.size(); i++) {
		if (glm::distance(glm::vec2(), superCircumcircle.center) < superCircumcircle.radius) { 
			// 点在外接圆内，将三角形的三个边保存到边缘数组中
			std::vector<glm::vec2> tmpResult = getEdges(i-3, i-2, i-1);
			edgeIndexPair.insert(edgeIndexPair.end(), tmpResult.begin(), tmpResult.end());
			// tmpTriangles中删除
			tmpTriangles.erase(tmpTriangles.begin() + i - 3);
			
			// 加入该点与其他点的edge
			edgeIndexPair.push_back(glm::vec2(i, i-3));
			edgeIndexPair.push_back(glm::vec2(i, i - 2));
			edgeIndexPair.push_back(glm::vec2(i, i - 1));

			// 将新组成的三个三角形加入tmpTriangles
			tmpTriangles.push_back(Triangle(tmpVertices[i - 2], tmpVertices[i - 1], tmpVertices[i]));
			tmpTriangles.push_back(Triangle(tmpVertices[i - 3], tmpVertices[i - 2], tmpVertices[i]));
			tmpTriangles.push_back(Triangle(tmpVertices[i - 3], tmpVertices[i - 1], tmpVertices[i]));


		}
	}
}

// 获得三角形三个边的排列组合: 逆时针
std::vector<glm::vec2> myUtils::getEdges(const int & index1, const int& index2, const int& index3 ) {
	std::vector<glm::vec2> ans;
	ans.push_back(glm::vec2(index2, index1));
	ans.push_back(glm::vec2(index3, index2));
	ans.push_back(glm::vec2(index1, index3));
	return ans;
}


Circumcircle myUtils::getCircumcircle(const Triangle& tmp) {
	Circumcircle circumCircle;
	float a = 2 * (tmp.p2.x - tmp.p1.x);
	float b = 2 * (tmp.p2.y - tmp.p1.y);
	float c = tmp.p2.x * tmp.p2.x + tmp.p2.y * tmp.p2.y - tmp.p1.x * tmp.p1.x - tmp.p1.y * tmp.p1.y;
	float d = 2 * (tmp.p3.x - tmp.p2.x);
	float e = 2 * (tmp.p3.y - tmp.p2.y);
	float f = tmp.p3.x * tmp.p3.x + tmp.p3.y * tmp.p3.y - tmp.p2.x * tmp.p2.x - tmp.p2.y * tmp.p2.y;
	float x = (b * f - e * c) / (b * d - e * a);
	float y = (d * c - a * f) / (b * d - e * a);
	float r = (float)sqrt((double)((x - tmp.p1.x) * (x - tmp.p1.x) + (y - tmp.p1.y) * (y - tmp.p1.y)));
	circumCircle.radius = r;
	circumCircle.center = glm::vec2(x,y);
	return circumCircle;
}*/


void myUtils::generateMesh(const std::vector<glm::vec2>& combinedPoints, Delaunay& mesh) {

	glm::vec4 boundingbox = getBoundingBox(combinedPoints);
	
	glm::vec2 basicPoints[4] = {
		glm::vec2(boundingbox.x, boundingbox.y), // upperLeft
		glm::vec2(boundingbox.z, boundingbox.y), //upperRight
		glm::vec2(boundingbox.x, boundingbox.w), // lowerLeft
		glm::vec2(boundingbox.z, boundingbox.w) // lowerRight
	};
	mesh.initialize(boundingbox, combinedPoints);
	/*for (int i = 0; i < 7; i++) {
		mesh.addPoint(combinedPoints[i].x, combinedPoints[i].y);
	}*/
}

void myUtils::generateMesh(const std::vector<glm::vec2>& vertices, const std::vector<glm::vec2>& voxels, Delaunay& mesh) {

	/*std::vector<glm::vec2> tmpVertices; // 将两个点集合并
	tmpVertices.assign(vertices.begin(), vertices.end());
	tmpVertices.insert(tmpVertices.end()-1, voxels.begin(), voxels.end());
	glm::vec4 boundingbox = getBoundingBox(tmpVertices);
	glm::vec2 basicPoints[4] = {
		glm::vec2(boundingbox.x, boundingbox.y), // upperLeft
		glm::vec2(boundingbox.z,boundingbox.y), //upperRight
		glm::vec2(boundingbox.x, boundingbox.w), // lowerLeft
		glm::vec2(boundingbox.z,boundingbox.w) // lowerRight

	};
	mesh.initialize(basicPoints);
	for (int i = 0; i < 2; i++) {
		mesh.addPoint(tmpVertices[i].x, tmpVertices[i].y);
	}*/
}

glm::vec4 myUtils::getBoundingBox(const std::vector<glm::vec2>& array) {
	GLfloat xMin = 10000.0f, xMax = -10000.0f, yMin = 10000.0f, yMax = -10000.0f;
	for (int i = 0; i < array.size(); i++) {
		if (array[i].x > xMax) {
			xMax = array[i].x;
		}
		if (array[i].x < xMin) {
			xMin = array[i].x;
		}
		if (array[i].y > yMax) {
			yMax = array[i].y;
		}
		if (array[i].y < yMin) {
			yMin = array[i].y;
		}
	}
	glm::vec4 boundingBox(xMin, yMin, xMax, yMax);
	return boundingBox;
}