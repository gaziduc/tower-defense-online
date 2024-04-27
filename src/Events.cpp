//
// Created by Gazi on 4/21/2024.
//

#include "Events.h"

#include <SDL.h>


bool Events::is_quit() {
    return _quit;
}

bool Events::is_key_down(const int key) {
    return _keys[key];
}

void Events::press_up_key(const int key) {
    _keys[key] = false;
}

bool Events::is_mouse_button_down(const int mouse_button) {
    return _mouse_buttons[mouse_button];
}

void Events::release_mouse_button(const int mouse_button) {
    _mouse_buttons[mouse_button] = false;
}

SDL_Point* Events::get_cursor_pos() {
    return &_cursor_pos;
}


bool Events::is_enter_down() {
    return is_key_down(SDL_SCANCODE_RETURN) || is_key_down(SDL_SCANCODE_KP_ENTER);
}

bool Events::is_escape_down() {
    return is_key_down(SDL_SCANCODE_ESCAPE);
}

std::string Events::get_text() {
    return _text;
}

bool Events::is_mouse_wheel_up() {
    return _mouse_wheel_y > 0;
}

bool Events::is_mouse_wheel_down() {
    return _mouse_wheel_y < 0;
}

void Events::update_events(SDL_Window *window) {
    _quit = false;
    _text = "";
    _mouse_wheel_y = 0;

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // Quit event
            case SDL_QUIT:
                _quit = true;
            break;

            // Keyboard events
            case SDL_KEYDOWN:
                _keys[event.key.keysym.scancode] = true;
            break;
            case SDL_KEYUP:
                _keys[event.key.keysym.scancode] = false;
            break;

            // Mouse event
            case SDL_MOUSEBUTTONDOWN:
                _mouse_buttons[event.button.button] = true;
            break;

            case SDL_MOUSEBUTTONUP:
                _mouse_buttons[event.button.button] = false;
            break;

            case SDL_MOUSEMOTION:
                _cursor_pos.x = event.motion.x;
                _cursor_pos.y = event.motion.y;
            break;

            case SDL_MOUSEWHEEL:
                _mouse_wheel_y = event.wheel.y;
            break;

            // Text
            case SDL_TEXTINPUT:
                _text = std::string(event.text.text);
            break;
            default:
                break;
        }
    }
}