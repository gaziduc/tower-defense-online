//
// Created by Gazi on 4/20/2024.
//

#ifndef ANIMATION_H
#define ANIMATION_H
#include <SDL.h>
#include <string>
#include <vector>


class Animation {
protected:
    std::vector<SDL_Texture*> _textures;
    unsigned _num_frames_per_texture;

public:
    // Constructors
    Animation(SDL_Window *window, SDL_Renderer* renderer, const std::string &sprites_folder, unsigned num_textures, unsigned num_frames_per_texture);
    Animation(SDL_Window *window, SDL_Renderer* renderer, const std::string &anim_filename);

    // Destructors
    virtual ~Animation() = default;

    SDL_Texture* get_texture(unsigned texture_index);
    unsigned get_num_textures();
    unsigned get_num_frames();
};



#endif //ANIMATION_H
