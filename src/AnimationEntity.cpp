//
// Created by Gazi on 4/26/2024.
//

#include "AnimationEntity.h"

AnimationEntity::AnimationEntity(SDL_Window *window, SDL_Renderer* renderer, const std::string& sprites_folder, unsigned num_textures, unsigned num_frames_per_texture)
    : Animation(window, renderer, sprites_folder, num_textures, num_frames_per_texture) {
    _texture_index = 0;
    SDL_Point point = {.x = 0, .y = 0};
    SDL_QueryTexture(_textures[_texture_index], nullptr, nullptr, &point.x, &point.y);
    _ratio = static_cast<float>(point.x) / static_cast<float>(point.y);
}

AnimationEntity::AnimationEntity(SDL_Window *window, SDL_Renderer* renderer, const std::string &anim_filename): Animation(window, renderer, anim_filename) {
    _texture_index = 0;
}

SDL_Texture* AnimationEntity::get_texture() {
    return _textures[_texture_index];
}

void AnimationEntity::goto_next_texture_index() {
    _texture_index++;
    if (_texture_index >= _textures.size()) {
        _texture_index = 0;
    }
}

float AnimationEntity::get_ratio() {
    return _ratio;
}