#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <array>
#include <random>
#include <time.h>
#include <SDL.h>

#include "Constants.h"
#include "Globals.h"
#include "Ball.h"
#include "Wall.h"
#include "Vehicle.h"
//#include "Sensor.h"

// The window we will be rendering to
std::shared_ptr<SDL_Window> gWindow = nullptr;

// The window renderer
std::shared_ptr<SDL_Renderer> gRenderer = nullptr;

// Start up SDL and create window
bool initRenderer();

// Free media and shut down SDL
void closeRenderer();

// Draw circle using midpoint circle algorithm
void drawCircle(std::shared_ptr<SDL_Renderer> gRenderer, int32_t centreX, int32_t centreY, int32_t radius);

// Load parameters from a file
bool chooseSimulations();
bool loadParametersFromFile();
void printParameters();

// Create objects
void createObjects(std::vector<std::shared_ptr<Wall>>& walls,
    std::shared_ptr<Wall>& piston, std::shared_ptr<Wall>& bottom,
    std::vector<std::shared_ptr<Ball>>& balls);

// Render objects
void renderBall(std::shared_ptr<Ball> ball);
void renderWalls(std::vector<std::shared_ptr<Wall>> walls);

// Piston dynamics
void processPiston(std::shared_ptr<Wall> piston, std::shared_ptr<Wall> bottom,
    std::vector<std::shared_ptr<Ball>> balls, bool& finish);

/* Main function */
int main(int argc, char* args[]) // needs argc and args[] for the SDL  
{
    // Simulation objects  
    std::vector<std::shared_ptr<Wall>> walls;
    std::shared_ptr<Wall> piston;
    std::shared_ptr<Wall> bottom;
    std::vector<std::shared_ptr<Ball>> balls;

    std::vector<std::shared_ptr<Vehicle>> vehicles;
    vehicles.push_back(std::make_shared<Vehicle>(100, 100, 50, 0, 10, 20));
    vehicles.push_back(std::make_shared<Vehicle>(150, 150, 50, M_PI / 2, 10, 20));//²¹³äµÄ´úÂë

    // Choose simulations  
    if (!chooseSimulations())
        return 0;

    // Create the objects  
    createObjects(walls, piston, bottom, balls);

    if (balls.size() >= 1 && is_testing > 0)
        balls.at(0)->setSpecificDirection(ball0_speed, ball0_vel_angle); // set for testing  

    if (balls.size() >= 2 && is_testing > 0)
        balls.at(1)->setSpecificDirection(ball1_speed, ball1_vel_angle); // set for testing  

    // Message for stopping the threads  
    bool finish = false;

    // Simulate  
    for (auto ball : balls)
    {
        ball->simulate();
    }

    // Process piston dynamics  
    std::thread t1(processPiston, piston, bottom, std::ref(balls), std::ref(finish));

    // Start up SDL and create window  
    if (!initRenderer())
    {
        std::cout << "Failed to initialize!" << std::endl;
    }
    else
    {
        // Initialize variables  
        bool quit = false; // loop flag  
        int tempKey = 0;

        // Main loop  
        while (!quit)
        {   
            SDL_Event e; // Event handler  

            // Handle keycylinder events on queue 
            while (SDL_PollEvent(&e) != 0)
            {
                // User requests quit  
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }

                // Adjust temp at bottom  
                if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
                {
                    switch (e.key.keysym.sym)
                    {
                    case SDLK_LEFT:
                        tempKey = -1;
                        break;
                    case SDLK_RIGHT:
                        tempKey = +1;
                        break;
                    default:
                        break;
                    }
                }

                if (e.type == SDL_KEYUP && e.key.repeat == 0)
                {
                    switch (e.key.keysym.sym)
                    {
                    case SDLK_LEFT:
                        if (tempKey < 0)
                            tempKey = 0;
                        break;
                    case SDLK_RIGHT:
                        if (tempKey > 0)
                            tempKey = 0;
                        break;
                    default:
                        break;
                    }
                }
            }
            // Update temp
            if (tempKey == +1) {
                double temp = bottom->getHeat();
                if (temp < bottom_temp_max) {
                    temp++;
                    bottom->setHeat(temp);
                }
            }
            if (tempKey == -1) {
                double temp = bottom->getHeat();
                if (temp > bottom_temp_min) {
                    temp--;
                    bottom->setHeat(temp);
                }
            }

            // Clear screen
            SDL_SetRenderDrawColor(gRenderer.get(), 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(gRenderer.get());

            // Render ball
            for (auto ball : balls) {
                renderBall(ball);
            }

            // Render walls
            renderWalls(walls);

            // Update screen
            SDL_RenderPresent(gRenderer.get());

        } // End of main loop
    }
    // Close the renderer
    closeRenderer();

    // Ask each ball in the simulation to terminate
    for (auto ball : balls) {
        ball->setShutdown(); // Signal the ball to shut down its simulation
    }

    // Wait for all threads to finish before returning from the function
    finish = true; // Flag to indicate that the simulation should finish
    t1.join(); // Wait for the main simulation thread to finish

    return 0; // Return success status
}

