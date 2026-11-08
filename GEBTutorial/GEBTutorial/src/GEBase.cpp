#include <iostream>
#include "GamesEngineeringBase.h" // Include the GamesEngineeringBase header

#include "GameManager.h"

int main() {
    GameManager gameManager;

    if (gameManager.Init() < 1)
    {
        return 0;
    }

    while (gameManager.IsRunning())
    {
        // Check for input (key presses or window events)
        gameManager.HandleInput();

        // Clear the window for the next frame rendering
        gameManager.GetWindow().clear();

        // Update game logic
        gameManager.Update();

        // Draw();
        gameManager.Draw();

        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        gameManager.GetWindow().present();
    }
    
	gameManager.Destroy();
    return 0;
}