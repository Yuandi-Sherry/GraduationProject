#include "utils.h"
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include "Delaunay.h"
#include <cmath>
#include <queue>

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

std::vector<glm::vec3> myUtils::movement6 = {
		{-1, 0, 0}, { 0,-1, 0},	{ 0, 0,-1},
		{ 0, 0, 1}, { 0, 1, 0}, { 1, 0, 0}
};


void myUtils::neighbors6(const glm::vec3 curPos, glm::vec3 resolution, std::vector <glm::vec3>& result) {
	if (result.size() > 0) {
		std::cout << "in neighbors26: result.size() !=0 " << std::endl;
	}
	for (int i = 0; i < 6; i++) {
		if ((curPos + movement6[i]).x >= 0 && (curPos + movement6[i]).x < resolution.x
			&& (curPos + movement6[i]).y >= 0 && (curPos + movement6[i]).y < resolution.y
			&& (curPos + movement6[i]).z >= 0 && (curPos + movement6[i]).z < resolution.z)
			result.push_back(curPos + movement6[i]);
	}
}

void myUtils::neighbors6(const glm::vec3 curPos, glm::vec3 resolution, std::vector <int>& result) {
	std::vector <glm::vec3> tmp;
	neighbors6(curPos, resolution, tmp);
	for (int i = 0; i < tmp.size(); i++) {
		int ans = tmp[i].x + tmp[i].y * resolution.x * resolution.z + tmp[i].z * resolution.x;
		if (ans < resolution.y * resolution.x * resolution.z) {
			result.push_back(ans);
		}

	}
}

void myUtils::fillInNeighbors6(const glm::vec3 curPos, glm::vec3 resolution, const int* markVoxel, std::vector <int>& result) {
	std::vector <glm::vec3> tmp;
	neighbors6(curPos,resolution, tmp);
	for (int i = 0; i < tmp.size(); i++) {
		int ans = tmp[i].x + tmp[i].y * resolution.x * resolution.z + tmp[i].z * resolution.x;
		if (markVoxel[ans] == 1) {
			result.push_back(ans);
		}
	}
}

void myUtils::blankInNeighbors6(const glm::vec3 curPos, glm::vec3 resolution, const int* markVoxel, std::vector <int>& result) {
	std::vector <glm::vec3> tmp;
	neighbors6(curPos,resolution, tmp);
	for (int i = 0; i < tmp.size(); i++) {
		int ans = tmp[i].x + tmp[i].y * resolution.x * resolution.z + tmp[i].z * resolution.x;
		if (markVoxel[ans] == 0) {
			result.push_back(ans);
		}
	}
}


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
			result.push_back(ans);
		}
	}
}

void myUtils::fillInNeighbors26(const glm::vec3 curPos, glm::vec3 resolution, const int* markVoxel, std::vector <int>& result) {
	std::vector <glm::vec3> tmp;
	neighbors26(curPos, tmp);
	for (int i = 0; i < tmp.size(); i++) {
		int ans = tmp[i].x + tmp[i].y * resolution.x * resolution.z + tmp[i].z * resolution.x;
		if (markVoxel[ans] == 1) {
			result.push_back(ans);
		}
	}
}

void myUtils::blankInNeighbors26(const glm::vec3 curPos, glm::vec3 resolution, const int* markVoxel, std::vector <int>& result) {
	std::vector <glm::vec3> tmp;
	neighbors26(curPos, tmp);
	for (int i = 0; i < tmp.size(); i++) {
		int ans = tmp[i].x + tmp[i].y * resolution.x * resolution.z + tmp[i].z * resolution.x;
		if (markVoxel[ans] == 0 || markVoxel[ans] == -1) {
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
// 用于深度优先搜索，如果返回值为true，说明26邻域有内容,DFSindices最初存起始点在markVoxel的下标
void myUtils::BFS(const glm::vec3& resolution, int* markVoxel, std::queue<int> & DFSindices, const int& length, const int& threshold) {
	// 遍历数组
	while (!DFSindices.empty()) {
		
		if (markVoxel[DFSindices.front()]!=0) {
			DFSindices.pop();
			continue;
		}
		
		//std::cout << DFSindices.front() << std::endl;
		std::vector <int> result;
		int iy = DFSindices.front() / (resolution.x * resolution.z);
		int iz = (DFSindices.front() - iy * resolution.x * resolution.z) / (resolution.x);
		int ix = DFSindices.front() - iy * resolution.x * resolution.z - iz * resolution.x;
		glm::vec3 curPos = glm::vec3(ix, iy, iz);
		// 获得26邻域中未mark的下标
		blankInNeighbors6(curPos, resolution, markVoxel, result);	
		// 则标记为1
		markVoxel[DFSindices.front()] = 1; // 后来标记的记为2
		// 如果得到的26邻域空白的数大于threshold，将邻域加入队列
		if (result.size() > threshold) { 
			for (int j = 0; j < result.size(); j++) {
				DFSindices.push(result[j]);
			}	
		}
		DFSindices.pop();
		// 输出
		//for (int y = 0; y < resolution.y; y++) {
			/*for (int z = 0; z < resolution.z; z++) {
				for (int x = 0; x < resolution.x; x++) {
					if ((int)(x + iy * resolution.x * resolution.z + z * resolution.x) == length / 2) {
						std::cout << "⚪";
					}
					else if (markVoxel[(int)(x + iy * resolution.x * resolution.z + z * resolution.x)] == 1) {
						std::cout << "■";
					}
					else if (markVoxel[(int)(x + iy * resolution.x * resolution.z + z * resolution.x)] == 2) {
						std::cout << "□";
					}
					else {
						std::cout << "  ";
					}
				}
				std::cout << std::endl;
			}
			std::cout << "----------------------------------------------------------" << std::endl;*/
		//}

	}	
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
	mesh.initialize(boundingbox);
	
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