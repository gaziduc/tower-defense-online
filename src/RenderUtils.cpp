//
// Created by Gazi on 4/27/2024.
//

#include "RenderUtils.h"

#include <SDL_ttf.h>

#include "AnimationEntity.h"

void RenderUtils::render_animation_entity(SDL_Renderer* renderer, Constants::Anim anim, SDL_FRect* dst_pos) {
    AnimationEntity* animation_entity = dynamic_cast<AnimationEntity*>(Constants::get_animation(anim).get());
    SDL_RenderCopyF(renderer, animation_entity->get_texture(), nullptr, dst_pos);
    animation_entity->goto_next_texture_index();
}

void RenderUtils::render_animation_entity_with_text(SDL_Renderer* renderer, Constants::Anim anim, std::string& text, SDL_FRect* dst_pos) {
    render_animation_entity(renderer, anim, dst_pos);

    SDL_FRect old_pos = {.x = dst_pos->x, .y = dst_pos->y, .w = dst_pos->w, .h = dst_pos->h};

    SDL_Surface *text_surface = TTF_RenderText_Shaded(Constants::get_font(Constants::NORMAL), text.c_str(), {.r = 0, .g = 0, .b = 0, .a = SDL_ALPHA_OPAQUE}, {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE});
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    dst_pos->x += 100;
    dst_pos->y = dst_pos->y + dst_pos->h / 2 - text_surface->h / 2;
    dst_pos->w = text_surface->w;
    dst_pos->h = text_surface->h;
    SDL_RenderCopyF(renderer, text_texture, nullptr, dst_pos);
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);

    (*dst_pos) = {.x = old_pos.x, .y = old_pos.y, .w = old_pos.w, .h = old_pos.h};
}
