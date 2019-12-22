#ifndef _RULER_H_
#define _RULER_H_
#include <vector>
#include <glad/glad.h>
#include <stb_image.h>
#include "Shader.h"
class Ruler
{
public:
	Ruler() {

	}
	~Ruler() {

	}
	void initVertexObject();
	void generateTexture();
	void draw(Shader & shader);
private:
	std::vector<GLfloat> vertices = {
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // top left 
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f // bottom left
	};
	unsigned int VBO, VAO;
	unsigned int texture;
};


#endif
