#include "Area.h"
#include "Plane.h"
// imgui
#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <string>
#include <set>
#include "MyCylinder.h"
#include "MyLine.h"
#include <map>

#include "utils.h"
/*----------------------GENERAL----------------------*/
extern const unsigned int SCR_WIDTH, SCR_HEIGHT;
float light_near_plane = 0.0f, light_far_plane = 800.0f, light_left_plane = -150.0f, light_right_plane = 150.0f, light_top_plane = 150.0f, light_bottom_plane = -150.0f;
Area::Area()
{
	modelsID.clear();
	transformMat = glm::rotate( glm::mat4(1.0f), glm::radians(-90.0f),glm::vec3(1.0f, 0.0f, 0.0f));
}

Area::~Area()
{
}

void Area::init() {
	camera.setPosition(cameraPos);camera.setOrthology();
	// ruler
	ruler.initVertexObject();
	mySphere.initVertexObject();
	myCube.initVertexObject();
	lines.initVertexObject();
	// init 
	lightProjection = glm::ortho(light_left_plane, light_right_plane, light_bottom_plane, light_top_plane, light_near_plane, light_far_plane);
	lightView = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;

	initDepthBuffer();	
}

void Area::initDepthBuffer() {
	// create depth texture
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Area::setModelsID(const std::vector<GLint>& modelsIDs) {
	this->modelsID.assign(modelsIDs.begin(), modelsIDs.end());
}

void Area::tackleRemoveTumor(Shader& shader, Shader& shadowShader) {
	if (removeMode == 0) { // normal
		shader.use();
		drawModels(shader, shadowShader);
	}
	else if (removeMode == 1) { // 获取鼠标位置转换
		drawModels(shader, shadowShader);
		shader.use();
		glm::mat4 model = glm::translate(glm::mat4(1.0f), removePos);
		model = glm::translate(model, -BaseModel::modelCenter);
		model = glm::scale(model, glm::vec3(cutRadius, cutRadius, cutRadius));
		shader.setInt("type", 1);
		shader.setMat4("model", transformMat * model);
		shader.setVec3("color", glm::vec3(235.0f / 255, 122.0f / 255, 119.0f / 255));
		mySphere.draw();
		shader.setInt("type", 0);
	}
}

void Area::setRemovePos(glm::vec3 pos) {
	// local coordinate
	removePos = glm::inverse(glm::translate(glm::mat4(1.0f),-BaseModel::modelCenter)) * glm::inverse(transformMat) * glm::vec4(pos, 1.0f);
	removeMode = 1;
}

glm::vec3 random3(glm::vec3 p)
{
	p = glm::vec3(glm::dot(p, glm::vec3(127.1, 311.7, 74.7)),
		glm::dot(p, glm::vec3(269.5, 183.3, 246.1)),
		glm::dot(p, glm::vec3(113.5, 271.9, 124.6)));

	return -1.0f + 2.0f * (sin(p) * 43758.5453123f - floor(sin(p) * 43758.5453123f));
}

template<typename T>
T mix(const T & x, const T & y, float & a) {
	return x*(1-a) + y*a;
}
float noise3d(glm::vec3 p)
{
	glm::vec3 i = floor(p);
	glm::vec3 f = fract(p);

	glm::vec3 u = f * f * (3.0f - 2.0f * f);
	/*mix(glm::dot(random3(i + glm::vec3(0.0, 0.0, 0.0)), f - glm::vec3(0.0, 0.0, 0.0)),
		glm::dot(random3(i + glm::vec3(1.0, 0.0, 0.0)), f - glm::vec3(1.0, 0.0, 0.0)), u.x);*/
	return mix(mix(mix(glm::dot(random3(i + glm::vec3(0.0, 0.0, 0.0)), f - glm::vec3(0.0, 0.0, 0.0)),
		glm::dot(random3(i + glm::vec3(1.0, 0.0, 0.0)), f - glm::vec3(1.0, 0.0, 0.0)), u.x),
		mix<float>(glm::dot(random3(i + glm::vec3(0.0, 1.0, 0.0)), f - glm::vec3(0.0, 1.0, 0.0)),
			glm::dot(random3(i + glm::vec3(1.0, 1.0, 0.0)), f - glm::vec3(1.0, 1.0, 0.0)), u.x), u.y),
		mix(mix<float>(glm::dot(random3(i + glm::vec3(0.0, 0.0, 1.0)), f - glm::vec3(0.0, 0.0, 1.0)),
			glm::dot(random3(i + glm::vec3(1.0, 0.0, 1.0)), f - glm::vec3(1.0, 0.0, 1.0)), u.x),
			mix<float>(glm::dot(random3(i + glm::vec3(0.0, 1.0, 1.0)), f - glm::vec3(0.0, 1.0, 1.0)),
				glm::dot(random3(i + glm::vec3(1.0, 1.0, 1.0)), f - glm::vec3(1.0, 1.0, 1.0)), u.x), u.y), u.z);
}
#define ADD_RANDOM true
#define RAND_COOR 0.3f
int countSubdivision = 0;
void subdivide(const int& verticesIndex, const vector<int>& verIndices, vector<GLfloat>& vertices,
	vector<GLint>& indices, const int& cutRadius, const glm::vec3& removePos, const int& level, map<pair<int, int>, int>& hashMap,
	const int& edgeBegin, const int& edgeEnd, set<int> &edges) {
	
	// 判断法向量方向， 是否需要修改verIndices的顺序
	vector<glm::vec3> points = {
		glm::vec3(vertices[verIndices[0] * 6], vertices[verIndices[0] * 6 + 1], vertices[verIndices[0] * 6 + 2]),
		glm::vec3(vertices[verIndices[1] * 6], vertices[verIndices[1] * 6 + 1], vertices[verIndices[1] * 6 + 2]),
		glm::vec3(vertices[verIndices[2] * 6], vertices[verIndices[2] * 6 + 1], vertices[verIndices[2] * 6 + 2])
	};

	glm::vec3 normal = glm::cross(points[0] - points[1], points[1] - points[2]);
	
	vector<int> verIndicesIn;
	verIndicesIn.assign(verIndices.begin(), verIndices.end());
	// 如果为边缘，则判断法向量和边缘顶点法向量同方向
	int isEdge = 0;
	for (int i = 0; i < 3; i++) {
		if (verIndices[i] >= edgeBegin && verIndices[i] < edgeEnd || edges.find(verIndices[i])!= edges.end()) {
			isEdge++;
			if (isEdge > 1) {
				glm::vec3 vertexNormal = glm::vec3(vertices[verIndices[i] * 6 + 3], vertices[verIndices[i] * 6 + 4], vertices[verIndices[i] * 6 + 5]);
				if (glm::dot(normal, vertexNormal) < 0) {
					int tmp = verIndicesIn[0];
					verIndicesIn[0] = verIndicesIn[1];
					verIndicesIn[1] = tmp;
					points = {
					glm::vec3(vertices[verIndicesIn[0] * 6], vertices[verIndicesIn[0] * 6 + 1], vertices[verIndicesIn[0] * 6 + 2]),
					glm::vec3(vertices[verIndicesIn[1] * 6], vertices[verIndicesIn[1] * 6 + 1], vertices[verIndicesIn[1] * 6 + 2]),
					glm::vec3(vertices[verIndicesIn[2] * 6], vertices[verIndicesIn[2] * 6 + 1], vertices[verIndicesIn[2] * 6 + 2])
					};
					normal = glm::cross(points[0] - points[1], points[1] - points[2]);
				}
				break;
			}
				
		}
	}
	if (isEdge <= 1) {
		if (glm::dot(normal, removePos - points[0]) < 0) {
			int tmp = verIndicesIn[0];
			verIndicesIn[0] = verIndicesIn[1];
			verIndicesIn[1] = tmp;
			points = {
			glm::vec3(vertices[verIndicesIn[0] * 6], vertices[verIndicesIn[0] * 6 + 1], vertices[verIndicesIn[0] * 6 + 2]),
			glm::vec3(vertices[verIndicesIn[1] * 6], vertices[verIndicesIn[1] * 6 + 1], vertices[verIndicesIn[1] * 6 + 2]),
			glm::vec3(vertices[verIndicesIn[2] * 6], vertices[verIndicesIn[2] * 6 + 1], vertices[verIndicesIn[2] * 6 + 2])
			};
			normal = glm::cross(points[0] - points[1], points[1] - points[2]);
		}
	}
	
	
	if (level < 2) {
		vector<glm::vec3> newPoints = {
			(points[0] + points[1]) / 2.0f,
			(points[1] + points[2]) / 2.0f,
			(points[2] + points[0]) / 2.0f
		};
		// 将incenter放在球体表面
		int newIndices[3];
		for (int i = 0; i < 3; i++) {
			pair<int, int> orderedPair = verIndicesIn[i] < verIndicesIn[(i + 1) % 3] ? make_pair(verIndicesIn[i], verIndicesIn[(i + 1) % 3]) :
				make_pair(verIndicesIn[(i + 1) % 3], verIndicesIn[i]);
			map<pair<int, int>, int>::iterator it = hashMap.find(orderedPair);
			if (it != hashMap.end()) {
				newIndices[i] = it->second;
			}
			else {
				GLfloat distance = glm::distance(newPoints[i], removePos);
				if ((verIndicesIn[i] >= edgeBegin && verIndicesIn[i] < edgeEnd || edges.find(verIndicesIn[i]) != edges.end())
					&& (verIndicesIn[(i + 1) % 3] >= edgeBegin && verIndicesIn[(i + 1) % 3] < edgeEnd || edges.find(verIndicesIn[(i + 1) % 3]) != edges.end())) {
					edges.insert(vertices.size() / 6);
				}
				else {
					newPoints[i] = newPoints[i] + (removePos - newPoints[i]) * (distance - cutRadius) / distance;
					if (ADD_RANDOM) {
						newPoints[i] += RAND_COOR * noise3d(newPoints[i]) * glm::normalize(removePos - newPoints[i]);
					}
				}
				
				newIndices[i] = vertices.size() / 6;
				hashMap[orderedPair] = newIndices[i];
				if (hashMap.find(orderedPair) == hashMap.end()) {
					countSubdivision++;
				}
				// 顶点坐标
				vertices.push_back(newPoints[i].x);
				vertices.push_back(newPoints[i].y);
				vertices.push_back(newPoints[i].z);
				// 法向量占位
				vertices.push_back(0);
				vertices.push_back(0);
				vertices.push_back(0);
			}
		}
		// 当前计算插入的三角形是否需要再分

		for (int i = 0; i < 3; i++) {
			subdivide(vertices.size() / 6, { verIndicesIn[i], newIndices[i],newIndices[(i + 2) % 3] }, vertices, indices, cutRadius, removePos, level+1, hashMap, edgeBegin, edgeEnd, edges);
		}
		subdivide(vertices.size() / 6, { newIndices[0], newIndices[1], newIndices[2] }, vertices, indices, cutRadius, removePos, level+1, hashMap, edgeBegin, edgeEnd, edges);
	}
	else {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				vertices[verIndicesIn[i] * 6 + 3 + j] += normal[j];
			}
		}
		indices.insert(indices.end(), verIndicesIn.begin(), verIndicesIn.end());
	}
}

