#ifndef _LINE_H_
#define _LINE_H_
#include "BaseModel.h"
class Line : public BaseModel
{
public:
	Line(const std::vector<GLfloat> &vertices, glm::vec3 colorID, PrimitiveType type);
	~Line();
	GLfloat getDistance();
	void initVertexObject();
private:

};


#endif
