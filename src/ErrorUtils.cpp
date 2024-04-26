//
// Created by Gazi on 4/20/2024.
//

#include "ErrorUtils.h"

#include <chrono>
#include <SDL_messagebox.h>

#include "Constants.h"
#include "LogUtils.h"

void ErrorUtils::display_last_sdl_error_and_quit(SDL_Window *window, const std::source_location& location) {
    display_error_and_quit(window, SDL_GetError(), location);
}

void ErrorUtils::display_error_and_quit(SDL_Window *window, const std::string &error, const std::source_location& location) {
    LogUtils::log_message(LogUtils::ERROR, error, location);
    if (window != nullptr) {
        SDL_SetWindowFullscreen(window, 0);
    }
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, Constants::project_error_title.c_str(), error.c_str(), window);
    exit(1);
}





