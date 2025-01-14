#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

class LTexture
{
public:
    LTexture();
    ~LTexture();

    bool init;
    bool loadFromFile(string path);
    void free();
    void setColor(Uint8 red, Uint8 green, Uint8 blue);
    void setBlendMode(SDL_BlendMode blending);
    void setAlpha(Uint8 alpha);
    void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
    int getWidth();
    int getHeight();
private:

    SDL_Texture* mTexture;
    int mWidth;
    int mHeight;
};
