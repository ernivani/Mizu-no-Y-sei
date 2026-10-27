#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <stdlib.h>
#include <deque>
#include <algorithm>
#include <new>

#include "renderwindow.h"
#include "entity.h"
#include "player.h"
#include "shark.h"

 
const int ALIVE = 0;
const int SHARK_DEATH = 1;
const int HOLE_DEATH = 2;

const Uint8 *keyState;

RenderWindow window;

std::vector<SDL_Texture*> playerTex; 
SDL_Texture* arrow;
SDL_Texture* highscoreBox;
SDL_Texture* deathOverlay;

std::vector<SDL_Texture*> sharkTex;

SDL_Texture* far;
SDL_Texture* foregroundMerged;
SDL_Texture* foreground1;
SDL_Texture* foreground2;
SDL_Texture* sand;

TTF_Font* font32;
TTF_Font* font32_outline;
TTF_Font* font24;
TTF_Font* font16;

SDL_Color white = { 255, 255, 255, 255 };
SDL_Color black = { 0, 0, 0, 255 };

Mix_Chunk* jumpSfx;
Mix_Chunk* fallSfx;
Mix_Chunk* hitSfx;
Mix_Chunk* clickSfx;

// sharks
std::vector<Shark> sharks;
Uint32 lastSharkSpawnTime = 0;


Uint32 lastTime = SDL_GetTicks();
Uint32 currentTime;
float deltaTime;

bool gameRunning = true;
bool playedDeathSFX = false;
bool mainMenu = true;



struct BackgroundLayer {
    float positionx;
	float positiony;
    float speed;
    SDL_Texture* texture;
    int width;
    int height;
};

std::deque<BackgroundLayer> backgroundLayers;

bool initBackgroundLayers() {
    backgroundLayers.push_back({0, 0, 20.0f, far, 256, 192});
    backgroundLayers.push_back({0, 0, 45.0f, sand, 256, 192});
	backgroundLayers.push_back({0, 0, 90.0f, foregroundMerged, 512, 192});
    return true;
}

void renderRepeatingBackground() {
	float scale = static_cast<float>(RenderWindow::getHeight()) / 192.f;

    for (auto& bg : backgroundLayers) {
		int tileW = static_cast<int>(bg.width * scale);
		int tileH = static_cast<int>(bg.height * scale);
		if (tileW <= 0)
			continue;

        bg.positionx -= bg.speed * deltaTime;
		while (bg.positionx <= -tileW)
			bg.positionx += tileW;
		while (bg.positionx > 0)
			bg.positionx -= tileW;

        for (float x = bg.positionx; x < RenderWindow::getWidth(); x += tileW) {
            SDL_Rect dstRect = {static_cast<int>(x), 0, tileW, tileH};
            SDL_RenderCopy(window.getRenderer(), bg.texture, nullptr, &dstRect);
        }
    }
}


alignas(Player) static unsigned char playerStorage[sizeof(Player)];
Player& player = *reinterpret_cast<Player*>(playerStorage);


int laneCount()
{
	return std::max(2, RenderWindow::getHeight() / 160);
}

int sharkCap()
{
	int cap = std::max(1, laneCount() - 1);
	int score = player.getScoreInt();
	if (score < 12)
		return 1;
	if (score < 30)
		return std::min(cap, 2);
	return cap;
}

Uint32 sharkInterval()
{
	float ms = std::max(1500.f, 3000.f - player.getScoreInt() * 30.f);
	return static_cast<Uint32>(ms);
}

void spawnShark()
{
	int lanes = laneCount();
	int laneH = RenderWindow::getHeight() / lanes;
	const int drawH = 64;
	std::vector<int> openLanes;

	for (int i = 0; i < lanes; i++)
	{
		float laneY = i * laneH + (laneH - drawH) / 2.f;
		bool occupied = false;
		for (auto& shark : sharks)
		{
			if (std::abs(shark.getY() - laneY) < laneH * 0.55f)
				occupied = true;
		}
		if (!occupied)
			openLanes.push_back(i);
	}
	if (openLanes.empty())
		return;

	int lane = openLanes[rand() % openLanes.size()];
	float y = lane * laneH + (laneH - drawH) / 2.f;
	float speed = std::min(190.f, 105.f + player.getScoreInt() * 1.5f);
	sharks.push_back(Shark(RenderWindow::getWidth() + 12, y, sharkTex, speed));
	lastSharkSpawnTime = SDL_GetTicks();
}

