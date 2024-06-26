//
// Created by Gazi on 4/27/2024.
//

#ifndef RENDERUTILS_H
#define RENDERUTILS_H
#include "Constants.h"


class RenderUtils {
public:
    static void render_animation_entity(SDL_Renderer* renderer, Constants::Anim anim, SDL_FRect* dst_pos);
    static void render_animation_entity_ex(SDL_Renderer* renderer, Constants::Anim anim, SDL_FRect* dst_pos, double angle, SDL_FPoint* center, SDL_RendererFlip flip);
    static void render_text_prompt(SDL_Window* window, SDL_Renderer* renderer, std::string& text, SDL_FRect* dst_pos, Constants::Font font, SDL_Color fg_color, float ratio_x, float ratio_y);
    static float render_text(SDL_Window* window, SDL_Renderer* renderer, std::string& text, SDL_FRect* dst_pos, Constants::Font font, SDL_Color fg_color, float ratio_x, float ratio_y);
    static void render_animation_entity_with_text(SDL_Window* window, SDL_Renderer* renderer, Constants::Anim anim, std::string& text, Constants::Font font, SDL_FRect* dst_pos, float ratio_x, float ratio_y);
};



#endif //RENDERUTILS_H
