#include <iostream>
#include "GamesEngineeringBase.h" // Include the GamesEngineeringBase header

int main() {

    // Create a canvas window with dimensions 1024x768 and title “Tiles"
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 768, "Tiles");
    bool running = true; // Variable to control the main loop's running state.

    while (running)
    {
        // Check for input (key presses or window events)
        if (canvas.keyPressed(VK_ESCAPE)) break;
        if (canvas.keyPressed(VK_SPACE)) {

        }

        // Clear the window for the next frame rendering
        canvas.clear();
        // Update game logic

        // Draw();
        for (int i = 0; i < canvas.getHeight(); i++) {
            for (int j = 0; j < canvas.getWidth(); j++) {
                canvas.draw(j, i, 0, 0, 128);
            }
        }

        for (int i = 0; i < canvas.getHeight(); i++) {
            canvas.draw(70, i, 255, 255, 255);
        }

        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
    }
    return 0;
}