void reset()
{
	player.reset();
	sharks.clear();
	lastSharkSpawnTime = SDL_GetTicks();
}
void gameLoop()
{
	SDL_Event event;
	
    currentTime = SDL_GetTicks();
    deltaTime = (currentTime - lastTime) / 1000.0f;
	if (deltaTime > 0.05f)
		deltaTime = 0.05f;
    lastTime = currentTime;

 	while (SDL_PollEvent(&event)) 
 	{
    	switch (event.type) {
			case SDL_QUIT: 
			{
				gameRunning = false;
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				if (mainMenu)
				{
					if (event.button.button == SDL_BUTTON_LEFT && SDL_GetTicks() > 2500)
					{
						mainMenu = false;
						lastSharkSpawnTime = SDL_GetTicks();
						Mix_PlayChannel(-1, clickSfx, 0);
					}
				}
				else 
				{
					if (event.button.button == SDL_BUTTON_LEFT && player.isDead() == ALIVE)
					{

						// handle player input
					}
					else if (player.isDead() != ALIVE)
					{
						Mix_PlayChannel(-1, clickSfx, 0);
						reset();
						playedDeathSFX = false;
					}
				}
				break;
			}
			case SDL_WINDOWEVENT:
			{
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    int newWidth = event.window.data1;
                    int newHeight = event.window.data2;
                    RenderWindow::handleWindowResize(newWidth, newHeight);
                }
                break;
			}
    	}
	}
	if (mainMenu)
	{
		if (SDL_GetTicks() < 2500)
		{
			window.clear();
			window.renderCenter(0, sin(SDL_GetTicks()/100) * 2 - 4, "MIZU NO YOSEI", font24, white);
			window.display();
		}
		else 
		{
			window.clear();
			renderRepeatingBackground();

			window.renderCenter(0, -24, "Steer with the cursor.", font16, white);
			window.renderCenter(0, 4, "A lane always stays open.", font16, white);
			window.renderCenter(0, 48 + sin(SDL_GetTicks() / 100.f) * 2, "Click to start", font24, white);
			window.display();
		}
	}
	else
	{
		sharks.erase(std::remove_if(sharks.begin(), sharks.end(), [](Shark& shark) {
			return shark.isOffScreen();
		}), sharks.end());

		if (player.isDead() == ALIVE && sharks.size() < static_cast<size_t>(sharkCap()) && currentTime - lastSharkSpawnTime > sharkInterval())
			spawnShark();

		if (player.isDead() == ALIVE)
			player.update(deltaTime);

		window.clear();
        renderRepeatingBackground();
		for (auto& shark : sharks) {
			shark.update(deltaTime);
			SDL_Rect destRect;
			destRect.x = static_cast<int>(shark.getX());
			destRect.y = static_cast<int>(shark.getY());
			destRect.w = shark.getDrawWidth();
			destRect.h = shark.getDrawHeight();

			SDL_RenderCopy(window.getRenderer(), shark.getCurrentTexture(), NULL, &destRect);

			if (player.isDead() == ALIVE && shark.isTouchingPlayer(player))
				player.setDead(SHARK_DEATH);
		}

		if (player.isDead() != ALIVE && !playedDeathSFX)
		{
			if (player.isDead() == HOLE_DEATH)
				Mix_PlayChannel(-1, fallSfx, 0);
			else if (player.isDead() == SHARK_DEATH)
				Mix_PlayChannel(-1, hitSfx, 0);
			playedDeathSFX = true;
		}


		window.render(player);
		window.render(25, 30, arrow);
		window.render(62, 20, player.getScore(), font32_outline, black);
		window.render(65, 23, player.getScore(), font32, white);
		window.render(0, 65, highscoreBox);
		window.render(65, 64, player.getHighscore(), font16, white);



		if (player.isDead() != ALIVE)
		{
			window.render(deathOverlay, 800, 480);
			
			if (player.isDead() == HOLE_DEATH)
			{
				window.renderCenter(0, -24, "The Ocean is a dangerous place.", font24, white);
			}
			if (player.isDead() == SHARK_DEATH)
			{
				window.renderCenter(0, -24, "A shark just ate you.", font24, white);
			}
			window.renderCenter(0, 12, "Click to retry.", font16, white);
		}
		
		if (player.isDead() == ALIVE)
		{
			int mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);
			window.drawBubble(mouseX, mouseY, static_cast<int>(Player::steerRadius));
		}

		window.display();
	}
}

