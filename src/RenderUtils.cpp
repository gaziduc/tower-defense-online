//
// Created by Gazi on 4/27/2024.
//

#include "RenderUtils.h"

#include <SDL_ttf.h>

#include "AnimationEntity.h"
#include "ErrorUtils.h"

void RenderUtils::render_animation_entity(SDL_Renderer* renderer, Constants::Anim anim, SDL_FRect* dst_pos) {
    AnimationEntity* animation_entity = dynamic_cast<AnimationEntity*>(Constants::get_animation(anim).get());
    SDL_RenderCopyF(renderer, animation_entity->get_texture(), nullptr, dst_pos);
    animation_entity->goto_next_texture_index();
}

void RenderUtils::render_text_shaded(SDL_Window* window, SDL_Renderer* renderer, std::string& text, SDL_FRect* dst_pos, SDL_Color fg_color, SDL_Color bg_color) {
    if (text.empty()) {
        return;
    }
    SDL_Surface *text_surface = TTF_RenderText_Shaded(Constants::get_font(Constants::NORMAL), text.c_str(), fg_color, bg_color);
    if (text_surface == nullptr) {
        ErrorUtils::display_last_sdl_error_and_quit(window);
    }
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_FRect text_pos = { .x = dst_pos->x, .y = dst_pos->y, .w = static_cast<float>(text_surface->w), .h = static_cast<float>(text_surface->h) };
    SDL_RenderCopyF(renderer, text_texture, nullptr, &text_pos);
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
}

void RenderUtils::render_animation_entity_with_text(SDL_Window* window, SDL_Renderer* renderer, Constants::Anim anim, std::string& text, SDL_FRect* dst_pos) {
    render_animation_entity(renderer, anim, dst_pos);
    float old_dst_pos_x = dst_pos->x;
    dst_pos->x += dst_pos->w + 20;
    render_text_shaded(window, renderer, text, dst_pos, {.r = 0, .g = 0, .b = 0, .a = SDL_ALPHA_OPAQUE},  {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE});
    dst_pos->x = old_dst_pos_x;
}
