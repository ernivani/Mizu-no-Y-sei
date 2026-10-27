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

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
		if (font == nullptr || p_text == nullptr)
			return;

		SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, p_text, textColor);
		if (surfaceMessage == nullptr)
			return;

		SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		SDL_Rect dst = {static_cast<int>(p_x), static_cast<int>(p_y), surfaceMessage->w, surfaceMessage->h};
		SDL_FreeSurface(surfaceMessage);
		if (message == nullptr)
			return;

		SDL_RenderCopy(renderer, message, nullptr, &dst);
		SDL_DestroyTexture(message);
}

void RenderWindow::renderCenter(float p_x, float p_y, const char* p_text, TTF_Font* font, SDL_Color textColor)
{
		if (font == nullptr || p_text == nullptr)
			return;

		SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, p_text, textColor);
		if (surfaceMessage == nullptr)
			return;

		SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		SDL_Rect dst;
		dst.w = surfaceMessage->w;
		dst.h = surfaceMessage->h;
		dst.x = SCREEN_WIDTH/2 - dst.w/2 + static_cast<int>(p_x);
		dst.y = SCREEN_HEIGHT/2 - dst.h/2 + static_cast<int>(p_y);
		SDL_FreeSurface(surfaceMessage);
		if (message == nullptr)
			return;

		SDL_RenderCopy(renderer, message, nullptr, &dst);
		SDL_DestroyTexture(message);
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
	SDL_BlendMode previous = SDL_BLENDMODE_NONE;
	SDL_GetRenderDrawBlendMode(renderer, &previous);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
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
	SDL_SetRenderDrawBlendMode(renderer, previous);
}

void RenderWindow::drawBubble(int centreX, int centreY, int radius)
{
	float pulse = sin(SDL_GetTicks() / 220.0) * 1.5;
	int r = radius + static_cast<int>(pulse);
	if (r < 4)
		return;

	SDL_Color fill = {210, 240, 255, 32};
	drawCircle(centreX, centreY, r, fill, true);

	SDL_BlendMode previous = SDL_BLENDMODE_NONE;
	SDL_GetRenderDrawBlendMode(renderer, &previous);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	auto ring = [&](int rad, Uint8 alpha) {
		SDL_SetRenderDrawColor(renderer, 245, 252, 255, alpha);
		int steps = rad * 8;
		for (int i = 0; i < steps; i++)
		{
			float angle = static_cast<float>(i) / static_cast<float>(steps) * 6.2831853f;
			int x = centreX + static_cast<int>(lround(cos(angle) * rad));
			int y = centreY + static_cast<int>(lround(sin(angle) * rad));
			SDL_RenderDrawPoint(renderer, x, y);
		}
	};

	ring(r, 230);
	ring(r - 1, 180);
	SDL_SetRenderDrawColor(renderer, 170, 220, 255, 140);
	int innerSteps = (r - 3) * 8;
	for (int i = 0; i < innerSteps; i++)
	{
		float angle = static_cast<float>(i) / static_cast<float>(innerSteps) * 6.2831853f;
		int x = centreX + static_cast<int>(lround(cos(angle) * (r - 3)));
		int y = centreY + static_cast<int>(lround(sin(angle) * (r - 3)));
		SDL_RenderDrawPoint(renderer, x, y);
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 235);
	int hx = centreX - r / 3;
	int hy = centreY - r / 3;
	int hr = r / 6;
	if (hr < 6)
		hr = 6;
	int shineSteps = hr * 6;
	for (int i = 0; i < shineSteps; i++)
	{
		float angle = 3.5f + static_cast<float>(i) / static_cast<float>(shineSteps) * 1.5f;
		int x = hx + static_cast<int>(lround(cos(angle) * hr));
		int y = hy + static_cast<int>(lround(sin(angle) * hr));
		SDL_RenderDrawPoint(renderer, x, y);
		SDL_RenderDrawPoint(renderer, x, y + 1);
	}

	SDL_SetRenderDrawBlendMode(renderer, previous);
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