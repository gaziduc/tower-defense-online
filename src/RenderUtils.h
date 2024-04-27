//
// Created by Gazi on 4/27/2024.
//

#ifndef RENDERUTILS_H
#define RENDERUTILS_H
#include "Constants.h"


class RenderUtils {
public:
    static void render_animation_entity(SDL_Renderer* renderer, Constants::Anim anim, SDL_FRect* dst_pos);
    static void render_animation_entity_with_text(SDL_Renderer* renderer, Constants::Anim anim, std::string& text, SDL_FRect* dst_pos);
};



#endif //RENDERUTILS_H
