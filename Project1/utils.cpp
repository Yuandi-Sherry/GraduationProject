#include "utils.h"
#include <vector>
#include <iostream>


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
