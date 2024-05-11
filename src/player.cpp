#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <vector>
#include <string>

#include "player.h"
#include "renderwindow.h"
#include "entity.h"

const float GRAVITY = 9.81;
const float WATER_DRAG = 1;
const int ALIVE = 0;
const int SHARK_DEATH = 1;
const int HOLE_DEATH = 2;

Player::Player(float p_x, float p_y, std::vector<SDL_Texture*> p_tex)
	: Entity{ p_x, p_y, p_tex}, velocityX(0), velocityY(0), dead(0)
{
	setX(RenderWindow::getWidth() / 2 - getWidth() / 2);
	setY(RenderWindow::getHeight() - getHeight() - 64);
}

float Player::distanceFromCursor()
{
	int mouseX = 0;
	int mouseY = 0;
	SDL_GetMouseState(&mouseX, &mouseY);

	return sqrt(pow((getX() + getWidth()/2) - mouseX, 2) + pow((getY() + getHeight()/2) - mouseY, 2));
}

void Player::moveWithCursor(float deltaTime)
{
	int mouseX = 0;
	int mouseY = 0;
	SDL_GetMouseState(&mouseX, &mouseY);

	float distance = distanceFromCursor();
	if (distance < 100)
	{
		velocityX += (mouseX - (getX() + getWidth() / 2)) * 0.1f * deltaTime;
		velocityY += (mouseY - (getY() + getHeight() / 2)) * 0.1f * deltaTime;
	}
	else
	{
		velocityX *= WATER_DRAG;
		velocityY *= WATER_DRAG;
	}

	velocityY += GRAVITY * deltaTime * 0.1f;

	setX(getX() + velocityX);
	setY(getY() + velocityY);

	// check if the player is out of bounds
	if (getX() < 0 || getX() > RenderWindow::getWidth() || getY() < -getHeight())
	{
		dead = HOLE_DEATH; // Changing to CURSOR_DEATH since the player is hitting a boundary
	}
	else if (getY() > RenderWindow::getHeight()  + getHeight())
	{
		dead = HOLE_DEATH;
	}
}

void Player::animEyes()
{
	int mouseX = 0;
	int mouseY = 0;
	SDL_GetMouseState(&mouseX, &mouseY);

	setAnimOffsetX(0, clamp(mouseX - getX() - getWidth()/2, -2.5, 2.5));
	setAnimOffsetY(0, clamp(mouseY - getY() - getHeight()/2 + 15, -2.5, 2.5));
}

void Player::update(float deltaTime)
{
	timer++;
	score = timer / 50; 
	if (score > highscore)
	{
		highscore = score;
	}

	setX(getX() - 1); //autoscroll
	animEyes();
	moveWithCursor(deltaTime);
}

const char* Player::getScore()
{
    static std::string s;
    s = "TIME: " + std::to_string(score);
    return s.c_str();
}


const char* Player::getHighscore()
{
   static std::string s;
    s = "BEST: " + std::to_string(score);
    return s.c_str();
}

int Player::getScoreInt()
{
	return score;
}

int Player::isDead()
{
	return dead;
}

void Player::setDead(int p_dead)
{
	dead = p_dead;
}

void Player::reset()
{
	setX(RenderWindow::getWidth() / 2 - getWidth() / 2);
	setY(RenderWindow::getHeight()  - getHeight() - 64);
	score = 0;
	timer = 0;
	velocityX = 0;
	velocityY = 0;
	dead = 0;
}

float Player::clamp(float p_value, float p_min, float p_max)
{
	if (p_value > p_max)
		return p_max;
	if (p_value < p_min)
		return p_min;
	return p_value;
}
