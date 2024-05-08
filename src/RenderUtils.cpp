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

void RenderUtils::render_animation_entity_ex(SDL_Renderer* renderer, Constants::Anim anim, SDL_FRect* dst_pos, double angle, SDL_FPoint* center, SDL_RendererFlip flip) {
    AnimationEntity* animation_entity = dynamic_cast<AnimationEntity*>(Constants::get_animation(anim).get());
    SDL_RenderCopyExF(renderer, animation_entity->get_texture(), nullptr, dst_pos, angle, center, flip);
    animation_entity->goto_next_texture_index();
}

void RenderUtils::render_text_prompt(SDL_Window* window, SDL_Renderer* renderer, std::string& text, SDL_FRect* dst_pos, SDL_Color fg_color, float ratio_x, float ratio_y) {
    float w = render_text(window, renderer, text, dst_pos, fg_color,  ratio_x, ratio_y);
    if (SDL_GetTicks64() % 1000 < 500) {
        if (dst_pos->x == Constants::SDL_POS_X_CENTERED) {
            dst_pos->x = Constants::VIEWPORT_WIDTH / 2 + w / 2;
        }

        std::string prompt("_");
        render_text(window, renderer, prompt, dst_pos, fg_color, ratio_x, ratio_y);
    }
}

float RenderUtils::render_text(SDL_Window* window, SDL_Renderer* renderer, std::string& text, SDL_FRect* dst_pos, SDL_Color fg_color, float ratio_x, float ratio_y) {
    if (text.empty()) {
        return 0;
    }
    SDL_Surface *text_surface = TTF_RenderText_Blended(Constants::get_font(Constants::NORMAL), text.c_str(), fg_color);
    if (text_surface == nullptr) {
        ErrorUtils::display_last_sdl_error_and_quit(window);
    }
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    SDL_FRect text_pos = { .x = dst_pos->x, .y = dst_pos->y, .w = dst_pos->w, .h = dst_pos->h };
    text_pos.w = static_cast<float>(text_surface->w) * ratio_x;
    text_pos.h = static_cast<float>(text_surface->h) * ratio_y;

    if (dst_pos->x == Constants::SDL_POS_X_CENTERED) {
        text_pos.x = Constants::VIEWPORT_WIDTH / 2 - text_surface->w / 2;
    }

    text_pos.x *= ratio_x;
    text_pos.y *= ratio_y;


    SDL_RenderCopyF(renderer, text_texture, nullptr, &text_pos);
    SDL_DestroyTexture(text_texture);

    int w = text_surface->w;
    SDL_FreeSurface(text_surface);

    return w;
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
    render_text(window, renderer, text, dst_pos, {.r = 0, .g = 0, .b = 0, .a = SDL_ALPHA_OPAQUE}, ratio_x, ratio_y);
    dst_pos->x = old_dst_pos.x;
    dst_pos->y = old_dst_pos.y;
}
