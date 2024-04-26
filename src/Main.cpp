#include <map>
#include <optional>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "Animation.h"
#include "AnimationEntity.h"
#include "Constants.h"
#include "Entity.h"
#include "ErrorUtils.h"
#include "Events.h"
#include "framerate/SDL2_framerate.h"

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        ErrorUtils::display_last_sdl_error_and_quit(nullptr);
    }

    SDL_Window *window = SDL_CreateWindow(Constants::project_name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (window == nullptr) {
        ErrorUtils::display_last_sdl_error_and_quit(nullptr);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        ErrorUtils::display_last_sdl_error_and_quit(window);
    }

    int img_flags = IMG_INIT_PNG;
    int img_initted_flags = IMG_Init(img_flags);
    if ((img_initted_flags & img_flags) != img_flags) {
        ErrorUtils::display_last_sdl_error_and_quit(window);
    }

    if (TTF_Init() == -1) {
        ErrorUtils::display_last_sdl_error_and_quit(window);
    };

    Constants::load_animations(window, renderer);
    SDL_Texture *background = IMG_LoadTexture(renderer, "resources/images/backgrounds/0.png");
    if (background == nullptr) {
        ErrorUtils::display_last_sdl_error_and_quit(window);
    }

    TTF_Font* font = TTF_OpenFont("resources/fonts/bebasneue-regular.ttf", 70);
    if (font == nullptr) {
        ErrorUtils::display_last_sdl_error_and_quit(window);
    }

    FPSmanager fps_manager;
    SDL_initFramerate(&fps_manager);
    SDL_setFramerate(&fps_manager, 60);

    std::map<unsigned, std::map<Entity::EntityDirection, std::vector<Entity>>> entities_map;
    for (unsigned row_index = 0; row_index < Constants::num_battle_rows; row_index++) {
        Entity entity_right_to_left(Entity::SWORD_MAN, Entity::RIGHT_TO_LEFT, row_index);
        entities_map[row_index][entity_right_to_left.get_entity_direction()].emplace_back(entity_right_to_left);
    }

    Events events;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);

    while (true) {
        // Handle event
        events.update_events(window);
        if (events.is_quit()) {
            break;
        }
        int row_num = static_cast<int>(static_cast<float>(events.get_cursor_pos()->y) / Constants::row_height);
        if (events.is_mouse_button_down(SDL_BUTTON_LEFT)) {
            events.release_mouse_button(SDL_BUTTON_LEFT);
            if (row_num >= 0 && row_num < Constants::num_battle_rows) {
                Entity new_entity(Entity::GUN_MAN, Entity::LEFT_TO_RIGHT, row_num);
                entities_map[row_num][Entity::LEFT_TO_RIGHT].emplace_back(new_entity);
            }
        }

        // Physics and game
        for (int row_index = 0; row_index < Constants::num_battle_rows; row_index++) {
            std::map<Entity::EntityDirection, std::vector<Entity>> entities_map_for_row = entities_map[row_index];

            std::vector<Entity> entity_left_to_right_list = entities_map_for_row[Entity::LEFT_TO_RIGHT];
            std::vector<Entity> entity_right_to_left_list = entities_map_for_row[Entity::RIGHT_TO_LEFT];

            std::optional<Entity> left_to_right_entity_first = std::nullopt;
            int left_to_right_first_entity_index = -1;
            if (!entity_left_to_right_list.empty()) {
                left_to_right_entity_first = entity_left_to_right_list[0];
                left_to_right_first_entity_index = 0;
                for (int i = 1; i < entity_left_to_right_list.size(); i++) {
                    Entity current_entity = entity_left_to_right_list[i];
                    if (current_entity.get_entity_dst_pos()->x > left_to_right_entity_first->get_entity_dst_pos()->x) {
                        left_to_right_entity_first = current_entity;
                        left_to_right_first_entity_index = i;
                    }
                }
            }

            std::optional<Entity> right_to_left_entity_first = std::nullopt;
            int right_to_left_first_entity_index = -1;
            if (!entity_right_to_left_list.empty()) {
                right_to_left_entity_first = entity_right_to_left_list[0];
                right_to_left_first_entity_index = 0;
                for (int i = 1; i < entity_right_to_left_list.size(); i++) {
                    Entity current_entity = entity_right_to_left_list[i];
                    if (current_entity.get_entity_dst_pos()->x < right_to_left_entity_first->get_entity_dst_pos()->x) {
                        right_to_left_entity_first = current_entity;
                        right_to_left_first_entity_index = i;
                    }
                }
            }


            for (Entity& left_to_right : entity_left_to_right_list) {
                if (right_to_left_entity_first && left_to_right.get_entity_dst_pos()->x + left_to_right.get_entity_dst_pos()->w + left_to_right.get_range() >= right_to_left_entity_first->get_entity_dst_pos()->x) {
                    left_to_right.attack(entity_right_to_left_list[right_to_left_first_entity_index]);
                } else {
                    left_to_right.move();
                }
            }


            for (Entity& right_to_left: entity_right_to_left_list) {
                if (left_to_right_entity_first && right_to_left.get_entity_dst_pos()->x - right_to_left.get_range() <= left_to_right_entity_first->get_entity_dst_pos()->x + left_to_right_entity_first->get_entity_dst_pos()->w) {
                    right_to_left.attack(entity_left_to_right_list[left_to_right_first_entity_index]);
                } else {
                    right_to_left.move();
                }
            }

            std::vector<Entity>::iterator entity_ltr_it = entity_left_to_right_list.begin();
            while (entity_ltr_it != entity_left_to_right_list.end()) {
                if (entity_ltr_it->is_dead()) {
                    entity_ltr_it = entity_left_to_right_list.erase(entity_ltr_it);
                } else {
                    entity_ltr_it++;
                }
            }

            std::vector<Entity>::iterator entity_rtl_it = entity_right_to_left_list.begin();
            while (entity_rtl_it != entity_right_to_left_list.end()) {
                if (entity_rtl_it->is_dead()) {
                    entity_rtl_it = entity_right_to_left_list.erase(entity_rtl_it);
                } else {
                    entity_rtl_it++;
                }
            }


            entities_map_for_row[Entity::EntityDirection::LEFT_TO_RIGHT] = entity_left_to_right_list;
            entities_map_for_row[Entity::EntityDirection::RIGHT_TO_LEFT] = entity_right_to_left_list;

            entities_map[row_index] = entities_map_for_row;
        }



        // Rendering
        SDL_RenderClear(renderer);
        for (float x = 0; x < 1920; x += 128) {
            for (float y = 0; y < Constants::battlefield_height; y += 128) {
                SDL_FRect dst_tile_pos = { .x = x, .y = y, .w = 128, .h = 128 };
                SDL_RenderCopyF(renderer, background, nullptr, &dst_tile_pos);
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawLineF(renderer, 0, Constants::battlefield_height, 1920, Constants::battlefield_height);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_FRect dst_pos = { .x = 0, .y = Constants::battlefield_height + 1, .w = 1920, .h = 200 };
        SDL_RenderFillRectF(renderer, &dst_pos);
        AnimationEntity* coin_anim = dynamic_cast<AnimationEntity*>(Constants::get_animation(Constants::COIN).get());
        SDL_FRect coin_dst_rect = { .x = 1600, .y = Constants::battlefield_height + 30, .w = 80, .h = 80 };
        SDL_RenderCopyF(renderer, coin_anim->get_texture(), nullptr, &coin_dst_rect);
        coin_anim->goto_next_texture_index();

        SDL_Surface *text = TTF_RenderText_Shaded(font, "100", {.r = 0, .g = 0, .b = 0, .a = 255}, {.r = 255, .g = 255, .b = 255, .a = 255});
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text);
        coin_dst_rect.x += 120;
        coin_dst_rect.w = text->w;
        coin_dst_rect.h = text->h;
        SDL_RenderCopyF(renderer, text_texture, nullptr, &coin_dst_rect);
        SDL_DestroyTexture(text_texture);
        SDL_FreeSurface(text);


        if (row_num > 0 && row_num < Constants::num_battle_rows) {
            SDL_SetRenderDrawColor(renderer, 128, 128, 128 ,64);
            SDL_FRect dst_pos = { .x = 0, .y = row_num * Constants::row_height, .w = 1920, .h = Constants::row_height };
            SDL_RenderFillRectF(renderer, &dst_pos);
        }

        // Rendering entities
        for (int row_index = 0; row_index < Constants::num_battle_rows; row_index++) {
            std::map<Entity::EntityDirection, std::vector<Entity>> entity_direction_map = entities_map[row_index];
            for (int direction = Entity::EntityDirection::LEFT_TO_RIGHT; direction < Entity::EntityDirection::NUM_DIRECTIONS; direction++) {
                std::vector<Entity> entity_list = entity_direction_map[static_cast<Entity::EntityDirection>(direction)];
                for (Entity& entity : entity_list) {
                    SDL_RenderCopyExF(renderer, entity.get_entity_texture(), nullptr, entity.get_entity_dst_pos(), 0, nullptr, entity.get_render_flip_from_direction());
                    entity.render_health_bar(renderer);
                }
            }

        }

        SDL_RenderPresent(renderer);

        // FPS
        SDL_framerateDelay(&fps_manager);
    }


    TTF_CloseFont(font);
    SDL_DestroyTexture(background);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
