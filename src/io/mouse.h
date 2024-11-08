#ifndef MOUSE_H
#define MOUSE_H

#include <iostream>
#include <glad/glad.h>
#include <SDL2/SDL.h>

/*
    mouse class to handle mouse callbacks
*/

class Mouse {
public:
    /*
        callbacks
    */

    // cursor position changed
    static void cursorPosCallback(SDL_Event event);
    // mouse button state changed
    static void mouseButtonCallback(SDL_Event event);
    // scroll wheel moved
    static void mouseWheelCallback(SDL_Event event);

    // any mouse button is being held down
    static void mouseButtonRepeat();

    /*
        accessors
    */

    // get mouse x position
    static double getMouseX();
    // get mouse y position
    static double getMouseY();

    // get mouse change in x
    static double getDX();
    // get mouse change in y
    static double getDY();

    // get scroll value in x
    static double getScrollDX();
    // get scroll value in y
    static double getScrollDY();

    // get button state
    static bool button(Uint8 button);
    // return if button changed then reset it in the changed array
    static bool buttonChanged(Uint8 button);
    // return if button changed and is now up
    static bool buttonWentUp(Uint8 button);
    // return if button changed and is now down
    static bool buttonWentDown(Uint8 button);

private:
    /*
        static mouse values
    */

    // x posiiton
    static double x;
    // y position
    static double y;

    // previous x position
    static double lastX;
    // previous y position
    static double lastY;

    // change in x position from lastX
    static double dx;
    // change in y position from lastY
    static double dy;

    // change in scroll x
    static double scrollDx;
    // change in scroll y
    static double scrollDy;

    // if this is the first change in the mouse position
    static bool firstMouse;

    // button state array (true for down, false for up)
    static bool buttons[];
    // button changed array (true if changed)
    static bool buttonsChanged[];
};

#endif