void startGame()
{
	window.createRenderer();
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	srand((unsigned)time(0));

	playerTex.push_back(window.loadTexture("res/textures/player/player_0.png"));
	playerTex.push_back(window.loadTexture("res/textures/player/player_1.png"));
	playerTex.push_back(window.loadTexture("res/textures/player/player_2.png"));
	playerTex.push_back(window.loadTexture("res/textures/player/player_3.png"));
	playerTex.push_back(window.loadTexture("res/textures/player/player_4.png"));

	sharkTex.push_back(window.loadTexture("res/textures/shark/Shark1.png"));
	sharkTex.push_back(window.loadTexture("res/textures/shark/Shark2.png"));
	sharkTex.push_back(window.loadTexture("res/textures/shark/Shark3.png"));
	sharkTex.push_back(window.loadTexture("res/textures/shark/Shark4.png"));
	sharkTex.push_back(window.loadTexture("res/textures/shark/Shark5.png"));
	sharkTex.push_back(window.loadTexture("res/textures/shark/Shark6.png"));
	sharkTex.push_back(window.loadTexture("res/textures/shark/Shark7.png"));
	sharkTex.push_back(window.loadTexture("res/textures/shark/Shark8.png"));

	far = window.loadTexture("res/textures/background/far.png");
	foregroundMerged = window.loadTexture("res/textures/background/foregound-merged.png");
	foreground1 = window.loadTexture("res/textures/background/foreground-1.png");
	foreground2 = window.loadTexture("res/textures/background/foreground-2.png");
	sand = window.loadTexture("res/textures/background/sand.png");

	arrow = window.loadTexture("res/textures/arrow.png");
	highscoreBox = window.loadTexture("res/textures/highscore_box.png");
	deathOverlay = window.loadTexture("res/textures/death_overlay.png");

	font32 = TTF_OpenFont("res/fonts/cocogoose.ttf", 32);
	font32_outline = TTF_OpenFont("res/fonts/cocogoose.ttf", 32);
	font24 = TTF_OpenFont("res/fonts/cocogoose.ttf", 24);
	font16 = TTF_OpenFont("res/fonts/cocogoose.ttf", 16);
	TTF_SetFontOutline(font32_outline, 3);

	jumpSfx = Mix_LoadWAV("res/sounds/jump.wav");
	fallSfx = Mix_LoadWAV("res/sounds/fall.wav");
	hitSfx = Mix_LoadWAV("res/sounds/hit.wav");
	clickSfx = Mix_LoadWAV("res/sounds/click.wav");

	new (playerStorage) Player(RenderWindow::getWidth() / 2, RenderWindow::getHeight() - 64, playerTex);
	initBackgroundLayers();
	SDL_SetWindowResizable(window.getWindow(), SDL_TRUE);

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(gameLoop, 0, 1);
#else
	while (gameRunning)
	{
		Uint32 frameStart = SDL_GetTicks();
		gameLoop();
		Uint32 elapsed = SDL_GetTicks() - frameStart;
		if (elapsed < 16)
			SDL_Delay(16 - elapsed);
	}
#endif

	window.cleanUp();
	TTF_CloseFont(font32);
	TTF_CloseFont(font32_outline);
	TTF_CloseFont(font24);
	TTF_CloseFont(font16);
	TTF_Quit();
	SDL_Quit();
}

#ifdef __EMSCRIPTEN__
void startGameAsync(void*)
{
	startGame();
}
#endif

int main(int argc, char* args[])
{
#ifdef __EMSCRIPTEN__
	SDL_SetHint(SDL_HINT_EMSCRIPTEN_ASYNCIFY, "0");
#endif
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	window.create("Mizu no Yosei", RenderWindow::getWidth(), RenderWindow::getHeight());

#ifdef __EMSCRIPTEN__
	emscripten_async_call(startGameAsync, nullptr, 0);
#else
	startGame();
#endif

	return 0;
}