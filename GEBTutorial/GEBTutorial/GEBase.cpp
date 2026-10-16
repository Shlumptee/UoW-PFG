#include <iostream>
#include "GamesEngineeringBase.h" // Include the GamesEngineeringBase header

#include "GameLoop.h"

int main() {
    GameLoop gameLoop;

    if (gameLoop.Init() < 1)
    {
        return 0;
    }

    while (gameLoop.isRunning)
    {
        // Check for input (key presses or window events)
        gameLoop.HandleInput();

        // Clear the window for the next frame rendering
        gameLoop.canvas.clear();

        // Update game logic
        gameLoop.Update();

        // Draw();
        gameLoop.Draw();

        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        gameLoop.canvas.present();
    }
    return 0;
}