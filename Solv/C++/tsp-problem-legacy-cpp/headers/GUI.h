#pragma once
#include "TravelingSalespersonProblem.h"
#include "SDL.h"
#include "SDL_gfxPrimitives.h"
#include "SDL_ttf.h"

class GUI
{
public:
	GUI(void);
	~GUI(void);
	void initSDL();
	void drawPath(std::vector<int> vectorSelectedOffspring);
	void drawText(const char * charText, int xPosition, int yPosition);
	static const int WIDTH;
	static const int HEIGHT;
private:
	SDL_Surface * screen;
	TTF_Font * font;
	SDL_Rect rectText;
	SDL_Event event;
	double x0Position;
	double y0Position;
	double x1Position;
	double y1Position;
};

