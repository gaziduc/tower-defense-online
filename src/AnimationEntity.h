//
// Created by Gazi on 4/26/2024.
//

#ifndef ANIMATIONENTITY_H
#define ANIMATIONENTITY_H
#include "Animation.h"


class AnimationEntity: public Animation {
private:
    unsigned _texture_index;
    float _ratio;

public:
    AnimationEntity(SDL_Window *window, SDL_Renderer* renderer, const std::string& sprites_folder, unsigned num_textures, unsigned num_frames_per_texture);
    AnimationEntity(SDL_Window *window, SDL_Renderer* renderer, const std::string &anim_filename);

    SDL_Texture* get_texture();
    void goto_next_texture_index();
    float get_ratio();
};



#endif //ANIMATIONENTITY_H
