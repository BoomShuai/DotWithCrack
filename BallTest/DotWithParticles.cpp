#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "DotWithParticles.h"
#include "LTexture.h"
#include <time.h>
using namespace std;

bool init();
bool loadMedia();
void close();

LTexture gDotTexture;
LTexture gRedTexture;
LTexture gGreenTexture;
LTexture gBlueTexture;
LTexture gShimmerTexture;

Particle::Particle(int x, int y)
{

	mPosX = x - 5 + (rand() % 25);
	mPosY = y - 5 + (rand() % 25);
	mFrame = rand() % 5;

	switch (rand() % 3)
	{
	case 0: mTexture = &gRedTexture; break;
	case 1: mTexture = &gGreenTexture; break;
	case 2: mTexture = &gBlueTexture; break;
	}
}

void Particle::render()
{
	mTexture->render(mPosX, mPosY);
	if (mFrame % 2 == 0)
	{
		gShimmerTexture.render(mPosX, mPosY);
	}
	mFrame++;
}

bool Particle::isDead()
{
	return mFrame > 10;
}

DotWithParticles::DotWithParticles()
{

	mPosX = 0;
	mPosY = 0;

	mVelX = 0;
	mVelY = 0;

	for (int i = 0; i < TOTAL_PARTICLES; ++i)
	{
		particles[i] = new Particle(mPosX, mPosY);
	}
}

DotWithParticles::~DotWithParticles()
{
	for (int i = 0; i < TOTAL_PARTICLES; ++i)
	{
		delete particles[i];
	}
}

void  DotWithParticles::handleEvent(SDL_Event& e)
{

	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY -= DOT_VEL; break;
		case SDLK_DOWN: mVelY += DOT_VEL; break;
		case SDLK_LEFT: mVelX -= DOT_VEL; break;
		case SDLK_RIGHT: mVelX += DOT_VEL; break;
		}
	}
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY += DOT_VEL; break;
		case SDLK_DOWN: mVelY -= DOT_VEL; break;
		case SDLK_LEFT: mVelX += DOT_VEL; break;
		case SDLK_RIGHT: mVelX -= DOT_VEL; break;
		}
	}
}


void  DotWithParticles::render()
{
	gDotTexture.render(mPosX, mPosY);
	renderParticles();
}

void  DotWithParticles::renderParticles()
{
	for (int i = 0; i < TOTAL_PARTICLES; ++i)
	{
		if (particles[i]->isDead())
		{
			delete particles[i];
			particles[i] = new Particle(mPosX, mPosY);
		}
	}
	for (int i = 0; i < TOTAL_PARTICLES; ++i)
	{
		particles[i]->render();
	}
}

bool loadMedia1()
{
	bool success = true;

	if (!gDotTexture.loadFromFile("C:\\Users\\MingDer\\Downloads\\EdgeDownalode\\38_particle_engines\\38_particle_engines\\dot.bmp"))
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}

	if (!gRedTexture.loadFromFile("C:\\Users\\MingDer\\Downloads\\EdgeDownalode\\38_particle_engines\\38_particle_engines\\red.bmp"))
	{
		printf("Failed to load red texture!\n");
		success = false;
	}

	if (!gGreenTexture.loadFromFile("C:\\Users\\MingDer\\Downloads\\EdgeDownalode\\38_particle_engines\\38_particle_engines\\green.bmp"))
	{
		printf("Failed to load green texture!\n");
		success = false;
	}

	if (!gBlueTexture.loadFromFile("C:\\Users\\MingDer\\Downloads\\EdgeDownalode\\38_particle_engines\\38_particle_engines\\blue.bmp"))
	{
		printf("Failed to load blue texture!\n");
		success = false;
	}

	if (!gShimmerTexture.loadFromFile("C:\\Users\\MingDer\\Downloads\\EdgeDownalode\\38_particle_engines\\38_particle_engines\\shimmer.bmp"))
	{
		printf("Failed to load shimmer texture!\n");
		success = false;
	}

	gRedTexture.setAlpha(192);
	gGreenTexture.setAlpha(192);
	gBlueTexture.setAlpha(192);
	gShimmerTexture.setAlpha(192);

	return success;
}

