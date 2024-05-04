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
std::vector<SDL_Point> Constants::_anim_sizes;

void Constants::load_animations(SDL_Window *window, SDL_Renderer *renderer) {
    // Sword
    Animation sword_man_run(window, renderer, SWORD_MAN_RES + "/run", 10, 3);
    _anims.push_back(std::make_shared<Animation>(sword_man_run));
    Animation sword_man_slash(window, renderer, SWORD_MAN_RES + "/slash", 10, 3);
    _anims.push_back(std::make_shared<Animation>(sword_man_slash));
    Animation sword_man_hurt(window, renderer, SWORD_MAN_RES + "/hurt", 10, 3);
    _anims.push_back(std::make_shared<Animation>(sword_man_hurt));
    AnimationEntity sword_man_idle(window, renderer, SWORD_MAN_RES + "/idle", 10, 3);
    _anims.push_back(std::make_shared<AnimationEntity>(sword_man_idle));

    // Gun
    Animation gun_man_run(window, renderer, GUN_MAN_RES + "/run", 10, 3);
    _anims.push_back(std::make_shared<Animation>(gun_man_run));
    Animation gun_man_shot(window, renderer, GUN_MAN_RES + "/shot", 10, 3);
    _anims.push_back(std::make_shared<Animation>(gun_man_shot));
    Animation gun_man_hurt(window, renderer, GUN_MAN_RES + "/hurt", 10, 3);
    _anims.push_back(std::make_shared<Animation>(gun_man_hurt));
    AnimationEntity gun_man_idle(window, renderer, GUN_MAN_RES + "/idle", 10, 3);
    _anims.push_back(std::make_shared<AnimationEntity>(gun_man_idle));

    // Coin
    AnimationEntity coin(window, renderer, "resources/images/coin.webp");
    _anims.push_back(std::make_shared<AnimationEntity>(coin));

    AnimationEntity health(window, renderer, "resources/images/health.webp");
    _anims.push_back(std::make_shared<AnimationEntity>(health));

    load_animations_sizes();
}

void Constants::load_animations_sizes() {
    for (std::shared_ptr<Animation>& animation : _anims) {
        int w = 0;
        int h = 0;
        SDL_QueryTexture(animation->get_texture(0), nullptr, nullptr, &w, &h);
        _anim_sizes.push_back({.x = w, .y = h});
    }
}

SDL_Point Constants::get_animation_size(Anim anim) {
    return _anim_sizes[anim];
}

void Constants::load_fonts(SDL_Window *window) {
    TTF_Font* font = TTF_OpenFont("resources/fonts/bebasneue-regular.ttf", 70);
    if (font == nullptr) {
        ErrorUtils::display_last_ttf_error_and_quit(window);
    }

    _fonts.push_back(font);
}

std::shared_ptr<Animation> Constants::get_animation(Anim anim) {
    return _anims[anim];
}

TTF_Font* Constants::get_font(Font font) {
    return _fonts[font];
}

