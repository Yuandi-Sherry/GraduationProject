#ifndef _CHARACTER_H_
#define _CHARACTER_H_
#include <glad/glad.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"
extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;
struct singleCharacter {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

class Character
{
public:
	Character();
	~Character();
	void RenderText(Shader& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
	void init();
	void prepareCharacters();
	glm::mat4 getProjection();
private:
	std::map<GLchar, singleCharacter> Characters;
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
	//Shader shader("characterShader.vs", "characterShader.frag");
	FT_Library ft;
	FT_Face face;

	GLuint VAO, VBO;
	glm::mat4 projection;
};

#endif 