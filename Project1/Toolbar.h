#ifndef _TOOL_BAR_
#define _TOOL_BAR_

class Toolbar
{
public:
	Toolbar();
	~Toolbar();
	bool ruler;
	bool cutface;
	void setVertex(glm::vec3 firstVertex) {
		if (currentIndex == -1) {
			std::cout << "Already select 2 points" << std::endl;
			return;
		}
		rulerVertices[currentIndex] = firstVertex;
		if (currentIndex == 0)
			currentIndex = 1;
		else if (currentIndex == 1)
			currentIndex = -1;

	}
	GLfloat getDistance() {
		return sqrt(pow(rulerVertices[0].x - rulerVertices[1].x, 2) + pow(rulerVertices[0].y - rulerVertices[1].y, 2) + pow(rulerVertices[0].z - rulerVertices[1].z, 2));
	}
	void reset() {
		currentIndex = 0;

	}
private:
	glm::vec3 rulerVertices[2];
	int currentIndex; // 0, 1
};

Toolbar::Toolbar()
{
	ruler = false;
	cutface = false;
	currentIndex = 0;
}

Toolbar::~Toolbar()
{
}
#endif