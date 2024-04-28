//
// Created by Gazi on 4/20/2024.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <memory>
#include <SDL_ttf.h>
#include <string>

#include "Animation.h"

class Constants {
private:
    static std::vector<std::shared_ptr<Animation>> _anims;
    static std::vector<TTF_Font*> _fonts;

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

        NUM_ANIM
    };

    enum Font {
        NORMAL,

        NUM_FONT
    };

    inline static const std::string PROJECT_NAME = "Tower Defense Online";
    inline static const std::string PROJECT_ERROR_TITLE = PROJECT_NAME + " Error";
    static constexpr unsigned VIEWPORT_WIDTH = 1920;
    static constexpr unsigned VIEWPORT_HEIGHT = 1080;
    static constexpr unsigned NUM_STATE_PER_ENTITY = 4;
    static constexpr unsigned NUM_BATTLE_ROWS = 8;
    static constexpr float ROW_HEIGHT = 110;
    static constexpr float BATTLEFIELD_HEIGHT = NUM_BATTLE_ROWS * ROW_HEIGHT;
    static constexpr float ENTITY_HEIGHT = 100;
    static constexpr float HEALTH_BAR_WIDTH = 80;
    static constexpr float HEALTH_BAR_HEIGHT = 5;
    // If you change the following lines, update src/server/ServerConstants.h
    static constexpr int INITIAL_PLAYER_HEALTH = 500;
    static constexpr int INITIAL_PLAYER_MONEY = 200;
    static constexpr int MAX_MESSAGE_SIZE = 1024;
    static constexpr char WORD_DELIMITER = ' ';
    static constexpr char MESSAGE_DELIMITER = '\n';
    inline static const std::string MESSAGE_NEW_ENTITY = "NEW_ENTITY";
    inline static const std::string MESSAGE_START = "START";

    static void load_animations(SDL_Window *window, SDL_Renderer *renderer);
    static void load_fonts(SDL_Window *window);
    static std::shared_ptr<Animation> get_animation(Anim anim);
    static TTF_Font* get_font(Font font);
};

#endif //CONSTANTS_H
