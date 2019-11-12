#include "Line.h"
Line::Line(const std::vector<GLfloat> &vertices, int colorID, PrimitiveType type) : BaseModel(vertices, colorID, type)
{
}

Line:: ~Line()
{
}

GLfloat Line::getDistance() {
	return sqrt(pow((*getVertices())[0] - (*getVertices())[3], 2) + pow((*getVertices())[1] - (*getVertices())[4], 2) + pow((*getVertices())[2] - (*getVertices())[5], 2));
}