void Area::updateCutVertices(BaseModel * tumor) {
	voxel2D.clear();
	voxel3D.clear();
	vertex2D.clear();
	combinedPoints.clear();
	glm::vec3 resolution = (*tumor).getResolution();
	// 记录要删除的网格顶点
	std::vector<GLfloat> tmpVertices(*(*tumor).getVertices());
	std::vector<int> tmpIndices(*(*tumor).getIndices());
	std::vector<int> tobeDelete;
	std::map<int, int> updateIndices;
	for (int i = 0; i < tmpVertices.size() / 6; i++) {
		glm::vec3 curVer = glm::vec3(tmpVertices[i * 6], tmpVertices[i * 6 + 1], tmpVertices[i * 6 + 2]);
		GLfloat dis = glm::distance(curVer, removePos);
		if (dis < cutRadius) {
			// 记录需要删除的点在vertices的index
			tobeDelete.push_back(i);
		}
	}

	// 计算前后indices的映射关系
	int* newIndices = new int[tmpVertices.size() / 6];
	for (int i = 0; i < tmpVertices.size() / 6; i++) {
		newIndices[i] = i;
	}
	for (int i = 0, j = 0; i < tmpVertices.size() / 6 && j < tobeDelete.size(); i++) {
		if (i == tobeDelete[j]) {
			j++;
			// 之后的下标更新
			newIndices[i] = -1;
			for (int k = i + 1; k < tmpVertices.size() / 6; k++) {
				newIndices[k] -= 1;
			}
		}
	}

	// 计算边缘顶点构成的平面的平均法向量
	glm::vec3 averNormal = glm::vec3(0, 0, 0); //边缘顶点的平均法向量
	glm::vec3 averCenter = glm::vec3(0, 0, 0); //边缘顶点的平均法向量
	std::set<int> edgeIndices; // 记录边缘顶点在【更新后】vertices中的下标
	for (int i = 0; i < tmpIndices.size() / 3; i++) {
		//记录和移除点相连的三角形
		if (newIndices[tmpIndices[i * 3]] == -1 || newIndices[tmpIndices[i * 3 + 1]] == -1 || newIndices[tmpIndices[i * 3 + 2]] == -1) {
			// 对是-1的检测之前是否加入，其他的加入
			for (int j = 0; j < 3; j++) {
				if (newIndices[tmpIndices[i * 3 + j]] != -1) {
					// 直接放入映射后的index
					edgeIndices.insert(newIndices[tmpIndices[i * 3 + j]]);
					averCenter += glm::vec3(tmpVertices[tmpIndices[i * 3 + j] * 6], tmpVertices[tmpIndices[i * 3 + j] * 6 + 1], tmpVertices[tmpIndices[i * 3 + j] * 6 + 2]);
					averNormal += glm::vec3(tmpVertices[tmpIndices[i * 3 + j] * 6 + 3], tmpVertices[tmpIndices[i * 3 + j] * 6 + 4], tmpVertices[tmpIndices[i * 3 + j] * 6 + 5]);
				}
			}
		}
	}
	// 更新indices
	for (int i = 0; i < tmpIndices.size() / 3; ) {
		// 删除和移除点相连的三角形
		if (newIndices[tmpIndices[i * 3]] == -1 || newIndices[tmpIndices[i * 3 + 1]] == -1 || newIndices[tmpIndices[i * 3 + 2]] == -1) {
			// 删除
			tmpIndices.erase(tmpIndices.begin() + i * 3 + 2);
			tmpIndices.erase(tmpIndices.begin() + i * 3 + 1);
			tmpIndices.erase(tmpIndices.begin() + i * 3);
		}
		else {
			tmpIndices[i * 3] = newIndices[tmpIndices[i * 3]];
			tmpIndices[i * 3 + 1] = newIndices[tmpIndices[i * 3 + 1]];
			tmpIndices[i * 3 + 2] = newIndices[tmpIndices[i * 3 + 2]];
			i++;
		}
	}
	delete[] newIndices;
	// 删除对应的vertices
	for (int i = 0; i < tobeDelete.size(); i++) {
		for (int j = 5; j >=0; j--) {
			tmpVertices.erase(tmpVertices.begin() + (tobeDelete[i] - i) * 6 + j);
		}
	}
	int oldVerticesSize = tmpVertices.size() / 6;
	averCenter /= edgeIndices.size();
	GLfloat averRadius = 0;
	for (std::set<int> ::iterator it = edgeIndices.begin(); it != edgeIndices.end(); it++) {
		averRadius += glm::distance(averCenter, glm::vec3(tmpVertices[(*it) * 6], tmpVertices[(*it) * 6 + 1], tmpVertices[(*it) * 6 + 2]));
	}
	averRadius /= edgeIndices.size();
	// 计算离averNormal最近的球面坐标作为二维空间原点
	int originIndex = -1;
	GLfloat maxDot = -1;

	// 局部坐标系位置
	std::vector<glm::vec3>* voxelsPos = (*tumor).getVoxelsPos();
	// 体素空间的下标
	std::vector<glm::vec3>* voxelsIndex = (*tumor).getVoxelsIndex();
	std::set<int> edgeVoxelsIndex;
	// 标记边缘体素
	for (int i = 0; i < voxelsPos->size(); ) {
		// 判断是否在球里面
		GLfloat dis = glm::distance((*voxelsPos)[i], removePos);
		if (dis < cutRadius) {
			// 移除局部坐标
			voxelsPos->erase(voxelsPos->begin() + i);
			// 计算标记数组的的下标
			int tmpIndex = (voxelsIndex->begin() + i)->y * resolution.x * resolution.z + (voxelsIndex->begin() + i)->z * resolution.x + (voxelsIndex->begin() + i)->x;
			// 更新标记数组
			(*tumor).markVoxel[tmpIndex] = 0;
			// 26邻域
			std::vector<int> resultIndex;
			myUtils::neighbors26(*(voxelsIndex->begin() + i), resolution, resultIndex);
			// 标记数组中为1且在result中的变为2，否则仍然为1
			for (int j = 0; j < resultIndex.size(); j++) {
				if ((*tumor).markVoxel[resultIndex[j]] == 1) {
					// 将对应位置标记为2
					(*tumor).markVoxel[resultIndex[j]] = 2;
					// 记录位置坐标
					edgeVoxelsIndex.insert(resultIndex[j]);
				}
			}
			voxelsIndex->erase(voxelsIndex->begin() + i);
		}
		else {
			i++;
		}
	}
	// 在体素中找到二维空间原点
	glm::vec3 origin = glm::vec3(-10000.0f, -10000.0f, -10000.0f);
	glm::vec3 originNormal;
	glm::vec3 originRight; // 任意一个法向量的垂直方向作为x+
	glm::vec3 originUp;
	for (int i = 0; i < static_cast<int>(resolution.x * resolution.y * resolution.z); i++) {
		if ((*tumor).markVoxel[i] == 2) {
			int iy = i / (resolution.x * resolution.z);
			int iz = (i - iy * resolution.x * resolution.z) / (resolution.x);
			int ix = i - iy * resolution.x * resolution.z - iz * resolution.x;
			glm::vec3 coor = (*tumor).boxMin + glm::vec3(ix * (*tumor).getStep(), iy * (*tumor).getStep(), iz * (*tumor).getStep());
			glm::vec3 tmpNormal = glm::normalize(removePos - coor);
			// 确定原点
			if (glm::dot(tmpNormal, averNormal) > maxDot) {
				maxDot = glm::dot(tmpNormal, averNormal);
				originIndex = i;
			}
		}
	}
	std::vector<int> cutIndices;
	std::vector<GLfloat> cutVertices; // 还有法向量
	// 在体素中找到二维空间原点
	if (originIndex != -1) {
		int originY = originIndex / (resolution.x * resolution.z);
		int originZ = (originIndex - originY * resolution.x * resolution.z) / (resolution.x);
		int originX = originIndex - originY * resolution.x * resolution.z - originZ * resolution.x;
		origin = (*tumor).boxMin + glm::vec3(originX * (*tumor).getStep(), originY * (*tumor).getStep(), originZ * (*tumor).getStep());
		GLfloat distance = glm::distance(origin, removePos);
		origin = origin + (removePos - origin) * (distance - cutRadius) / distance;
		//cout << "ruler : " << (distance - cutRadius) / distance << endl;
		// 法向量
		originNormal = glm::normalize(removePos - origin);
		originRight = glm::normalize(glm::cross(originNormal, glm::vec3(0, 0, 1)));
		originUp = glm::normalize(glm::cross(originNormal, originRight));
		voxel2D.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		cutVertices.push_back(origin.x);
		cutVertices.push_back(origin.y);
		cutVertices.push_back(origin.z);
		// normal
		glm::vec3 normal = removePos - origin;
		cutVertices.push_back(normal.x);
		cutVertices.push_back(normal.y);
		cutVertices.push_back(normal.z);
		(*tumor).markVoxel[originIndex] = 0; // 这个位置就不存在了，而改为tumor模型上的位置了
	}
	else {
		std::cout << "FAIL TO FIND ORIGIN" << std::endl;
		return;
	}
	
	// 计算边缘体素的二维坐标
	for (int i = 0; i < static_cast<int>(resolution.x * resolution.y * resolution.z); i++) {
		if (i != originIndex && (*tumor).markVoxel[i] == 2) {
			int iy = i / (resolution.x * resolution.z);
			int iz = (i - iy * resolution.x * resolution.z) / (resolution.x);
			int ix = i - iy * resolution.x * resolution.z - iz * resolution.x;
			glm::vec3 coor = (*tumor).boxMin + glm::vec3(ix * (*tumor).getStep(), iy * (*tumor).getStep(), iz * (*tumor).getStep());
			GLfloat distance = glm::distance(coor, removePos);
			glm::vec3 newCoor = coor + (removePos - coor) * (distance - cutRadius) / distance;
			// 计算平面上的坐标: 弧长
			double rho = acos(glm::dot(originNormal, glm::normalize(newCoor - origin))) * cutRadius * glm::distance(newCoor, origin) / 10.0f;
			// 和纵轴夹角（y正半轴
			glm::vec3 OP = newCoor - origin;
			glm::vec3 OH = glm::dot(OP, originNormal) * originNormal;
			glm::vec3 OQ = OP - OH;
			GLfloat theta = acos(glm::dot(glm::normalize(OQ), originRight));
			if (glm::dot(glm::normalize(OQ), originUp) < 0) {
				theta = -theta;
			}
			// voxel3D.push_back(coor);
			voxel2D.push_back(glm::vec3(rho * cos(theta), rho * sin(theta), 200.0f));
			// 球体里外改一改
			if (ADD_RANDOM) {
				newCoor += RAND_COOR * noise3d(newCoor) * glm::normalize(removePos - newCoor);
			}
			
			cutVertices.push_back(newCoor.x);
			cutVertices.push_back(newCoor.y);
			cutVertices.push_back(newCoor.z);
			// normal
			//glm::vec3 normal = removePos - newCoor;
			cutVertices.push_back(0.0f);
			cutVertices.push_back(0.0f);
			cutVertices.push_back(0.0f);
			// 恢复1
			(*tumor).markVoxel[i] = 1;
		}
	}

	int edgeBegin = cutVertices.size() / 6;

	// 计算边缘顶点的二维空间坐标
	std::vector<int> orderedIndices;
	for (std::set<int>::iterator it = edgeIndices.begin(); it != edgeIndices.end(); it++) {
		glm::vec3 coor = glm::vec3(tmpVertices[(*it) * 6], tmpVertices[(*it) * 6 + 1], tmpVertices[(*it) * 6 + 2]);
		GLfloat distance = glm::distance(coor, removePos);
		glm::vec3 newCoor = coor + (removePos - coor) * (distance - cutRadius) / distance;
		// 计算球面距离
		GLfloat rho = 5 * acos(glm::dot(originNormal, glm::normalize(coor - origin))) * cutRadius * glm::distance(coor, origin) / 10.0f;
		// 计算和纵轴夹角（y正半轴
		glm::vec3 OP = newCoor - origin;
		glm::vec3 OH = glm::dot(OP, originNormal) * originNormal;
		glm::vec3 OQ = OP - OH;
		GLfloat theta = acos(glm::dot(glm::normalize(OQ), originRight));
		if (glm::dot(glm::normalize(OQ), originUp) < 0) {
			theta = -theta;
		}
		vertex2D.push_back(glm::vec2(rho * cos(theta), rho * sin(theta)));
		orderedIndices.push_back(*it);
		// 将原本在tumor边缘的顶点加入cutTumor中
		for (int i = 0; i < 6; i++) {
			cutVertices.push_back(tmpVertices[(*it) * 6+i]);
		}
	}
	int edgeEnd = cutVertices.size() / 6;
	// -------------在二维空间构造三角剖分-----------------
	// 要求voxel在前,vertex在后
	combinedPoints.assign(voxel2D.begin(), voxel2D.end());
	combinedPoints.insert(combinedPoints.end(), vertex2D.begin(), vertex2D.end());
	myUtils::generateMesh(combinedPoints, mesh);
	for (int i = 0; i < combinedPoints.size(); i++) {
		mesh.addPoint(combinedPoints[i]);
	}
	mesh.deleteSuperTriangle();
	/// 更新模型
	(*tumor).setVertices(tmpVertices);
	(*tumor).setIndices(tmpIndices);
	(*tumor).initVertexObject(); // 我不知道这一步为什么要在Models.push的前面
	// 肿瘤切面单独放出
	// vertices: 和combinedPoints顺序一致
	// indices: mesh的order-4即可
	// 先是voxel,再是vertex
	vector<GLfloat> interplotedVer;
	int cutVerticesSize = cutVertices.size() / 6;
	map<pair<int, int>, int> hashMap;
	set<int> edges;
	cout << "edges: " << edgeBegin << " " << edgeEnd << endl;
	for (std::list<Triangle>::iterator it = mesh.triangleVector.begin(); it != mesh.triangleVector.end(); it++) {
		// 三个点都为顶点则去除
		vector<int> verIndices; // 剖分得到的三角形下标
		for (int j = 0; j < 3; j++) {
			verIndices.push_back(it->vertices[j] - 4);
		}
		GLfloat thresArea = 2.0f;	
		//cout << "------------------------------------------" << endl;
		//cout << verIndices[0] << " " << verIndices[1] << " " << verIndices[2] << endl;
		subdivide(cutVertices.size()/6, verIndices, cutVertices, cutIndices, cutRadius, removePos, 0, hashMap, edgeBegin, edgeEnd, edges);
	}
	// 更新cutVertices的法向量
	BaseModel cutTumor(cutVertices, cutIndices, glm::vec3(235.0f/255, 122.0f/255, 119.0f/255));
	cutTumor.initVertexObject();
	// 加入Models
	Area::cutTumor.push_back(cutTumor);
}


