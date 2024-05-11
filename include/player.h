#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>

#include "entity.h"

class Player : public Entity {
public:	
	Player(float p_x, float p_y, std::vector<SDL_Texture*> p_tex);
	float distanceFromCursor();
	void moveWithCursor(float deltaTime);
	void update(float deltaTime);
	const char* getScore();
	const char* getHighscore();
	int getScoreInt();
	int isDead();
	void setDead(int p_dead);
	void reset();
private:
	float velocityX, velocityY;
	void animEyes();
	float clamp(float p_value, float p_min, float p_max);
	int score = 0;
	int highscore = 0;
	int timer = 0;
	int dead = 0;
};
