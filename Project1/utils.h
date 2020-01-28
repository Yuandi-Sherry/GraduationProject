#ifndef _MY_UTILS_H_
#define _MY_UTILS_H_
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <vector>
#include "Delaunay.h"

namespace myUtils {

	extern std::vector<glm::vec3> movement26;

	struct Triangle {
		glm::vec2 p1;
		glm::vec2 p2;
		glm::vec2 p3;
		Triangle(glm::vec2 a, glm::vec2 b, glm::vec2 c) :p1(a), p2(b), p3(c) {};
	};
	// Õ‚Ω”‘≤
	struct Circumcircle {
		glm::vec2 center;
		GLfloat radius;
	};

	void neighbors26(const glm::vec3 curPos, std::vector <glm::vec3>& result);

	void neighbors26(const glm::vec3 curPos, glm::vec3 resolution, std::vector <int>& result);

	int neighbors26(const glm::vec3 curPos, glm::vec3 resolution, const int* markVoxel);

	void delaunay(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& voxels);

	glm::vec4 getBoundingBox(const std::vector<glm::vec2>& array);

	Circumcircle getCircumcircle(const Triangle & tmp);

	std::vector<glm::vec2> getEdges(const int& index1, const int& index2, const int& index3);

	void generateMesh(const std::vector<glm::vec2>& vertices, const std::vector<glm::vec2>& voxels, Delaunay& mesh);

	void generateMesh(const std::vector<glm::vec2>& vertices, Delaunay& mesh, const int& size);
}

#endif // !_MY_UTILS_H_
