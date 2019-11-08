#ifndef _TOOL_BAR_
#define _TOOL_BAR_

class Toolbar
{
public:
	Toolbar();
	~Toolbar();
	bool ruler;
	bool cutface;
private:
	
};

Toolbar::Toolbar()
{
	ruler = false;
	cutface = false;
}

Toolbar::~Toolbar()
{
}
#endif