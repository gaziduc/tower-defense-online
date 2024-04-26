//
// Created by Gazi on 4/26/2024.
//

#ifndef ANIMATIONENTITY_H
#define ANIMATIONENTITY_H
#include "Animation.h"


class AnimationEntity: public Animation {
private:
    unsigned _texture_index;
public:
    AnimationEntity(SDL_Window *window, SDL_Renderer* renderer, const std::string &gif_filename);

    SDL_Texture* get_texture();
    void goto_next_texture_index();
};



#endif //ANIMATIONENTITY_H
