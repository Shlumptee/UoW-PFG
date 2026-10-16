#include <iostream>
#include "GamesEngineeringBase.h" // Include the GamesEngineeringBase header

int main() {

    // Create a canvas window with dimensions 1024x768 and title “Tiles"
    GamesEngineeringBase::Window canvas;
    GamesEngineeringBase::Image image;
    canvas.create(1024, 768, "Tiles");
    bool running = true; // Variable to control the main loop's running state.

    unsigned int red = 0, green = 0, blue = 0;

    while (running)
    {
        // Check for input (key presses or window events)
        if (canvas.keyPressed(VK_ESCAPE)) break;
        if (canvas.keyPressed(VK_SPACE)) {

        }
        if (canvas.keyPressed(VK_SHIFT)) {
            if (canvas.keyPressed('R')) {
                red++;
                if (red > 255) {
                    red = 0;
                }
            }
            if (canvas.keyPressed('G')) {
                green++;
                if (green > 255) {
                    green = 0;
                }
            }
            if (canvas.keyPressed('B')) {
                blue++;
                if (blue > 255) {
                    blue = 0;
                }
            }
        }
        else if (canvas.keyPressed(VK_CONTROL)) {
            if (canvas.keyPressed('R')) {
                red--;
                if (red < 0) {
                    red = 255;
                }
            }
            if (canvas.keyPressed('G')) {
                green--;
                if (green < 0) {
                    green = 255;
                }
            }
            if (canvas.keyPressed('B')) {
                blue--;
                if (blue < 0) {
                    blue = 255;
                }
            }
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

        image.load("./assets/Circle.png");
        for (int i = 0; i < image.width; i++) {
            for (int j = 0; j < image.height; j++) {
                if (!image.alphaAt(i, j)) {
                    continue;
                }

                unsigned char* pixel = image.at(i, j);
                canvas.draw(100 + i, 100 + j, red, green, blue);
            }
        }

        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
    }
    return 0;
}