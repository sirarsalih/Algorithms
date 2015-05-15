#include "GUI.h"

const int GUI::WIDTH = 1024;
const int GUI::HEIGHT = 768;
const int BPP = 32;		//Bits pr. pixel

const char * FONT_TYPE = "SegoeWP.ttf";			//Font type
const int FONT_SIZE = 20;							//Font size
SDL_Color colorWhite = {255, 255, 255};				//Font color (foreground)
SDL_Color colorBlack = {0, 0, 0};					//Font color (background)

GUI::GUI(void)
{
	//Init positions for path
	x0Position = 0;
	y0Position = 0;
	x1Position = 0;
	y1Position = 0;
}

void GUI::initSDL()
{
	//Initialize SDL and SDL_TTF (text)
	if (SDL_Init(SDL_INIT_VIDEO) == -1 || TTF_Init() == -1) {
		exit(0);
	}
	screen = SDL_SetVideoMode(WIDTH, HEIGHT, BPP, SDL_SWSURFACE);
	font = TTF_OpenFont(FONT_TYPE, FONT_SIZE);	//Load the font for text
	/*if (font == NULL) {
		exit(0);
	}*/
	SDL_WM_SetCaption("Traveling Salesman Problem", "Traveling Salesman Problem");
}

void GUI::drawPath(std::vector<int> vectorSelectedOffspring)
{
	//Fill the screen with black colour (reset)
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

	//Draw the path of the offspring
	for(int i = 0; i < vectorSelectedOffspring.size(); i++)
	{
		//Get x, y positions
		x0Position = TravelingSalespersonProblem::mapX[vectorSelectedOffspring[i]];
		y0Position = TravelingSalespersonProblem::mapY[vectorSelectedOffspring[i]];

		if(i == vectorSelectedOffspring.size() - 1)
		{
			x1Position = TravelingSalespersonProblem::mapX[vectorSelectedOffspring[0]];
			y1Position = TravelingSalespersonProblem::mapY[vectorSelectedOffspring[0]];
		}

		else
		{
			x1Position = TravelingSalespersonProblem::mapX[vectorSelectedOffspring[i + 1]];
			y1Position = TravelingSalespersonProblem::mapY[vectorSelectedOffspring[i + 1]];
		}

		//Draw path
		lineRGBA(screen, x0Position, y0Position, x1Position, y1Position, 255, 255, 255, 255);	
	}

	//Draw fixed city positions
	for(int i = 0; i < vectorSelectedOffspring.size(); i++)
	{
		//Filled rectangle/box
		boxRGBA(screen, TravelingSalespersonProblem::mapX[i], TravelingSalespersonProblem::mapY[i], TravelingSalespersonProblem::mapX[i] + 3, TravelingSalespersonProblem::mapY[i] + 3, 255, 0, 0, 255);
	}

	//Update the screen buffer
	SDL_Flip(screen);

	//Event poll for input from user
	while(SDL_PollEvent(&event)) 
	{
		if(event.type == SDL_QUIT) 
		{
			TTF_CloseFont(font);	//Unload font
			SDL_Quit();				//Exit SDL window
			exit(0);				//Exit all
		}
	}
}

void GUI::drawText(const char * charText, int xPosition, int yPosition)
{   
	//Set the text (allocate)
	if (font != NULL) {
		SDL_Surface * text = TTF_RenderText_Shaded(font, charText, colorWhite, colorBlack);

		//Set the position of the text
		rectText.x = xPosition;
		rectText.y = yPosition;

		//Blit the text on the screen
		SDL_BlitSurface(text, NULL, screen, &rectText);

		//Update the screen buffer
		SDL_Flip(screen);

		//Free the text (free)
		SDL_FreeSurface(text);
	}	
}

GUI::~GUI(void)
{
}
