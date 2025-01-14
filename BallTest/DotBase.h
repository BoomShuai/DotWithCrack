#pragma once
#include <SDL.h>
#include <vector>

class DotBase
{
public:
    static const int DOT_WIDTH = 20;
    static const int DOT_HEIGHT = 20;
    static const int DOT_VEL = 1;

    DotBase(){}
    virtual ~DotBase() {}   
    virtual void handleEvent(SDL_Event& e) = 0;
    void move(){}
    virtual void render() = 0;

    int mPosX, mPosY;
    int mVelX, mVelY;
};
