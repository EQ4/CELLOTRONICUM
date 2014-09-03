#ifndef CONTROLLERSDEF_H
#define CONTROLLERSDEF_H
#include "controllers.h"


class MousePosController : public Controller
{
	public:
	
	static constexpr const char* name="MousePos";
	static constexpr int busCount=2;
	const char* busNames[busCount]={"posX", "posY"};
	
	const char* getName() {return name;}
	int getBusCount() {return busCount;}
	const char* const* getBusNames() {return busNames;}
	MousePosController(int X, int Y)
	{
		initGUI(X, Y);
	}
	
	float getValue(int id) 
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		float value[2]={float(x)/float(width-1), 1.0f-float(y)/float(height-1)};
		return value[id];
	}
};

void registerControllers();
	
Controller* getController(const char* name, int X=0, int Y=0);

#endif