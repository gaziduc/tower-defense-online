//
// Created by Gazi on 4/21/2024.
//

#include "Constants.h"

#include <memory>
#include <SDL_image.h>

#include "AnimationEntity.h"

std::vector<std::shared_ptr<Animation>> Constants::_anims;


void Constants::load_animations(SDL_Window *window, SDL_Renderer *renderer) {
    // Sword
    Animation sword_man_run(window, renderer, "resources/images/sword_man/run", 10, 3);
    _anims.push_back(std::make_shared<Animation>(sword_man_run));
    Animation sword_man_slash(window, renderer, "resources/images/sword_man/slash", 10, 3);
    _anims.push_back(std::make_shared<Animation>(sword_man_slash));
    Animation sword_man_hurt(window, renderer, "resources/images/sword_man/hurt", 10, 3);
    _anims.push_back(std::make_shared<Animation>(sword_man_hurt));

    // Gun
    Animation gun_man_run(window, renderer, "resources/images/gun_man/run", 10, 3);
    _anims.push_back(std::make_shared<Animation>(gun_man_run));
    Animation gun_man_shot(window, renderer, "resources/images/gun_man/shot", 10, 3);
    _anims.push_back(std::make_shared<Animation>(gun_man_shot));
    Animation gun_man_hurt(window, renderer, "resources/images/gun_man/hurt", 10, 3);
    _anims.push_back(std::make_shared<Animation>(gun_man_hurt));

    // Coin
    AnimationEntity coin(window, renderer, "resources/images/coin.gif");
    _anims.push_back(std::make_shared<AnimationEntity>(coin));
}

std::shared_ptr<Animation> Constants::get_animation(Anim anim) {
    return _anims[anim];
}

