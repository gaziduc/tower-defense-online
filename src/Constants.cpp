//
// Created by Gazi on 4/21/2024.
//

#include "Constants.h"

#include <memory>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "AnimationEntity.h"
#include "ErrorUtils.h"

std::vector<std::shared_ptr<Animation>> Constants::_anims;
std::vector<TTF_Font*> Constants::_fonts;

void Constants::load_animations(SDL_Window *window, SDL_Renderer *renderer) {
    // Sword
    Animation sword_man_run(window, renderer, "resources/images/sword_man/run", 10, 3);
    _anims.push_back(std::make_shared<Animation>(sword_man_run));
    Animation sword_man_slash(window, renderer, "resources/images/sword_man/slash", 10, 3);
    _anims.push_back(std::make_shared<Animation>(sword_man_slash));
    Animation sword_man_hurt(window, renderer, "resources/images/sword_man/hurt", 10, 3);
    _anims.push_back(std::make_shared<Animation>(sword_man_hurt));
    AnimationEntity sword_man_idle(window, renderer, "resources/images/sword_man/idle", 10, 3);
    _anims.push_back(std::make_shared<AnimationEntity>(sword_man_idle));

    // Gun
    Animation gun_man_run(window, renderer, "resources/images/gun_man/run", 10, 3);
    _anims.push_back(std::make_shared<Animation>(gun_man_run));
    Animation gun_man_shot(window, renderer, "resources/images/gun_man/shot", 10, 3);
    _anims.push_back(std::make_shared<Animation>(gun_man_shot));
    Animation gun_man_hurt(window, renderer, "resources/images/gun_man/hurt", 10, 3);
    _anims.push_back(std::make_shared<Animation>(gun_man_hurt));
    AnimationEntity gun_man_idle(window, renderer, "resources/images/gun_man/idle", 10, 3);
    _anims.push_back(std::make_shared<AnimationEntity>(gun_man_idle));

    // Coin
    AnimationEntity coin(window, renderer, "resources/images/coin.webp");
    _anims.push_back(std::make_shared<AnimationEntity>(coin));

    AnimationEntity health(window, renderer, "resources/images/health.webp");
    _anims.push_back(std::make_shared<AnimationEntity>(health));
}

void Constants::load_fonts(SDL_Window *window) {
    TTF_Font* font = TTF_OpenFont("resources/fonts/bebasneue-regular.ttf", 70);
    if (font == nullptr) {
        ErrorUtils::display_last_sdl_error_and_quit(window);
    }

    _fonts.push_back(font);
}

std::shared_ptr<Animation> Constants::get_animation(Anim anim) {
    return _anims[anim];
}

TTF_Font* Constants::get_font(Font font) {
    return _fonts[font];
}

