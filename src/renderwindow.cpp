#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "renderwindow.h"
#include "entity.h"

#include <cmath>

int SCREEN_WIDTH = 900;
int SCREEN_HEIGHT = 480;

RenderWindow::RenderWindow()
{

}

void RenderWindow::create(const char* p_title, int p_w, int p_h)
{
	window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_w, p_h, SDL_WINDOW_SHOWN);

	if (window == NULL)
		std::cout << "Window failed to init. Error: " << SDL_GetError() << std::endl;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}


SDL_Texture* RenderWindow::loadTexture(const char* p_filePath)
{
	SDL_Texture* texture = NULL; 
	texture = IMG_LoadTexture(renderer, p_filePath);

	if (texture == NULL)
		std::cout << "Failed to load texture. Error: " << SDL_GetError() << std::endl;

	return texture;
}

void RenderWindow::clear()
{
	SDL_SetRenderDrawColor(renderer, 90, 90, 90, 255);
	SDL_RenderClear(renderer);
}

void RenderWindow::render(Entity& p_entity)
{

	for (int i = p_entity.getSize() - 1; i >= 0; i--)
	{
		SDL_Rect src;
		src.x = p_entity.getCurrentFrame().x;
		src.y = p_entity.getCurrentFrame().y;
		src.w = p_entity.getCurrentFrame().w;
		src.h = p_entity.getCurrentFrame().h;

		SDL_Rect dst;
		dst.x = p_entity.getX() + p_entity.getAnimOffsetX(i);
		dst.y = p_entity.getY() + p_entity.getAnimOffsetY(i);
		dst.w = p_entity.getCurrentFrame().w;
		dst.h = p_entity.getCurrentFrame().h;

		SDL_RenderCopy(renderer, p_entity.getTex(i), &src, &dst);
	}
}

void RenderWindow::render(float p_x, float p_y, SDL_Texture* p_tex)
{
	SDL_Rect src;
	src.x = 0;
	src.y = 0;
	SDL_QueryTexture(p_tex, NULL, NULL, &src.w, &src.h); 

	SDL_Rect dst;
	dst.x = p_x;
	dst.y = p_y;
	dst.w = src.w;
	dst.h = src.h;

	SDL_RenderCopy(renderer, p_tex, &src, &dst);
}

void RenderWindow::render(SDL_Texture* p_tex, int width, int height)
{ 
	float scaleX = static_cast<float>(SCREEN_WIDTH) / width;
    float scaleY = static_cast<float>(SCREEN_HEIGHT ) / height;
    int scaledWidth = static_cast<int>(width * scaleX);
    int scaledHeight = static_cast<int>(height * scaleY);

    // Render the background layer with scaled dimensions
    SDL_Rect dstRect = {0, 0, scaledWidth, scaledHeight};
    SDL_RenderCopy(renderer, p_tex, nullptr, &dstRect);
}

void RenderWindow::render(float p_x, float p_y, const char* p_text, TTF_Font* font, SDL_Color textColor)
{
		SDL_Surface* surfaceMessage = TTF_RenderText_Blended( font, p_text, textColor);
		SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

		SDL_Rect src;
		src.x = 0;
		src.y = 0;
		src.w = surfaceMessage->w;
		src.h = surfaceMessage->h; 

		SDL_Rect dst;
		dst.x = p_x;
		dst.y = p_y;
		dst.w = src.w;
		dst.h = src.h;

		SDL_RenderCopy(renderer, message, &src, &dst);
		SDL_FreeSurface(surfaceMessage);
}

void RenderWindow::renderCenter(float p_x, float p_y, const char* p_text, TTF_Font* font, SDL_Color textColor)
{
		SDL_Surface* surfaceMessage = TTF_RenderText_Blended( font, p_text, textColor);
		SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

		SDL_Rect src;
		src.x = 0;
		src.y = 0;
		src.w = surfaceMessage->w;
		src.h = surfaceMessage->h; 

		SDL_Rect dst;
		dst.x = SCREEN_WIDTH/2 - src.w/2 + p_x;
		dst.y = SCREEN_HEIGHT/2 - src.h/2 + p_y;
		dst.w = src.w;
		dst.h = src.h;

		SDL_RenderCopy(renderer, message, &src, &dst);
		SDL_FreeSurface(surfaceMessage);
}

void RenderWindow::display()
{
	SDL_RenderPresent(renderer);
}

void RenderWindow::cleanUp()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}


// add field fill
void RenderWindow::drawCircle(int centreX, int centreY, int radius, SDL_Color color, bool fill)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	for (double dy = 1; dy <= radius; dy += 1.0)
	{
		double dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));
		if (fill)
		{
			SDL_RenderDrawLine(renderer, centreX - dx, centreY + dy - radius, centreX + dx, centreY + dy - radius);
			SDL_RenderDrawLine(renderer, centreX - dx, centreY - dy + radius, centreX + dx, centreY - dy + radius);
		}
		else
		{
			SDL_RenderDrawPoint(renderer, centreX + dx, centreY + dy - radius);
			SDL_RenderDrawPoint(renderer, centreX - dx, centreY + dy - radius);
			SDL_RenderDrawPoint(renderer, centreX + dx, centreY - dy + radius);
			SDL_RenderDrawPoint(renderer, centreX - dx, centreY - dy + radius);
		}
	}
}

SDL_Renderer* RenderWindow::getRenderer()
{
	return renderer;
}

SDL_Window* RenderWindow::getWindow()
{
	return window;
}

void RenderWindow::handleWindowResize(int p_w, int p_h)
{
	SCREEN_WIDTH = p_w;
	SCREEN_HEIGHT = p_h;
}

int RenderWindow::getWidth()
{
	return SCREEN_WIDTH;
}

int RenderWindow::getHeight()
{
	return SCREEN_HEIGHT;
}