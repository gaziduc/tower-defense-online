//
// Created by Gazi on 4/20/2024.
//

#ifndef ERRORUTILS_H
#define ERRORUTILS_H
#include <SDL.h>
#include <source_location>
#include <string>


class ErrorUtils {
public:
    // Display errors
    static void display_last_sdl_error_and_quit(SDL_Window *window, const std::source_location& location = std::source_location::current());
    static void display_last_img_error_and_quit(SDL_Window *window, const std::source_location& location = std::source_location::current());
    static void display_last_mix_error_and_quit(SDL_Window *window, const std::source_location& location = std::source_location::current());
    static void display_last_net_error_and_quit(SDL_Window *window, const std::source_location& location = std::source_location::current());
    static void display_last_ttf_error_and_quit(SDL_Window *window, const std::source_location& location = std::source_location::current());

    static void display_error_and_quit(SDL_Window *window, const std::string& error, const std::source_location& location = std::source_location::current());
};



#endif //ERRORUTILS_H
