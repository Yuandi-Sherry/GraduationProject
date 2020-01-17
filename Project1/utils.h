#ifndef _MY_UTILS_H_
#define _MY_UTILS_H_
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace myUtils {

	extern std::vector<glm::vec3> movement26;

	void neighbors26(const glm::vec3 curPos, std::vector <glm::vec3>& result);

	void neighbors26(const glm::vec3 curPos, glm::vec3 resolution, std::vector <int>& result);

	int neighbors26(const glm::vec3 curPos, glm::vec3 resolution, const int* markVoxel);

}

#endif // !_MY_UTILS_H_

