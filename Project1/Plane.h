#ifndef _PLANE_H_
#define _PLANE_H_
#include "BaseModel.h"
class Plane : public BaseModel
{
public:
	Plane(const std::vector<GLfloat> &vertices, int colorID, PrimitiveType type);
	~Plane();
	void initVertexObject();
private:

};


#endif