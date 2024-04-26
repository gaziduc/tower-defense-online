//
// Created by Gazi on 4/20/2024.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <memory>
#include <string>

#include "Animation.h"

class Constants {
private:
    static std::vector<std::shared_ptr<Animation>> _anims;
public:
    enum Anim {
        // Sword
        SWORD_MAN_RUN = 0,
        SWORD_MAN_SLASH,
        SWORD_MAN_HURT,
        // Gun
        GUN_MAN_RUN,
        GUN_MAN_SHOT,
        GUN_MAN_HURT,

        COIN,

        NUM_ANIM
    };

    inline static const std::string project_name = "Tower Defense Online";
    inline static const std::string project_error_title = project_name + " Error";
    static constexpr unsigned num_state_per_entity = 3;
    static constexpr unsigned num_battle_rows = 8;
    static constexpr float row_height = 110;
    static constexpr float battlefield_height = num_battle_rows * row_height;
    static constexpr float entity_height = 100;
    static constexpr float health_bar_width = 80;
    static constexpr float health_bar_height = 5;

    static void load_animations(SDL_Window *window, SDL_Renderer *renderer);
    static std::shared_ptr<Animation> get_animation(Anim anim);
};

#endif //CONSTANTS_H
