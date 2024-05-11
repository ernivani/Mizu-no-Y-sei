#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <vector>
#include <string>

#include "renderwindow.h"
#include "entity.h"
#include "player.h"

class Shark : public Entity {
private:
    float velocityX, velocityY;
    std::vector<SDL_Texture*> textures;
    size_t currentTextureIndex = 0;

public:
    Shark(float p_x, float p_y, std::vector<SDL_Texture*> p_tex);
    bool isTouchingPlayer(Player& p_player);
    void move(float deltaTime);
    void update(float deltaTime);
    void reset();
    SDL_Texture *getCurrentTexture() const;
};