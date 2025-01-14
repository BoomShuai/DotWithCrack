/*#include <iostream>
#include <cmath>
#include <SDL.h>

using namespace std;

// Define the width and height of the screen
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// Function to draw a filled square
void DrawSquare(SDL_Renderer* renderer, int centerX, int centerY, int size, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_Rect rect = { centerX - size / 2, centerY - size / 2, size, size };
    SDL_RenderFillRect(renderer, &rect);
}

// Function to draw a filled equilateral triangle
void DrawEquilateralTriangle(SDL_Renderer* renderer, int centerX, int centerY, int side, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    int height = (int)(side * sqrt(3) / 2);
    int points[3][2] = {
        {centerX, centerY - height / 2},
        {centerX - side / 2, centerY + height / 2},
        {centerX + side / 2, centerY + height / 2}
    };

    // Draw the triangle
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawLine(renderer, points[0][0], points[0][1], points[1][0], points[1][1]);
    SDL_RenderDrawLine(renderer, points[1][0], points[1][1], points[2][0], points[2][1]);
    SDL_RenderDrawLine(renderer, points[2][0], points[2][1], points[0][0], points[0][1]);
}

// Function to draw a filled circle
void DrawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, Uint8 r, Uint8 g, Uint8 b) {
    for (int y = centerY - radius; y <= centerY + radius; y++) {
        for (int x = centerX - radius; x <= centerX + radius; x++) {
            int dx = x - centerX;
            int dy = y - centerY;
            if (dx * dx + dy * dy <= radius * radius) {
                SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    // Initialize SDL with video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return -1;
    }

    // Create a window with the specified title, position, width, height, and flags
    window = SDL_CreateWindow("Shapes Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return -1;
    }

    // Create a renderer for the window with hardware acceleration
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Set the draw color to white
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Clear the renderer with the draw color
    SDL_RenderClear(renderer);

    // Calculate the center of the window
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;

    // Define the size of the shapes
    int shapeSize = 120;
    int spacing = shapeSize + 25; // Space between shapes

    // Draw a red square, a blue equilateral triangle, and a green circle
    DrawSquare(renderer, centerX - 2 * spacing, centerY, shapeSize, 0xFF, 0x00, 0x00); // Red square
    DrawEquilateralTriangle(renderer, centerX, centerY, shapeSize, 0x00, 0x00, 0xFF); // Blue triangle
    DrawCircle(renderer, centerX + 2 * spacing, centerY, shapeSize / 2, 0x00, 0xFF, 0x00); // Green circle

    // Update the screen to display the rendered content
    SDL_RenderPresent(renderer);

    // Wait for 5000 milliseconds (5 seconds)
    SDL_Delay(5000);

    // Clean up by destroying the renderer and window, and quitting SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}*/