void Area::setViewport(GLfloat left, GLfloat bottom, GLfloat width, GLfloat height) {
	viewportPara[0] = left;
	viewportPara[1] = bottom;
	viewportPara[2] = width;
	viewportPara[3] = height;
	camera.setSize(width, height);
}

void Area::updateLightSpaceMatrix() {
	lightProjection = glm::ortho(light_left_plane,light_right_plane, light_bottom_plane, light_top_plane, light_near_plane, light_far_plane);
	lightView = glm::lookAt(camera.Position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
}

void Area::renderDepthBuffer(Shader & shadowShader, std::vector<BaseModel*> & models) {
	shadowShader.use();
	shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model,-BaseModel::modelCenter);
	shadowShader.setMat4("model", transformMat * model);
	for (int i = 0; i < modelsID.size(); i++) {
		models[modelsID[i]]->draw();
	}
	if (modeSelection == RULER || modeSelection == NEAREST_VESSEL) {
		model = glm::translate(glm::mat4(1.0f), ruler.position);
		model = glm::rotate(model, ruler.rotateAngle, glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(ruler.scaleSize, ruler.scaleSize, 1));
		shadowShader.setMat4("model", model);
		ruler.draw();
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	
}

glm::vec4 Area::getViewport() {
	return glm::vec4(viewportPara[0], viewportPara[1], viewportPara[2], viewportPara[3]);
}


void Area::drawModels(Shader & shader, Shader & shadowShader) {
	shadowShader.use();
	shadowShader.setInt("cut", 0);
	renderDepthBuffer(shadowShader, models);
	glViewport(viewportPara[0], viewportPara[1], viewportPara[2], viewportPara[3]);
	
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	shader.setInt("withShadow", 1);
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("lightPos", camera.Position);
	shader.setInt("cut", 0);
	shader.setMat4("projection", camera.getOrthology());
	shader.setMat4("view", camera.GetViewMatrix());
	std::vector<glm::vec3>* tmpVoxelPos = (*models[1]).getVoxelsPos();
	glm::mat4 model(1.0f);
	model = glm::translate(glm::mat4(1.0f),-BaseModel::modelCenter);
	shader.setMat4("model", transformMat * model);
	shader.setInt("withLight", 1);
	for (int i = 0; i < modelsID.size(); i++) {
		shader.setVec3("color", (*models[modelsID[i]]).getColor());	
		(*models[modelsID[i]]).draw();
	}
	// main & tumor
	if (cutTumor.size()> 0 && (modelsID.size() > 1 || modelsID[0] == 1)) {
		shader.setInt("type", 2);
		shader.setVec3("color", cutTumor[0].getColor());
		for (int i = 0; i < cutTumor.size(); i++) {
			cutTumor[i].draw();			
		}
		shader.setInt("type", 0);
	}
	
}


void Area::drawRuler(Shader & textureShader, Shader& shader) {
	// ruler
	textureShader.use();
	textureShader.setVec3("viewPos", camera.Position);
	textureShader.setVec3("lightPos", camera.Position);
	textureShader.setMat4("projection", camera.getOrthology());
	textureShader.setMat4("view", camera.GetViewMatrix());
	//改为从0刻度开始
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, ruler.position);
	model = glm::rotate(model, ruler.rotateAngle, glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(ruler.scaleSize, ruler.scaleSize, 1));
	textureShader.setMat4("model", model);
	ruler.generateTexture();
	ruler.draw();

	// ends
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	shader.setVec3("color", glm::vec3(0.5f, 0.8f, 0.3f));
	shader.setMat4("projection", camera.getOrthology());
	shader.setMat4("view", camera.GetViewMatrix());
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("lightPos", camera.Position);
	shader.setInt("withLight", 1);
	shader.setInt("withShadow", 0);
	model = glm::scale(glm::translate(glm::mat4(1.0f), ruler.ends[0]), glm::vec3(3, 3, 3));
	shader.setMat4("model", model);
	mySphere.draw();
	
	if (modeSelection == NEAREST_VESSEL)
		shader.setVec3("color", glm::vec3(1.0f, 0.4f, 0.4f));
	model = glm::scale(glm::translate(glm::mat4(1.0f), ruler.ends[1]), glm::vec3(3, 3, 3));
	shader.setMat4("model", model);
	mySphere.draw();
}



