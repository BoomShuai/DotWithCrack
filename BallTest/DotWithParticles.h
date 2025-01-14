#pragma once
#include "DotBase.h"
#include "LTexture.h"
using namespace std;

const int TOTAL_PARTICLES = 20;

class Particle
{
public:

    Particle(int x, int y);
    void render();
    bool isDead();

private:
    int mPosX, mPosY;
    int mFrame;
    LTexture* mTexture;
};

class DotWithParticles : public DotBase
{
public:

    DotWithParticles();
    ~DotWithParticles();

    void handleEvent(SDL_Event& e) override;
    void move();
    void otherballmove(vector<DotWithParticles>& dots);
    void drawProximityCircle(SDL_Renderer* renderer, int screenWidth, int screenHeight, const DotWithParticles& otherDot);
    void render() override;

    Particle* particles[TOTAL_PARTICLES];
    void renderParticles();
};
