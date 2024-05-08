//
// Created by Gazi on 4/21/2024.
//

#include "Constants.h"

#include <memory>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "AnimationEntity.h"
#include "Entity.h"
#include "ErrorUtils.h"

std::vector<std::shared_ptr<Animation>> Constants::_anims;
std::vector<TTF_Font*> Constants::_fonts;
std::vector<SDL_Point> Constants::_anim_sizes;
std::vector<Mix_Chunk*> Constants::_chunks;

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

    AnimationEntity coin(window, renderer, "resources/images/coin.webp");
    _anims.push_back(std::make_shared<AnimationEntity>(coin));

    AnimationEntity health(window, renderer, "resources/images/health.webp");
    _anims.push_back(std::make_shared<AnimationEntity>(health));

    AnimationEntity arrow(window, renderer, "resources/images/arrow.gif");
    _anims.push_back(std::make_shared<AnimationEntity>(arrow));
}

std::shared_ptr<Animation> Constants::get_animation(Anim anim) {
    return _anims[anim];
}

void Constants::load_fonts(SDL_Window *window) {
    TTF_Font* font = TTF_OpenFont("resources/fonts/bebasneue-regular.ttf", 70);
    if (font == nullptr) {
        ErrorUtils::display_last_ttf_error_and_quit(window);
    }

    _fonts.push_back(font);
}

TTF_Font* Constants::get_font(Font font) {
    return _fonts[font];
}

void Constants::load_chunks(SDL_Window *window) {
    Mix_Chunk* slash = Mix_LoadWAV("resources/audio/slash.wav");
    if (slash == nullptr) {
        ErrorUtils::display_last_mix_error_and_quit(window);
    }
    _chunks.push_back(slash);

    Mix_Chunk* shoot = Mix_LoadWAV("resources/audio/shoot.wav");
    if (shoot == nullptr) {
        ErrorUtils::display_last_mix_error_and_quit(window);
    }
    _chunks.push_back(shoot);

    Mix_Chunk* drop_entity = Mix_LoadWAV("resources/audio/drop_entity.wav");
    if (drop_entity == nullptr) {
        ErrorUtils::display_last_mix_error_and_quit(window);
    }
    _chunks.push_back(drop_entity);

    Mix_Chunk* error = Mix_LoadWAV("resources/audio/error.wav");
    if (error == nullptr) {
        ErrorUtils::display_last_mix_error_and_quit(window);
    }
    _chunks.push_back(error);
}

void Constants::play_entity_attack_chunk(Entity::EntityType type) {
    if (_chunks.empty()) {
        return; // server side
    }

    Mix_PlayChannel(-1, _chunks[type], 0);
}

void Constants::play_drop_entity_chunk() {
    Mix_PlayChannel(-1, _chunks[2], 0);
}

void Constants::play_error_chunk() {
    Mix_PlayChannel(-1, _chunks[3], 0);
}


