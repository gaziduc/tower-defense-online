//
// Created by Gazi on 4/20/2024.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <memory>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string>

#include "Animation.h"
#include "Entity.h"
#include "LogUtils.h"

class Constants {
private:
    static std::vector<std::shared_ptr<Animation>> _anims;
    static std::vector<SDL_Point> _anim_sizes;
    static std::vector<TTF_Font*> _fonts;
    static std::vector<Mix_Chunk*> _chunks;

public:
    enum Anim {
        // Sword
        SWORD_MAN_RUN = 0,
        SWORD_MAN_SLASH,
        SWORD_MAN_HURT,
        SWORD_MAN_IDLE,
        // Gun
        GUN_MAN_RUN,
        GUN_MAN_SHOT,
        GUN_MAN_HURT,
        GUN_MAN_IDLE,
        // Icons
        COIN,
        HEALTH,
        ARROW,
        MINI_COINS,

        NUM_ANIM
    };

    enum Font {
        EXTRA_SMALL = 0,
        SMALL,
        NORMAL,
        NUMBER_SMALL,
        NUMBER_NORMAL,
        NUM_FONT
    };

    inline static const std::string PROJECT_NAME = "Tower Defense Online";
    inline static const std::string PROJECT_ERROR_TITLE = PROJECT_NAME + " Error";
    inline static const std::string PROJECT_NAME_SERVER = PROJECT_NAME + " Server";
    inline static const std::string PROJECT_SERVER_ERROR_TITLE = PROJECT_NAME_SERVER + " Error";
    static constexpr int VIEWPORT_WIDTH = 1920;
    static constexpr int VIEWPORT_HEIGHT = 1080;
    static constexpr unsigned NUM_STATE_PER_ENTITY = 4;
    static constexpr unsigned NUM_BATTLE_ROWS = 8;
    static constexpr float ROW_HEIGHT = 110;
    static constexpr float BATTLEFIELD_HEIGHT = NUM_BATTLE_ROWS * ROW_HEIGHT;
    static constexpr float ENTITY_HEIGHT = 100;
    static constexpr float HEALTH_BAR_WIDTH = 80;
    static constexpr float HEALTH_BAR_HEIGHT = 5;
    static constexpr float SDL_POS_X_CENTERED = -420000;
    static constexpr int INITIAL_PLAYER_HEALTH = 1000;
    static constexpr int INITIAL_PLAYER_MONEY = 200;

    // Network
    static constexpr int MESSAGE_START = 0;
    static constexpr int MESSAGE_NEW_ENTITY = 1;
    static constexpr int MESSAGE_STATE_BEGIN = 2;
    static constexpr int MESSAGE_ENTITY = 3;
    static constexpr int MESSAGE_STATE_END = 4;
    static constexpr int MESSAGE_TIME = 5;
    static constexpr int MESSAGE_ENTITY_DEATH = 6;

    // Animations
    inline static const std::string SWORD_MAN_RES = "resources/images/sword_man";
    inline static const std::string GUN_MAN_RES = "resources/images/gun_man";

    // Logs
    static constexpr LogUtils::Level LOG_LEVEL = LogUtils::WARN;

    static void load_animations(SDL_Window *window, SDL_Renderer *renderer);
    static std::shared_ptr<Animation> get_animation(Anim anim);

    static void load_fonts(SDL_Window *window);
    static TTF_Font* get_font(Font font);

    static void load_chunks(SDL_Window *window);
    static void play_entity_attack_chunk(Entity::EntityType type);
    static void play_drop_entity_chunk();
    static void play_error_chunk();
};

#endif //CONSTANTS_H