void Area::displayGUI() {
	ImGui::RadioButton("General", &modeSelection, GENERAL);
	ImGui::RadioButton("Ruler", &modeSelection, RULER);
	ImGui::RadioButton("Nearest Vessel", &modeSelection, NEAREST_VESSEL);
	ImGui::RadioButton("Rmove Tumor", &modeSelection, REMOVE_TUMOR);
	// adjust the radius of removing tumor
	if (modeSelection == REMOVE_TUMOR) {
		ImGui::SliderFloat("remove rumor radius: ", &cutRadius, 0, 20);
	}
}
/*----------------------RULER----------------------*/
void Area::setRulerVertex(const glm::vec3& vertexPosition) {
	glm::vec3 localPos = glm::vec4(vertexPosition, 1.0f);
	if (currentRulerIndex == 0) { // first end
		ruler.ends[currentRulerIndex] = localPos;
		currentRulerIndex = 1;
	}
	else { // second end
		ruler.ends[currentRulerIndex] = localPos;
		currentRulerIndex = 0;
		// calculate ruler attributes
		updateRuler(ruler.ends[0], ruler.ends[1]);
	}
}
#define SCALERULER 18.3
void Area::updateRuler(const glm::vec3& pos1, const glm::vec3& pos2) { // 输入参数为世界坐标
	// calculate ruler attributes
	ruler.setCutFace(BaseModel::frontFace);
	ruler.distance = glm::distance(pos1, pos2);	
	ruler.ends[0] = pos1;
	ruler.ends[1] = pos2;
	ruler.ends[0].z = ruler.CUTFACE;
	ruler.ends[1].z = ruler.CUTFACE;
	GLfloat projLen = glm::distance(ruler.ends[0], ruler.ends[1]);
	ruler.scaleSize = projLen / ruler.distance * SCALERULER;
	ruler.position = (pos1 + pos2) / 2.0f + glm::normalize(pos2-pos1) * (ruler.scaleSize* 14.0f - projLen) / 2.0f;
	ruler.position.z = ruler.CUTFACE;
	ruler.rotateAngle = atan((pos1.y - pos2.y) / (pos1.x - pos2.x));
}

