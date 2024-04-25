//
// Created by Gazi on 4/21/2024.
//

#include "Constants.h"

std::vector<Animation> Constants::_anims;


void Constants::load_animations(SDL_Window *window, SDL_Renderer *renderer) {
    // Sword
    Animation sword_man_run(window, renderer, "resources/sword_man/run", 10, 3);
    _anims.push_back(sword_man_run);
    Animation sword_man_slash(window, renderer, "resources/sword_man/slash", 10, 3);
    _anims.push_back(sword_man_slash);
    Animation sword_man_hurt(window, renderer, "resources/sword_man/hurt", 10, 3);
    _anims.push_back(sword_man_hurt);

    // Gun
    Animation gun_man_run(window, renderer, "resources/gun_man/run", 10, 3);
    _anims.push_back(gun_man_run);
    Animation gun_man_shot(window, renderer, "resources/gun_man/shot", 10, 3);
    _anims.push_back(gun_man_shot);
    Animation gun_man_hurt(window, renderer, "resources/gun_man/hurt", 10, 3);
    _anims.push_back(gun_man_hurt);
}

Animation Constants::get_animation(Anim anim) {
    return _anims[anim];
}

