//
// Created by Gazi on 4/20/2024.
//

#include "Animation.h"

#include <iostream>
#include <SDL_image.h>

#include "ErrorUtils.h"

Animation::Animation(SDL_Window *window, SDL_Renderer* renderer, const std::string& sprites_folder, unsigned num_textures, unsigned num_frames_per_texture) {
    for (unsigned i = 0; i < num_textures; i++) {
        std::string image_path = sprites_folder + '/' + std::to_string(i) + ".png";
        SDL_Texture *texture = IMG_LoadTexture(renderer, image_path.c_str());
        if (texture == nullptr) {
            ErrorUtils::display_last_sdl_error_and_quit(window);
        }
        _textures.push_back(texture);
    }

    _num_frames_per_texture = num_frames_per_texture;
}

Animation::Animation(SDL_Window *window, SDL_Renderer* renderer, const std::string &gif_filename) {
    IMG_Animation* anim_surfaces = IMG_LoadAnimation(gif_filename.c_str());

    for (int i = 0; i < anim_surfaces->count; i++) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, anim_surfaces->frames[i]);
        if (texture == nullptr) {
            ErrorUtils::display_last_sdl_error_and_quit(window);
        }
        _textures.push_back(texture);
    }

    _num_frames_per_texture = anim_surfaces->count;

    IMG_FreeAnimation(anim_surfaces);
}

SDL_Texture* Animation::get_texture(unsigned frame_num) {
    unsigned texture_index = frame_num / _num_frames_per_texture;
    return _textures[texture_index];
}

unsigned Animation::get_num_textures() {
    return _textures.size();
}

unsigned Animation::get_num_frames() {
    return get_num_textures() * _num_frames_per_texture;
}



