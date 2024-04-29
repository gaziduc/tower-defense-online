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

void RenderUtils::render_text_shaded(SDL_Window* window, SDL_Renderer* renderer, std::string& text, SDL_FRect* dst_pos, SDL_Color fg_color, SDL_Color bg_color, float ratio_x, float ratio_y) {
    if (text.empty()) {
        return;
    }
    SDL_Surface *text_surface = TTF_RenderText_Shaded(Constants::get_font(Constants::NORMAL), text.c_str(), fg_color, bg_color);
    if (text_surface == nullptr) {
        ErrorUtils::display_last_sdl_error_and_quit(window);
    }
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    dst_pos->w = static_cast<float>(text_surface->w) * ratio_x;
    dst_pos->h = static_cast<float>(text_surface->h) * ratio_y;

    if (dst_pos->x == Constants::SDL_POS_X_CENTERED) {
        int window_size_x;
        SDL_GetWindowSize(window, &window_size_x, nullptr);
        dst_pos->x = window_size_x / 2 - dst_pos->w / 2;
    } else {
        dst_pos->x *= ratio_x;
    }
    dst_pos->y *= ratio_y;

    SDL_FRect text_pos = { .x = dst_pos->x, .y = dst_pos->y, .w = dst_pos->w, .h = dst_pos->h };
    SDL_RenderCopyF(renderer, text_texture, nullptr, &text_pos);
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
}

void RenderUtils::render_animation_entity_with_text(SDL_Window* window, SDL_Renderer* renderer, Constants::Anim anim, std::string& text, SDL_FRect* dst_pos, float ratio_x, float ratio_y) {
    SDL_FRect old_dst_pos = {.x = dst_pos->x, .y = dst_pos->y, .w = dst_pos->w, .h = dst_pos->h};
    dst_pos->x *= ratio_x;
    dst_pos->y *= ratio_y;
    dst_pos->w *= ratio_x;
    dst_pos->h *= ratio_y;
    render_animation_entity(renderer, anim, dst_pos);
    dst_pos->x = old_dst_pos.x + old_dst_pos.w + 20;
    dst_pos->y = old_dst_pos.y;
    dst_pos->w = old_dst_pos.w;
    dst_pos->h = old_dst_pos.h;
    render_text_shaded(window, renderer, text, dst_pos, {.r = 0, .g = 0, .b = 0, .a = SDL_ALPHA_OPAQUE},  {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE}, ratio_x, ratio_y);
    dst_pos->x = old_dst_pos.x;
    dst_pos->y = old_dst_pos.y;
}