// Function to create simulation objects: walls and balls
void createObjects(std::vector<std::shared_ptr<Wall>>& walls,
    std::shared_ptr<Wall>& piston, std::shared_ptr<Wall>& bottom,
    std::vector<std::shared_ptr<Ball>>& balls) {
    // Create walls
    for (int nw = 0; nw < 4; nw++) {
        walls.push_back(std::make_shared<Wall>()); // Create a new wall and add it to the list
    }

    // Set positions and sizes for lateral walls
    walls.at(0)->setPosition(CYLINDER_CENTER_POSITION_X - CYLINDER_WIDTH / 2, CYLINDER_CENTER_POSITION_Y);
    walls.at(0)->setSize(WALL_WIDTH, CYLINDER_HEIGHT + WALL_WIDTH);
    walls.at(0)->setWallType(WallType::lateral);

    walls.at(1)->setPosition(CYLINDER_CENTER_POSITION_X + CYLINDER_WIDTH / 2, CYLINDER_CENTER_POSITION_Y);
    walls.at(1)->setSize(WALL_WIDTH, CYLINDER_HEIGHT + WALL_WIDTH);
    walls.at(1)->setWallType(WallType::lateral);

    // Set positions and sizes for piston and bottom walls
    walls.at(2)->setPosition(CYLINDER_CENTER_POSITION_X, CYLINDER_CENTER_POSITION_Y - CYLINDER_HEIGHT / 2);
    walls.at(2)->setSize(CYLINDER_WIDTH - WALL_WIDTH, WALL_WIDTH);
    walls.at(2)->setWallType(WallType::piston);

    walls.at(3)->setPosition(CYLINDER_CENTER_POSITION_X, CYLINDER_CENTER_POSITION_Y + CYLINDER_HEIGHT / 2);
    walls.at(3)->setSize(CYLINDER_WIDTH - WALL_WIDTH, WALL_WIDTH);
    walls.at(3)->setWallType(WallType::bottom);

    // Create references to piston and bottom
    piston = walls.at(2); // Reference to the piston wall
    piston->setMass(piston_mass); // Set mass for the piston

    bottom = walls.at(3); // Reference to the bottom wall
    bottom->setHeat(bottom_temp_min); // Set temperature for the bottom

    // Calculate the number of cells based on the number of balls
    int nCells = 0; // Number of cells
    int nRows, nCols;
    for (int n = 1; n <= nballs; n++) {
        nCells = n * n; // Calculate the number of cells as a square of n
        nRows = n; // Number of rows
        nCols = n; // Number of columns
        if (nCells >= nballs) {
            break; // Break the loop if the number of cells is sufficient
        }
    }

    // Define a struct for cell positions
    struct Cell {
        double x, y; // Position coordinates
    };

    std::vector<Cell> places; // Vector to hold cell positions
    double inicX, inicY, aux; // Temporary variables for position calculations
    walls.at(0)->getPosition(inicX, aux); // Get the position of the first wall
    inicX += WALL_WIDTH * 0.5; // Adjust the x-coordinate for the first cell
    walls.at(2)->getPosition(aux, inicY); // Get the position of the second wall
    inicY += WALL_WIDTH * 0.5; // Adjust the y-coordinate for the first cell

    // Calculate cell positions
    for (int nc = 0; nc < nCells; nc++) {
        Cell cell;
        cell.x = (inicX)+(nc % nCols) * ((CYLINDER_WIDTH - WALL_WIDTH) / nCols) + ((CYLINDER_WIDTH - WALL_WIDTH) / nCols) / 2.0;
        cell.y = (inicY)+(nc / nCols) * ((CYLINDER_HEIGHT - WALL_WIDTH) / nRows) + ((CYLINDER_HEIGHT - WALL_WIDTH) / nRows) / 2.0;
        places.push_back(cell); // Add the cell position to the vector
    }

    // Create balls at the calculated cell positions
    for (int nb = 0; nb < nballs; nb++) {
        balls.push_back(std::make_shared<Ball>()); // Create a new ball and add it to the list
        balls.at(nb)->setPosition(places.at(nb).x, places.at(nb).y); // Set the position of the ball

        // Prevent ball size overflow
        if (nCols * 2.0 * ball_radius > CYLINDER_WIDTH * 0.98) {
            balls.at(nb)->setRadius(CYLINDER_WIDTH * 0.98 / nCols / 2.0); // Adjust the radius if necessary
        }
        else {
            balls.at(nb)->setRadius(ball_radius); // Set the radius of the ball
        }

        balls.at(nb)->setRandomDirection(ball_speed); // Set a random direction for the ball
        balls.at(nb)->setMass(ball_mass); // Set the mass of the ball
        balls.at(nb)->setGravity(ball_gravity); // Set the gravity for the ball
        balls.at(nb)->setGasMode((gas_mode > 0)); // Set the gas mode for the ball
    }

    // Set references to other balls and walls in each ball
    for (auto ball : balls) {
        ball->setBalls(balls); // Set references to other balls
        ball->setWalls(walls); // Set references to walls
    }
}

