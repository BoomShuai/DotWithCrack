#pragma once
using namespace std;

struct Circle
{
	int x, y;
	int r;
};

class LTexture
{
public:
	LTexture();
	~LTexture();
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

class Dot
{
public:
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;
	static const int DOT_VEL = 1;

	Dot(int x, int y);
	void handleEvent(SDL_Event& e);
	void move(SDL_Rect& square, Circle& circle);
	void render();
	Circle& getCollider();

private:
	int mPosX, mPosY;
	int mVelX, mVelY;
	Circle mCollider;

	void shiftColliders();
};
