#include "Ruler.h"
#include <stb_image.h>
#include <iostream>
// 
void Ruler::initVertexObject() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &(vertices.front()), GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char *data = stbi_load("ruler.jpg", &width, &height, &nrChannels, 0);
	//std::cout << data << std::endl;
	//std::cout << " width " << width << " height " << height << std::endl;
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load ruler texture" << std::endl;
	}
	stbi_image_free(data);
}

void Ruler::generateTexture() {
	
}
void Ruler::draw() {
	// bind Texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// render container
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);
}


void Ruler::move(Camera_Movement direction, float deltaTime)
{
	float velocity = rulerSpeed * deltaTime;
	if (direction == FORWARD)
		position += glm::vec3(0.0f, 0.0f, 1.0f) * velocity;
	if (direction == BACKWARD)
		position -= glm::vec3(0.0f, 0.0f, 1.0f) * velocity;
	if (direction == LEFT)
		position -= glm::vec3(1.0f, 0.0f, 0.0f) * velocity;
	if (direction == RIGHT)
		position += glm::vec3(1.0f, 0.0f, 0.0f) * velocity;
}


glm::vec3 Ruler::getPosition() {
	return position;
}