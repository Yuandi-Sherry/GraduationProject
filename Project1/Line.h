#ifndef _LINE_H_
#define _LINE_H_
#include "BaseModel.h"
class Line : public BaseModel
{
public:
	Line(const std::vector<GLfloat> &vertices, int colorID, PrimitiveType type);
	~Line();
	GLfloat getDistance();
private:

};


#endif
