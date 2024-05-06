#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
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

#include "renderwindow.h"
#include "entity.h"
#include "player.h"

 
const int ALIVE = 0;
const int HOLE_DEATH = 2;

const Uint8 *keyState;

RenderWindow window;

std::vector<SDL_Texture*> playerTex; 
SDL_Texture* arrow;
SDL_Texture* highscoreBox;
SDL_Texture* deathOverlay;
// SDL_Texture* logo;

SDL_Texture* far;
SDL_Texture* foregroundMerged;
SDL_Texture* foreground1;
SDL_Texture* foreground2;
SDL_Texture* sand;

TTF_Font* font32;
TTF_Font* font32_outline;
TTF_Font* font24;
TTF_Font* font16;

SDL_Color white = { 255, 255, 255 };
SDL_Color black = { 0, 0, 0 };

Mix_Chunk* jumpSfx;
Mix_Chunk* fallSfx;
Mix_Chunk* hitSfx;
Mix_Chunk* clickSfx;

Uint32 lastTime = SDL_GetTicks();
Uint32 currentTime;
float deltaTime;

bool gameRunning = true;
bool playedDeathSFX = false;
bool mainMenu = true;

bool init()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	window.create("Mizu no Yosei", RenderWindow::getWidth(), RenderWindow::getHeight() );
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	srand((unsigned)time(0));

	playerTex.push_back(window.loadTexture("res/textures/player/player_0.png"));
	playerTex.push_back(window.loadTexture("res/textures/player/player_1.png"));
	playerTex.push_back(window.loadTexture("res/textures/player/player_2.png"));
	playerTex.push_back(window.loadTexture("res/textures/player/player_3.png"));
	playerTex.push_back(window.loadTexture("res/textures/player/player_4.png"));

	far = window.loadTexture("res/textures/background/far.png");
    foregroundMerged = window.loadTexture("res/textures/background/foregound-merged.png");
    foreground1 = window.loadTexture("res/textures/background/foreground-1.png");
    foreground2 = window.loadTexture("res/textures/background/foreground-2.png");
    sand = window.loadTexture("res/textures/background/sand.png");

	arrow = window.loadTexture("res/textures/arrow.png");
	highscoreBox = window.loadTexture("res/textures/highscore_box.png");
	deathOverlay = window.loadTexture("res/textures/death_overlay.png");
	// logo = window.loadTexture("res/textures/logo.png");

	font32 = TTF_OpenFont("res/fonts/cocogoose.ttf", 32);
	font32_outline = TTF_OpenFont("res/fonts/cocogoose.ttf", 32);
	font24 = TTF_OpenFont("res/fonts/cocogoose.ttf", 24);
	font16 = TTF_OpenFont("res/fonts/cocogoose.ttf", 16);
	TTF_SetFontOutline(font32_outline, 3); 

	jumpSfx = Mix_LoadWAV("res/sounds/jump.wav");
	fallSfx = Mix_LoadWAV("res/sounds/fall.wav");
	hitSfx = Mix_LoadWAV("res/sounds/hit.wav");
	clickSfx = Mix_LoadWAV("res/sounds/click.wav");
	Mix_PlayChannel(-1, jumpSfx, 0);

	return true;
}

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
    backgroundLayers.push_back({0,0, 25.0f, far, 256, 192});
    backgroundLayers.push_back({0, 0, 40.0f, sand, 256, 192}); 
	backgroundLayers.push_back({0, 0, 60.0f, foregroundMerged, 512, 192});
    return true;
}

void renderRepeatingBackground() {

    for (auto& bg : backgroundLayers) {
        bg.positionx -= bg.speed * deltaTime;

        // Render the background layer multiple times to cover the entire screen
        int x = static_cast<int>(bg.positionx);
		int y = 0;
        while (x < RenderWindow::getWidth()) {
            // Calculate scaled dimensions to fit the screen
            float scaleX = static_cast<float>(RenderWindow::getWidth()) / bg.width;
            float scaleY = static_cast<float>(RenderWindow::getHeight() ) / bg.height;
            int scaledWidth = static_cast<int>(bg.width * scaleX);
            int scaledHeight = static_cast<int>(bg.height * scaleY);

            // Render the background layer with scaled dimensions
            SDL_Rect dstRect = {x, y, scaledWidth, scaledHeight};
            SDL_RenderCopy(window.getRenderer(), bg.texture, nullptr, &dstRect);
            x += scaledWidth;
        }
    }
}


bool load = init();

Player player(RenderWindow::getWidth() / 2, RenderWindow::getHeight() - 64, playerTex);

void reset()
{
	player.reset();
}
void gameLoop()
{
	SDL_Event event;
	
    currentTime = SDL_GetTicks();
    deltaTime = (currentTime - lastTime) / 1000.0f; 
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

			// window.render(SCREEN_WIDTH/2 - 234, SCREEN_HEIGHT/2 - 94 - 30, logo);
			window.renderCenter(0, 90 + sin(SDL_GetTicks()/100) * 2, "Click to start", font24, white);
			window.display();
		}
	}
	else
	{
		if (player.isDead() == ALIVE)
		{
			player.update(deltaTime);
		}
		else if (!playedDeathSFX) {
			
			if (player.isDead() == HOLE_DEATH)
			{
				Mix_PlayChannel(-1, fallSfx, 0);
			}
			playedDeathSFX = true;
		}

		window.clear();
        renderRepeatingBackground();

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
			window.renderCenter(0, 12, "Click to retry.", font16, white);
		}

		
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		SDL_Color attractionZoneColor = {0, 255, 0, 255};
		window.drawCircle(mouseX, mouseY, 100.0f, attractionZoneColor, false);
		window.display();
	}
}

int main(int argc, char* args[])
{
	initBackgroundLayers();
	SDL_SetWindowResizable(window.getWindow(), SDL_TRUE);

	#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(gameLoop, 0, 1);
	#else
	while (gameRunning) 
	{
    	gameLoop();
    	SDL_Delay(16);
	}
	#endif

	window.cleanUp();
	TTF_CloseFont(font32);
	TTF_CloseFont(font32_outline);
	TTF_CloseFont(font24);
	TTF_CloseFont(font16);
	TTF_Quit();
	SDL_Quit();

	return 0;
}