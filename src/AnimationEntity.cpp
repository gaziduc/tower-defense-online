//
// Created by Gazi on 4/26/2024.
//

#include "AnimationEntity.h"

AnimationEntity::AnimationEntity(SDL_Window *window, SDL_Renderer* renderer, const std::string &gif_filename): Animation(window, renderer, gif_filename) {
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