bool loadMedia2()
{
	bool success = true;

	if (!gDotTexture.loadFromFile("C:\\Users\\MingDer\\Downloads\\EdgeDownalode\\38_particle_engines\\38_particle_engines\\dot.bmp"))
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}

	return success;
}

void  DotWithParticles::move()
{
	mPosX += mVelX;
	if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH))
	{
		mPosX -= mVelX;
	}
	mPosY += mVelY;
	if ((mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT))
	{
		mPosY -= mVelY;
	}
}

const int DOT_RADIUS = 10;
const int DOT_WIDTH = 2 * DOT_RADIUS;  
const int DOT_HEIGHT = 2 * DOT_RADIUS; 

bool checkCollision(DotWithParticles& dot1, DotWithParticles& dot2) {
	int dx = dot1.mPosX - dot2.mPosX;
	int dy = dot1.mPosY - dot2.mPosY;
	double distance = sqrt(dx * dx + dy * dy);

	return distance < (DOT_RADIUS + DOT_RADIUS);
}

vector<DotWithParticles> dots(10);

void handleCollision(DotWithParticles& dot1, DotWithParticles& dot2) 
{
	swap(dot1.mVelX, dot2.mVelX);
	swap(dot1.mVelY, dot2.mVelY);
}

void DotWithParticles::otherballmove(std::vector<DotWithParticles>& dots) {
	mPosX += mVelX;
	mPosY += mVelY;

	for (auto& otherDot : dots) {
		if (&otherDot != this && checkCollision(*this, otherDot)) {
			handleCollision(*this, otherDot); 
		}
	}
	if (mPosX < 0 || mPosX > SCREEN_WIDTH) mVelX = -mVelX;
	if (mPosY < 0 || mPosY > SCREEN_HEIGHT) mVelY = -mVelY;  
}

int main(int argc, char* argv[])
{
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		if (!loadMedia1() || !loadMedia2())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			bool quit = false;
			SDL_Event e;
			DotWithParticles dot;
			const int numDots = 10;
			vector<DotWithParticles> dots(numDots);
			srand(static_cast<unsigned int>(time(0)));

			for (int i = 0; i < numDots; ++i) {
				dots[i].mPosX = rand() % SCREEN_WIDTH;
				dots[i].mPosY = rand() % SCREEN_HEIGHT;
				dots[i].mVelX = rand() % 3 + 1;
				dots[i].mVelY = rand() % 3 + 1; 
			}

			while (!quit)
			{
				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					dot.handleEvent(e);
				}

				dot.move();
                for (auto& otherDot : dots) {
                    otherDot.otherballmove(dots); 
                }
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);  
				SDL_RenderClear(gRenderer);

				dot.render();
				for (auto& otherDot : dots) {
					otherDot.render();
					otherDot.drawProximityCircle(gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT, dot);
				}

				SDL_RenderPresent(gRenderer);
			}
		}
	}
	close();
	return 0;
}

void DotWithParticles::drawProximityCircle(SDL_Renderer* renderer, int screenWidth, int screenHeight, const DotWithParticles& otherDot) {
	int dx = otherDot.mPosX - mPosX;
	int dy = otherDot.mPosY - mPosY;
	double distance = sqrt(dx * dx + dy * dy);

	const double maxDistance = 300.0; 

	double ratio = min(1.0, distance / maxDistance);

	Uint8 green = static_cast<Uint8>(255 * ratio);  
	Uint8 red = static_cast<Uint8>(255 * (1.0 - ratio));  

	SDL_SetRenderDrawColor(renderer, red, green, 0x00, 0x80); 
	int r = DOT_RADIUS + 1;
	int indexX = mPosX + r;
	int indexY = mPosY + r;
	for (r ; r < DOT_RADIUS + 10; ++r) { 
		for (int w = 0; w < 360; w += 1) {  
			int x = static_cast<int>(indexX + r * cos(w * M_PI / 180.0));
			int y = static_cast<int>(indexY + r * sin(w * M_PI / 180.0));
			if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
				SDL_RenderDrawPoint(renderer, x, y);
			}
		}
	}
}

void close()
{

	gDotTexture.free();
	gRedTexture.free();
	gGreenTexture.free();
	gBlueTexture.free();
	gShimmerTexture.free();

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	IMG_Quit();
	SDL_Quit();
}