void Area::tackleRuler(Shader& shader, Shader& shadowShader, Shader& textureShader) {
	shader.use();
	drawModels(shader, shadowShader);
	drawRuler(textureShader, shader);
}


/*----------------------NEAREST_VESSEL----------------------*/
void Area::tackleNearestVessel(Shader& shader, Shader& shadowShader, Shader& textureShader) {
	shader.use();
	drawModels(shader, shadowShader);

	if (glm::vec3(-10000.0f, -10000.0f, -10000.0f) != nearestPos) {
		drawRuler(textureShader, shader);
	}
}
void Area::setLocalCoordinate(glm::vec3 worldCoor, BaseModel* vessel) {
	NVLocalPos = glm::inverse(glm::translate(glm::mat4(1.0f),-BaseModel::modelCenter)) * glm::inverse(transformMat) * glm::vec4(worldCoor, 1.0f);
	findNearest(vessel);
}

void Area::findNearest(BaseModel* vessel) {
	GLfloat min = 10000.0f;
	int minIndex = 0;
	for (int i = 0; i < (*vessel).getVertices()->size() / 6; i++) {
		glm::vec3 pos = glm::vec3((*(*vessel).getVertices())[i*6], (*(*vessel).getVertices())[i*6+1], (*(*vessel).getVertices())[i*6+2]);
		GLfloat dis = glm::distance(NVLocalPos, pos);
		if (dis < min) {
			min = dis;
			minIndex = i;
		}
	}
	vesselDistance = min;
	nearestPos = glm::vec3((*(*vessel).getVertices())[minIndex * 6], (*(*vessel).getVertices())[minIndex * 6 + 1], (*(*vessel).getVertices())[minIndex * 6 + 2]);
	glm::mat4 model = glm::translate(glm::mat4(1.0f),-BaseModel::modelCenter);
	model = transformMat * model;
	updateRuler(model * glm::vec4(NVLocalPos, 1.0f), model * glm::vec4(nearestPos, 1.0f));
}



void Area::removeTumor(BaseModel* tumor) {
	updateCutVertices(tumor);
	removeMode = 0;	
}