//
// Created by Gazi on 4/27/2024.
//

#ifndef RENDERUTILS_H
#define RENDERUTILS_H
#include "Constants.h"


class RenderUtils {
public:
    static void render_animation_entity(SDL_Renderer* renderer, Constants::Anim anim, SDL_FRect* dst_pos);
    static void render_text_shaded(SDL_Window* window, SDL_Renderer* renderer, std::string& text, SDL_FRect* dst_pos, SDL_Color fg_color, SDL_Color bg_color, float ratio_x, float ratio_y);
    static void render_animation_entity_with_text(SDL_Window* window, SDL_Renderer* renderer, Constants::Anim anim, std::string& text, SDL_FRect* dst_pos, float ratio_x, float ratio_y);
};



#endif //RENDERUTILS_H
