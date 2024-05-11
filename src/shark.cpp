#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <vector>
#include <string>

#include "shark.h"
#include "renderwindow.h"
#include "entity.h"
#include "player.h"

Shark::Shark(float p_x, float p_y, std::vector<SDL_Texture*> p_tex)
    : Entity(p_x, p_y, p_tex), velocityX(-200), velocityY(0), textures(p_tex) {}

bool Shark::isTouchingPlayer(Player& p_player) {
    float playerX = p_player.getX();
    float playerY = p_player.getY();
    float playerWidth = p_player.getWidth();
    float playerHeight = p_player.getHeight();

    float hitboxScale = 0.5f;
    float hitboxWidthReduction = getWidth() * (1 - hitboxScale) / 2;
    float hitboxHeightReduction = getHeight() * (1 - hitboxScale) / 2;

    float sharkX = getX() + hitboxWidthReduction;
    float sharkY = getY() + hitboxHeightReduction;
    float sharkWidth = getWidth() * hitboxScale;
    float sharkHeight = getHeight() * hitboxScale;

    bool collisionX = sharkX + sharkWidth >= playerX && playerX + playerWidth >= sharkX;
    bool collisionY = sharkY + sharkHeight >= playerY && playerY + playerHeight >= sharkY;

    return collisionX && collisionY;
}

void Shark::move(float deltaTime) {
    setX(getX() + velocityX * deltaTime);  // Move shark left
}

void Shark::update(float deltaTime) {
    move(deltaTime);
    if (getX() + getWidth() < 0) {
        setX(RenderWindow::getWidth());  // Reset shark to the right side when it goes off screen
    }

    // Update texture to the next frame in the list
    currentTextureIndex = (currentTextureIndex + 1) % textures.size();  // Cycle through textures
    setTexture(currentTextureIndex);  // Set the texture to the current index
}

void Shark::reset() {
    setX(RenderWindow::getWidth());  // Reset position off-screen to the right
    currentTextureIndex = 0;  // Reset texture index
    setTexture(currentTextureIndex);  // Set texture to the first texture in the list
}

SDL_Texture* Shark::getCurrentTexture() const {
    return textures[currentTextureIndex];
}