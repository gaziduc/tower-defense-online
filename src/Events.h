//
// Created by Gazi on 4/21/2024.
//

#ifndef EVENTS_H
#define EVENTS_H
#include <SDL.h>
#include <string>


class Events {
private:
    bool _quit = false;
    bool _keys[SDL_NUM_SCANCODES] = { false };
    std::string _text;
    bool _mouse_buttons[5] = { false };
    SDL_Point _cursor_pos = { .x = 0, .y = 0 };
    int _mouse_wheel_y = 0;

public:
    bool is_quit();
    bool is_key_down(int key);
    void press_up_key(int key);
    bool is_mouse_button_down(int mouse_button);
    void release_mouse_button(int mouse_button);
    SDL_Point* get_cursor_pos();
    bool is_enter_down();
    bool is_escape_down();
    std::string get_text();
    bool is_mouse_wheel_up();
    bool is_mouse_wheel_down();

    void update_events(SDL_Window *window);
};



#endif //EVENTS_H
