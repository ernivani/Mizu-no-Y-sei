#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <vector>
#include <string>

#include "shark.h"
#include "renderwindow.h"
#include "entity.h"
#include "player.h"

Shark::Shark(float p_x, float p_y, std::vector<SDL_Texture*> p_tex, float speed)
    : Entity(p_x, p_y, p_tex), velocityX(-speed), velocityY(0), textures(p_tex) {}

bool Shark::isTouchingPlayer(Player& p_player) {
    const float sharkScale = 0.72f;
    float sharkWidth = getDrawWidth() * sharkScale;
    float sharkHeight = getDrawHeight() * sharkScale;
    float sharkX = getX() + (getDrawWidth() - sharkWidth) / 2.f;
    float sharkY = getY() + (getDrawHeight() - sharkHeight) / 2.f;

    const float playerScale = 0.8f;
    float playerWidth = p_player.getWidth() * playerScale;
    float playerHeight = p_player.getHeight() * playerScale;
    float playerX = p_player.getX() + (p_player.getWidth() - playerWidth) / 2.f;
    float playerY = p_player.getY() + (p_player.getHeight() - playerHeight) / 2.f;

    bool collisionX = sharkX + sharkWidth >= playerX && playerX + playerWidth >= sharkX;
    bool collisionY = sharkY + sharkHeight >= playerY && playerY + playerHeight >= sharkY;

    return collisionX && collisionY;
}

int Shark::getDrawWidth() {
    return getWidth() * 2;
}

int Shark::getDrawHeight() {
    return getHeight() * 2;
}

bool Shark::isOffScreen() {
    return getX() + getDrawWidth() < 0;
}

void Shark::move(float deltaTime) {
    setX(getX() + velocityX * deltaTime);
}

void Shark::update(float deltaTime) {
    move(deltaTime);

    animTimer += deltaTime;
    const float frameTime = 0.1f;
    while (animTimer >= frameTime && !textures.empty()) {
        animTimer -= frameTime;
        currentTextureIndex = (currentTextureIndex + 1) % textures.size();
    }
}

void Shark::reset() {
    setX(RenderWindow::getWidth());  // Reset position off-screen to the right
    currentTextureIndex = 0;  // Reset texture index
    setTexture(currentTextureIndex);  // Set texture to the first texture in the list
}

SDL_Texture* Shark::getCurrentTexture() const {
    return textures[currentTextureIndex];
}