// Function to initialize the renderer
bool initRenderer() {
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        success = false;
    }
    else {
        // Create window
        gWindow = std::shared_ptr<SDL_Window>(
            SDL_CreateWindow("Elastic Collisions of Multiple Balls Simulation",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN),
            SDL_DestroyWindow);
        if (gWindow == nullptr) {
            std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
            success = false;
        }
        else {
            // Create vsynced renderer for window
            gRenderer = std::shared_ptr<SDL_Renderer>(
                SDL_CreateRenderer(gWindow.get(),
                    -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
                SDL_DestroyRenderer);
            if (gRenderer == NULL) {
                std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
                success = false;
            }
            else {
                // Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer.get(), 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }
    return success;
}

// Function to close the renderer and quit SDL subsystems
void closeRenderer() {
    SDL_Quit();  //Quit SDL subsystems
}

// Function to draw a circle using the Bresenham's algorithm
void drawCircle(std::shared_ptr<SDL_Renderer> renderer, int32_t centreX, int32_t centreY, int32_t radius) {
    const int32_t diameter = (radius * 2); // Calculate the diameter of the circle

    int32_t x = (radius - 1); // Initialize x coordinate
    int32_t y = 0; // Initialize y coordinate
    int32_t tx = 1; // Change in x for the decision parameter
    int32_t ty = 1; // Change in y for the decision parameter
    int32_t error = (tx - diameter); // Decision parameter

    while (x >= y) {
        // Render each octant of the circle
        SDL_RenderDrawPoint(renderer.get(), centreX + x, centreY - y);
        SDL_RenderDrawPoint(renderer.get(), centreX + x, centreY + y);
        SDL_RenderDrawPoint(renderer.get(), centreX - x, centreY - y);
        SDL_RenderDrawPoint(renderer.get(), centreX - x, centreY + y);
        SDL_RenderDrawPoint(renderer.get(), centreX + y, centreY - x);
        SDL_RenderDrawPoint(renderer.get(), centreX + y, centreY + x);
        SDL_RenderDrawPoint(renderer.get(), centreX - y, centreY - x);
        SDL_RenderDrawPoint(renderer.get(), centreX - y, centreY + x);

        if (error <= 0) {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0) {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

// Function to render a ball on the screen
void renderBall(std::shared_ptr<Ball> ball) {
    double x, y, r;
    ball->getPosition(x, y); // Get the position of the ball
    r = ball->getRadius(); // Get the radius of the ball

    if (x < 0) return; // Do not render if the ball is off-screen

    SDL_SetRenderDrawColor(gRenderer.get(), 0x00, 0x00, 0x00, 0xFF); // Set the color to black

    drawCircle(gRenderer, (int32_t)x, (int32_t)y, (int32_t)r); // Draw the ball
}

// Function to render walls on the screen
void renderWalls(std::vector<std::shared_ptr<Wall>> walls) {
    for (auto wall : walls) {
        // Set the rectangle for the wall
        SDL_Rect rect;
        double x, y, w, h;
        wall->getPosition(x, y); // Get the position of the wall
        wall->getSize(w, h); // Get the size of the wall

        if (x < 0) continue; // Skip rendering if the wall is off-screen

        rect.x = static_cast<int>(x - w / 2); // Calculate the x-coordinate of the rectangle
        rect.y = static_cast<int>(y - h / 2); // Calculate the y-coordinate of the rectangle
        rect.w = static_cast<int>(w); // Set the width of the rectangle
        rect.h = static_cast<int>(h); // Set the height of the rectangle

        switch (wall->getWallType()) {
        case WallType::lateral: // Lateral wall
            SDL_SetRenderDrawColor(gRenderer.get(), 0x00, 0x00, 0x00, 0xFF); // Set the color to black
            break;
        case WallType::piston: // Piston wall
            SDL_SetRenderDrawColor(gRenderer.get(), 0x00, 0x00, 0xFF, 0xFF); // Set the color to blue
            break;
        case WallType::bottom: // Bottom wall
            double k = (wall->getHeat() - bottom_temp_min) / (bottom_temp_max + 0.001); // Calculate the heat ratio
            double kMin = bottom_temp_min / (bottom_temp_max + 0.001); // Calculate the minimum heat ratio
            uint16_t red = static_cast<uint16_t>(0xFF * 0.60 + (0xFF * kMin + 0xFF * k) * 0.40); // Calculate the red component based on heat
            SDL_SetRenderDrawColor(gRenderer.get(), red, 0x00, 0x00, 0xFF); // Set the color based on heat
            break;
        }
        SDL_RenderDrawRect(gRenderer.get(), &rect); // Draw the wall rectangle
    }
}

// Function to process the piston dynamics and collisions with balls
void processPiston(std::shared_ptr<Wall> piston, std::shared_ptr<Wall> bottom,
    std::vector<std::shared_ptr<Ball>> balls, bool& finish) {
    // Set to piston dynamics
    long pistoncollisions = 0;  // Counter for piston collisions
    double upForce = 0.0;  // Force exerted upwards on the piston
    double mv = 0.0;  // Momentum of the piston
    double avgUpForce = 0.0;  // Average upward force
    double downForce = 0.0;  // Downward force for testing
    downForce = piston_gravity * piston_mass;  // Calculate downward force
    double pistonVel = 0.0;  // Velocity of the piston
    int cycleDuration = 10;  // Define cycle duration in milliseconds to calculate up force
    bool gasMode = (gas_mode > 0);  // Flag for gas mode
    double temp = 0;  // Temporary variable for temperature

    // Init stop watch
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    lastUpdate = std::chrono::system_clock::now();

    // Function to monitor and test the simulation
    auto f0 = [balls, cycleDuration, gasMode, &avgUpForce, &downForce, &temp, &finish]() {
        long simTime = 0;  // Simulation time counter
        while (!finish) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // Sleep for 1 second

            // Count balls inside piston (testing)
            int count = 0;
            for (auto ball : balls) {
                double x, y;
                ball->getPosition(x, y);
                if (x > CYLINDER_CENTER_POSITION_X - CYLINDER_WIDTH * 0.5 && x <
                    CYLINDER_CENTER_POSITION_X + CYLINDER_WIDTH * 0.5 &&
                    y > CYLINDER_CENTER_POSITION_Y - CYLINDER_HEIGHT * 0.5 && y <
                    CYLINDER_CENTER_POSITION_Y + CYLINDER_HEIGHT * 0.5) {
                    count++;
                }
            }

            // Print simulation statistics
            std::cout << "Time:" << simTime << "\t";
            std::cout << "balls:" << count << "";

            if (gasMode) {
                std::cout << "gas avg(1.0s) up force: " << std::fixed <<
                    std::setprecision(2) << abs(avgUpForce) << " \t";
                std::cout << "piston down force: " << std::setprecision(2) << downForce << " \t";
            }
            std::cout << "~temp(bottom): " << std::setprecision(2) << temp << std::endl;

            simTime++;
        }
        };

    // Thread for monitoring and testing
    std::thread t1(f0);

    // Main loop to process piston dynamics and collisions
    while (!finish) {
        // Compute time difference in milliseconds
        auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now() - lastUpdate).count();

        // Receive and process messages from balls about collisions with the piston
        for (auto ball : balls) {
            while (ball->dataIsAvailable()) {
                //std::cout << "Main: ball msg available" << std::endl;  
                CollisionData msg = ball->receiveMsg();  // Receive message from the ball
                CollisionType colType = msg.type;  // Type of collision
                double velocity = msg.velY;  // Velocity of the ball, downward is positive

                // Process collision based on type
                switch (colType) {
                case CollisionType::bottomCollision:
                    // Handle bottom collision
                    //std::cout << "ball-bottom collision" << std::endl;  
                    break;
                case CollisionType::pistonCollision:
                    // Handle piston collision
                    //std::cout << "*gas* ball-piston collision" << std::endl; 
                    pistoncollisions++;
                    mv += 2.0 * ball_mass * velocity;  // Calculate force exerted on the piston
                    break;
                }
            }
        }

        // Process piston dynamics if cycle duration has passed
        if (timeSinceLastUpdate > cycleDuration) {
            // Calculate upward force exerted on the piston
            // F * t = m * v  
            upForce = mv / (timeSinceLastUpdate / 1000.0);
            avgUpForce = avgUpForce + upForce / (1000 / cycleDuration) -
                avgUpForce / (1000 / cycleDuration);  // 1000ms average (for printing)

            // Update piston position and velocity based on forces
            double posX, posY;
            double pistonW, pistonH;
            double velX, velY;
            double acceleration;

            piston->getPosition(posX, posY);
            piston->getSize(pistonW, pistonH);
            piston->getVelocity(velX, velY);

            // F = m * a  
            acceleration = (upForce + downForce) / (piston_mass + 1e-10);
            //downward = positive; prevent overflow  

            // Calculate piston position (x - x0) = vo * t + 1/2 * a * t^2  
            posY = posY + pistonVel * (timeSinceLastUpdate / 1000.0) +
                0.5 * acceleration * pow(timeSinceLastUpdate / 1000.0, 2);

            // Piston velocity for next cycle  
            pistonVel = velY + acceleration * (timeSinceLastUpdate / 1000.0);

            // Limit piston movement inside cylinder area
            if (posY < CYLINDER_CENTER_POSITION_Y - CYLINDER_HEIGHT * 0.5) {
                posY = CYLINDER_CENTER_POSITION_Y - CYLINDER_HEIGHT * 0.5;
            }
            else if (posY > CYLINDER_CENTER_POSITION_Y + CYLINDER_HEIGHT * 0.5 - WALL_WIDTH) {
                posY = CYLINDER_CENTER_POSITION_Y + CYLINDER_HEIGHT * 0.5 - WALL_WIDTH;
            }

            // Piston at the top: no velocity upward  
            if (posY <= (CYLINDER_CENTER_POSITION_Y - CYLINDER_HEIGHT * 0.5)) {
                if (pistonVel < 0) pistonVel = 0;
            }
            // Piston at the bottom: no velocity downward
            else if (posY >= (CYLINDER_CENTER_POSITION_Y + CYLINDER_HEIGHT * 0.5 - WALL_WIDTH)) {
                if (pistonVel > 0) pistonVel = -pistonVel;
            }

            // Update piston state
            piston->setPosition(posX, posY);
            piston->setVelocity(0.0, pistonVel);

            // Reset for next cycle
            pistoncollisions = 0;
            mv = 0.0;
            temp = bottom->getHeat();

            // Reset stop watch for next cycle
            lastUpdate = std::chrono::system_clock::now();
        }// End of cycle computations

        // Sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    }// End of while loop  

    t1.join(); // It will close because it is monitoring the finish flag  
}

// Function to load parameters from a file
bool loadParametersFromFile() {
    // Define a struct to hold parameter name and value
    struct Reading {
        std::string name;  // Name of the parameter
        double value;     // Value of the parameter
    };

    // Inform the user that parameters are being loaded
    std::cout << "Loading parameters file...\n" << std::endl;
    std::cout << "Please enter input file name: [Parameters.txt]";

    // Get the input file name from the user
    std::string iname;
    std::getline(std::cin, iname);

    // Set default file name if no name is provided
    if (iname.empty()) {
        iname = "Parameters.txt";
    }

    // Open the input stream with the provided file name
    std::ifstream ist{ iname };

    // Check if the file could not be opened
    if (!ist) {
        std::cout << "\nCan't open input file " << iname << std::endl;
        return false;
    }

    // Confirm that the file is being read
    if (ist) {
        std::cout << "Reading the file " << iname << " ...\n" << std::endl;
    }

    // Vector to store the parameters read from the file
    std::vector<Reading> parameters;

    // Read from the file until the end is reached
    while (ist) {
        std::string name{};
        double value{};
        ist >> name >> value;
        if (ist) {
            parameters.push_back(Reading{ name, value });
        }
    }

    // Check if the correct number of parameters were read
    if (parameters.size() != 15) {
        return false;
    }

    // Load the parameters into memory
    for (auto parameter : parameters) {
        if (parameter.name == "GAS_MODE") gas_mode = parameter.value;

        if (parameter.name == "NBALLS") nballs = parameter.value;
        if (parameter.name == "BALL_RADIUS") ball_radius = parameter.value;
        if (parameter.name == "BALL_SPEED") ball_speed = parameter.value;
        if (parameter.name == "BALL_MASS") ball_mass = parameter.value;
        if (parameter.name == "BALL_GRAVITY") ball_gravity = parameter.value;

        if (parameter.name == "IS_TESTING") is_testing = parameter.value;
        if (parameter.name == "BALL0_SPEED") ball0_speed = parameter.value;
        if (parameter.name == "BALL0_VEL_ANGLE") ball0_vel_angle = parameter.value;
        if (parameter.name == "BALL1_SPEED") ball1_speed = parameter.value;
        if (parameter.name == "BALL1_VEL_ANGLE") ball1_vel_angle = parameter.value;

        if (parameter.name == "PISTON_MASS") piston_mass = parameter.value;
        if (parameter.name == "PISTON_GRAVITY") piston_gravity = parameter.value;

        if (parameter.name == "BOTTOM_TEMP_MIN") bottom_temp_min = parameter.value;
        if (parameter.name == "BOTTOM_TEMP_MAX") bottom_temp_max = parameter.value;
    }

    return true;
}

// Function to print the loaded parameters
void printParameters() {
    std::cout << "GAS_MODE " << gas_mode << std::endl;

    std::cout << "NBALLS " << nballs << std::endl;
    std::cout << "BALL_RADIUS " << ball_radius << std::endl;
    std::cout << "BALL_SPEED " << ball_speed << std::endl;
    std::cout << "BALL_MASS " << ball_mass << std::endl;
    std::cout << "BALL_GRAVITY " << ball_gravity << std::endl;

    std::cout << "IS_TESTING " << is_testing << std::endl;
    std::cout << "BALL0_SPEED " << ball0_speed << std::endl;
    std::cout << "BALL0_VEL_ANGLE " << ball0_vel_angle << std::endl;
    std::cout << "BALL1_SPEED " << ball1_speed << std::endl;
    std::cout << "BALL1_VEL_ANGLE " << ball1_vel_angle << std::endl;

    std::cout << "PISTON_MASS " << piston_mass << std::endl;
    std::cout << "PISTON_GRAVITY " << piston_gravity << std::endl;

    std::cout << "BOTTOM_TEMP_MIN " << bottom_temp_min << std::endl;
    std::cout << "BOTTOM_TEMP_MAX " << bottom_temp_max << std::endl;
}

// Function to choose a simulation scenario
bool chooseSimulations() {
    // Display options for the user to choose from
    std::cout << "Please, choose one example or read parameters from a file:" << std::endl;
    std::cout << "[1] Three balls. (default)" << std::endl;
    std::cout << "[2] One ball at rest. Piston with gravity." << std::endl;
    std::cout << "[3] Eight balls at rest. Piston with gravity." << std::endl;
    std::cout << "[4] Fifty balls (gas simulation approximation). Piston with gravity." << std::endl;
    std::cout << "[5] Load parameters from a file." << std::endl;
    std::cout << "[6] ¿ØÖÆµ¥¸öÇòµÄ¶¯Ì¬¶àÇòÅö×²¾àÀë¼ì²â" << std::endl; 
    std::cout << "[7] Åö×²ºÐ¼ì²âÅö×²" << std::endl;
    std::cout << "[8] Åö×²¾àÀë¼ì²âÅö×²" << std::endl;

    // Get user input for the simulation scenario
    std::string input;
    std::cout << "\nOption:";
    std::getline(std::cin, input);

    // Default to scenario 1 if no input is provided
    int menuOption = 1;
    if (!input.empty()) {
        menuOption = stoi(input);
    }

    // Switch statement to set parameters based on user selection
    switch (menuOption) {
    case 1:
        // Simulation mode for three balls
        gas_mode = 0;
        nballs = 3; // Maximum approximately 100
        ball_radius = 20; // Maximum approximately 0.5 * WALL_WIDTH
        ball_speed = 60; // Pixels/s (SPPED < RADIUS * 200 max approx)
        ball_mass = 0.1; // Minimum approximately 0.01 * piston_mass
        ball_gravity = 0; // If zero, simulate gas
        piston_mass = 10.0;
        piston_gravity = 0.0; // If zero, piston stops
        break;

    case 2:
        // Simulation mode for one ball at rest with gravity
        gas_mode = 0;
        nballs = 1; // Maximum approximately 100
        ball_radius = 20; // Maximum approximately 0.5 * WALL_WIDTH
        ball_speed = 0; // Pixels/s (SPPED < RADIUS * 200 max approx)
        ball_mass = 0.2; // Minimum approximately 0.01 * piston_mass
        ball_gravity = 0; // If zero, simulate gas
        piston_mass = 10.0;
        piston_gravity = 10.0; // If zero, piston stops
        break;

    case 3:
        // Simulation mode for eight balls at rest with gravity
        gas_mode = 0;
        nballs = 8; // Maximum approximately 100
        ball_radius = 8; // Maximum approximately 0.5 * WALL_WIDTH
        ball_speed = 0; // Pixels/s (SPPED < RADIUS * 200 max approx)
        ball_mass = 0.2; // Minimum approximately 0.01 * piston_mass
        ball_gravity = 0; // If zero, simulate gas
        piston_mass = 10.0;
        piston_gravity = 10.0; // If zero, piston stops
        break;

    case 4:
        // Simulation mode for fifty balls in gas simulation
        gas_mode = 1;
        nballs = 50; // Maximum approximately 100
        ball_radius = 4; // Maximum approximately 0.5 * WALL_WIDTH
        ball_speed = 60; // Pixels/s (SPPED < RADIUS * 200 max approx)
        ball_mass = 0.1; // Minimum approximately 0.01 * piston_mass
        ball_gravity = 0; // If zero, simulate gas
        piston_mass = 10.0;
        piston_gravity = 10.0; // If zero, piston stops
        std::cout << "\nPlease, click on cylinder and increase energy with '>' right key." << std::endl;
        break;

    case 5:
        // Load parameters from a file
        if (!loadParametersFromFile()) {
            std::cout << "Can not load the parameters file." << std::endl;
            std::cout << "Continue with the default? [y]" << std::endl;
            std::string tc;
            std::getline(std::cin, tc);
            if (tc.empty()) {
                tc = "y";
            }
            if (tc == "y") {
                gas_mode = 0;
                nballs = 3; // Maximum approximately 100
                ball_radius = 20; // Maximum approximately 0.5 * WALL_WIDTH
                ball_speed = 60; // Pixels/s (SPPED < RADIUS * 200 max approx)
                ball_mass = 0.1; // Minimum approximately 0.01 * piston_mass
                ball_gravity = 0; // If zero, simulate gas
                piston_mass = 10.0;
                piston_gravity = 0.0; // If zero, piston stops
                break;
            }
            return false;
        }
        break;

    case 6:
        system("C:\\Users\\MingDer\\source\\repos\\Test_Mading\\x64\\Debug\\Test_Mading.exe");
        break;

    case 7:
        system("C:\\Users\\MingDer\\source\\repos\\BallBox\\x64\\Debug\\BallBox.exe");
        break;

    case 8:
        system("C:\\Users\\MingDer\\source\\repos\\DistanceBalll\\x64\\Debug\\DistanceBalll.exe");
        break;
        
    default:
        // Default simulation mode for three balls
        gas_mode = 0;
        nballs = 3; // Maximum approximately 100
        ball_radius = 20; // Maximum approximately 0.5 * WALL_WIDTH
        ball_speed = 60; // Pixels/s (SPPED < RADIUS * 200 max approx)
        ball_mass = 0.1; // Minimum approximately 0.01 * piston_mass
        ball_gravity = 0; // If zero, simulate gas
        piston_mass = 100.0;
        piston_gravity = 0.0; // If zero, piston stops
        break;
    }

    // Print the selected simulation parameters
    std::cout << "\nPlease, verify the simulation parameters:\n" << std::endl;
    printParameters();

    // Wait for user to press enter to continue
    std::cout << "\nPress enter to continue..." << std::endl;
    std::cin.ignore();

    return true;
}
