#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>

#include "player.h"
#include "renderwindow.h"
#include "entity.h"

const float GRAVITY = 36.f;
const float CURRENT = -90.f;
const float ATTRACT = 8.f;
const float DRAG = 2.6f;
const float MAX_SPEED = 200.f;
const int HOLE_DEATH = 2;

Player::Player(float p_x, float p_y, std::vector<SDL_Texture*> p_tex)
	: Entity{ p_x, p_y, p_tex}, velocityX(0), velocityY(0), dead(0)
{
	setX(RenderWindow::getWidth() / 2 - getWidth() / 2);
	setY(RenderWindow::getHeight() - getHeight() - 64);
	loadHighscore();
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

	float cx = getX() + getWidth() / 2.f;
	float cy = getY() + getHeight() / 2.f;
	float dx = mouseX - cx;
	float dy = mouseY - cy;
	float distance = sqrt(dx * dx + dy * dy);

	float ax = CURRENT;
	float ay = GRAVITY;
	if (distance < steerRadius)
	{
		float influence = 1.f - distance / steerRadius;
		ax += dx * ATTRACT * influence;
		ay += dy * ATTRACT * influence;
	}

	velocityX += ax * deltaTime;
	velocityY += ay * deltaTime;

	float drag = exp(-DRAG * deltaTime);
	velocityX *= drag;
	velocityY *= drag;

	float speed = sqrt(velocityX * velocityX + velocityY * velocityY);
	if (speed > MAX_SPEED)
	{
		velocityX = velocityX / speed * MAX_SPEED;
		velocityY = velocityY / speed * MAX_SPEED;
	}

	setX(getX() + velocityX * deltaTime);
	setY(getY() + velocityY * deltaTime);

	cx = getX() + getWidth() / 2.f;
	cy = getY() + getHeight() / 2.f;
	if (cx < 0 || cx > RenderWindow::getWidth() || cy < 0 || cy > RenderWindow::getHeight())
		dead = HOLE_DEATH;
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
	survived += deltaTime;
	int nextScore = static_cast<int>(survived);
	if (nextScore != score)
	{
		score = nextScore;
		if (score > highscore)
		{
			highscore = score;
			saveHighscore();
		}
	}

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
	s = "BEST: " + std::to_string(highscore);
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
	survived = 0;
	velocityX = 0;
	velocityY = 0;
	dead = 0;
}

void Player::loadHighscore()
{
	std::ifstream in("highscore.txt");
	int saved = 0;
	if (in >> saved && saved > 0)
		highscore = saved;
}

void Player::saveHighscore()
{
	std::ofstream out("highscore.txt");
	if (out)
		out << highscore;
}

float Player::clamp(float p_value, float p_min, float p_max)
{
	if (p_value > p_max)
		return p_max;
	if (p_value < p_min)
		return p_min;
	return p_value;
}
