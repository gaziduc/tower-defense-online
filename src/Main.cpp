#include <map>
#include <optional>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_net.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "Animation.h"
#include "AnimationEntity.h"
#include "Colors.h"
#include "Constants.h"
#include "Entity.h"
#include "ErrorUtils.h"
#include "Events.h"
#include "Player.h"
#include "RenderUtils.h"
#include "framerate/SDL2_framerate.h"
#include "game/Game.h"

void send_new_entity_to_server(TCPsocket socket, Entity::EntityType type, Entity::EntityDirection direction, int row_num);
void handle_server_actions(SDL_Window* window, SDLNet_SocketSet socket_set, TCPsocket socket, Player& player, Player& enemy_player);
void pop_back_utf8(std::string& utf8_str);

int main(int argc, char *argv[]) {
    Colors::enable_colors_ifn();
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #endif
    /* Init SDL2 and show window */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        ErrorUtils::display_last_sdl_error_and_quit(nullptr);
    }

    SDL_Window *window = SDL_CreateWindow(Constants::PROJECT_NAME.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        ErrorUtils::display_last_sdl_error_and_quit(nullptr);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        ErrorUtils::display_last_sdl_error_and_quit(window);
    }

    int img_flags = IMG_INIT_PNG | IMG_INIT_WEBP;
    int img_initted_flags = IMG_Init(img_flags);
    if ((img_initted_flags & img_flags) != img_flags) {
        ErrorUtils::display_last_img_error_and_quit(window);
    }

    if (TTF_Init() == -1) {
        ErrorUtils::display_last_ttf_error_and_quit(window);
    }

    int mix_flags = MIX_INIT_MP3;
    int mix_initted_flags = Mix_Init(mix_flags);
    if ((mix_initted_flags & img_flags) != mix_flags) {
        ErrorUtils::display_last_mix_error_and_quit(window);
    }

    if (Mix_OpenAudio(48000, AUDIO_F32SYS, 2, 2048) == -1) {
        ErrorUtils::display_last_mix_error_and_quit(window);
    }

    if (SDLNet_Init() == -1) {
        ErrorUtils::display_last_net_error_and_quit(window);
    }


    Mix_Music* music = Mix_LoadMUS("resources/audio/glorious_morning.mp3");
    if (music == nullptr) {
        ErrorUtils::display_last_mix_error_and_quit(window);
    }

    // Load resources
    Constants::load_animations(window, renderer);
    SDL_Texture *background = IMG_LoadTexture(renderer, "resources/images/backgrounds/0.png");
    if (background == nullptr) {
        ErrorUtils::display_last_img_error_and_quit(window);
    }
    Constants::load_fonts(window);

    FPSmanager fps_manager;
    SDL_initFramerate(&fps_manager);
    SDL_setFramerate(&fps_manager, 60);

    Events events;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);

    TCPsocket socket = nullptr;
    SDLNet_SocketSet socket_set = SDLNet_AllocSocketSet(1);

    SDL_StartTextInput();
    std::string input;

    bool connected = false;
    while (!connected) {
        events.update_events(window);
        if (events.is_quit()) {
            exit(0);
        }

        if (events.is_key_down(SDL_SCANCODE_BACKSPACE)) {
            pop_back_utf8(input);
            events.press_up_key(SDL_SCANCODE_BACKSPACE);
        } else {
            input += events.get_text();
        }

        if (SDL_GetModState() & KMOD_CTRL && events.is_key_down(SDL_SCANCODE_V)) {
            input += SDL_GetClipboardText();
            events.press_up_key(SDL_SCANCODE_V);
        }

        if (events.is_enter_down()) {
            events.press_up_key(SDL_SCANCODE_RETURN);
            events.press_up_key(SDL_SCANCODE_KP_ENTER);

            IPaddress ip;
            if (SDLNet_ResolveHost(&ip, input.c_str(), 7777) == -1) {
                ErrorUtils::display_last_net_error_and_quit(window);
            }

            socket = SDLNet_TCP_Open(&ip);
            if (socket == nullptr) {
                ErrorUtils::display_last_net_error_and_quit(window);
            }

            SDLNet_TCP_AddSocket(socket_set, socket);
            connected = true;
        }

        SDL_Point window_size;
        SDL_GetWindowSize(window, &window_size.x, &window_size.y);
        float ratio_x = static_cast<float>(window_size.x) / Constants::VIEWPORT_WIDTH;
        float ratio_y = static_cast<float>(window_size.y) / Constants::VIEWPORT_HEIGHT;

        SDL_RenderClear(renderer);
        std::string enter_ip = "Enter the server IP address or hostname:";
        SDL_FRect dst_pos = {.x = Constants::SDL_POS_X_CENTERED, .y = 450, .w = 0, .h = 0};
        RenderUtils::render_text_shaded(window, renderer, enter_ip, &dst_pos, {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE},  {.r = 0, .g = 0, .b = 0, .a = SDL_ALPHA_OPAQUE}, ratio_x, ratio_y);
        dst_pos = {.x = Constants::SDL_POS_X_CENTERED, .y = 550, .w = 0, .h = 0};
        RenderUtils::render_text_shaded_prompt(window, renderer, input, &dst_pos, {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE},  {.r = 0, .g = 0, .b = 0, .a = SDL_ALPHA_OPAQUE}, ratio_x, ratio_y);
        SDL_RenderPresent(renderer);

        SDL_framerateDelay(&fps_manager);
    }

    SDL_StopTextInput();

    Player player(Constants::INITIAL_PLAYER_HEALTH, Constants::INITIAL_PLAYER_MONEY);
    Player enemy_player(Constants::INITIAL_PLAYER_HEALTH, Constants::INITIAL_PLAYER_MONEY);

    bool game_started = false;
    while (!game_started) {
        events.update_events(window);
        if (events.is_quit()) {
            exit(0);
        }

        handle_server_actions(window, socket_set, socket, player, enemy_player);
        if (player.get_direction() != Entity::UNDEFINED && enemy_player.get_direction() != Entity::UNDEFINED) {
            game_started = true;
        }

        SDL_Point window_size;
        SDL_GetWindowSize(window, &window_size.x, &window_size.y);
        float ratio_x = static_cast<float>(window_size.x) / Constants::VIEWPORT_WIDTH;
        float ratio_y = static_cast<float>(window_size.y) / Constants::VIEWPORT_HEIGHT;

        SDL_RenderClear(renderer);
        std::string waiting_text = "Waiting for another player...";
        SDL_FRect dst_pos = {.x = Constants::SDL_POS_X_CENTERED, .y = 500, .w = 0, .h = 0};
        RenderUtils::render_text_shaded(window, renderer, waiting_text, &dst_pos, {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE},  {.r = 0, .g = 0, .b = 0, .a = SDL_ALPHA_OPAQUE}, ratio_x, ratio_y);
        SDL_RenderPresent(renderer);


        SDL_framerateDelay(&fps_manager);
    }


    Mix_PlayMusic(music, -1);

    while (player.get_health() > 0 && enemy_player.get_health() > 0) {
        // Handle event
        events.update_events(window);
        if (events.is_quit()) {
            break;
        }

        handle_server_actions(window, socket_set, socket, player, enemy_player);

        SDL_Point window_size;
        SDL_GetWindowSize(window, &window_size.x, &window_size.y);
        float ratio_x = static_cast<float>(window_size.x) / Constants::VIEWPORT_WIDTH;
        float ratio_y = static_cast<float>(window_size.y) / Constants::VIEWPORT_HEIGHT;

        int row_num = static_cast<int>(static_cast<float>(events.get_cursor_pos()->y) / (Constants::ROW_HEIGHT * ratio_y));
        if (events.is_mouse_button_down(SDL_BUTTON_LEFT)) {
            events.release_mouse_button(SDL_BUTTON_LEFT);
            if (row_num >= 0 && row_num < Constants::NUM_BATTLE_ROWS) {
                int entity_cost = Entity::get_cost(player.get_selected_entity_type());
                if (player.get_money() >= entity_cost) {
                    player.increase_money(-entity_cost);
                    send_new_entity_to_server(socket, player.get_selected_entity_type(), player.get_direction(), row_num);
                }
            }
        }
        if (events.is_mouse_wheel_down()) {
            int new_selected_type = static_cast<int>(player.get_selected_entity_type()) - 1;
            if (new_selected_type < 0) {
                new_selected_type = Entity::EntityType::NUM_ENTITY_TYPE - 1;
            }
            player.set_selected_entity_type(static_cast<Entity::EntityType>(new_selected_type));
        } else if (events.is_mouse_wheel_up()) {
            int new_selected_type = static_cast<int>(player.get_selected_entity_type()) + 1;
            if (new_selected_type >= Entity::EntityType::NUM_ENTITY_TYPE) {
                new_selected_type = 0;
            }
            player.set_selected_entity_type(static_cast<Entity::EntityType>(new_selected_type));
        }

        // Game (move players, attacks, etc...)
        Game::process_game(player, enemy_player);

        // Rendering
        SDL_RenderClear(renderer);

        // Battlefield background
        for (float x = 0; x < Constants::VIEWPORT_WIDTH; x += 128 * ratio_x) {
            for (float y = 0; y < Constants::BATTLEFIELD_HEIGHT; y += 128 * ratio_y) {
                SDL_FRect dst_tile_pos = { .x = x, .y = y, .w = 128 * ratio_x, .h = 128 * ratio_y };
                SDL_RenderCopyF(renderer, background, nullptr, &dst_tile_pos);
            }
        }
        // Bottom menu
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLineF(renderer, 0, Constants::BATTLEFIELD_HEIGHT * ratio_y, Constants::VIEWPORT_WIDTH * ratio_x, Constants::BATTLEFIELD_HEIGHT * ratio_y);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_FRect dst_pos = { .x = 0, .y = (Constants::BATTLEFIELD_HEIGHT + 1) * ratio_y, .w = Constants::VIEWPORT_WIDTH * ratio_x, .h = 200 * ratio_y };
        SDL_RenderFillRectF(renderer, &dst_pos);
        // Coins
        dst_pos = { .x = 1680, .y = Constants::BATTLEFIELD_HEIGHT + 20, .w = 80, .h = 80 };
        std::string money_string(std::to_string(player.get_money()));
        RenderUtils::render_animation_entity_with_text(window, renderer, Constants::COIN, money_string, &dst_pos, ratio_x, ratio_y);
        // Health
        dst_pos = { .x = 1680, .y = Constants::BATTLEFIELD_HEIGHT + 100, .w = 80, .h = 80 };
        std::string health_string(std::to_string(player.get_health()));
        RenderUtils::render_animation_entity_with_text(window, renderer, Constants::HEALTH, health_string, &dst_pos, ratio_x, ratio_y);

        if (row_num >= 0 && row_num < Constants::NUM_BATTLE_ROWS) {
            SDL_SetRenderDrawColor(renderer, 128, 128, 128 ,64);
            SDL_FRect dst_pos = { .x = 0, .y = row_num * Constants::ROW_HEIGHT * ratio_y, .w = Constants::VIEWPORT_WIDTH * ratio_x, .h = Constants::ROW_HEIGHT * ratio_y };
            SDL_RenderFillRectF(renderer, &dst_pos);
        }

        // Rendering entities
        for (int row_index = 0; row_index < Constants::NUM_BATTLE_ROWS; row_index++) {
            std::vector<Entity> entity_list = player.get_entities_map()[row_index];
            for (Entity& entity : entity_list) {
                SDL_FRect* entity_dst_pos = entity.get_entity_dst_pos();
                SDL_FRect entity_render_pos = {.x = entity_dst_pos->x * ratio_x, .y = entity_dst_pos->y * ratio_y, .w = entity_dst_pos->w * ratio_x, .h = entity_dst_pos->h * ratio_y};
                SDL_RenderCopyExF(renderer, entity.get_entity_texture(), nullptr, &entity_render_pos, 0, nullptr, entity.get_render_flip_from_direction());
                entity.render_health_bar(renderer, ratio_x, ratio_y);
            }
            std::vector<Entity> enemy_entity_list = enemy_player.get_entities_map()[row_index];
            for (Entity& entity : enemy_entity_list) {
                SDL_FRect* entity_dst_pos = entity.get_entity_dst_pos();
                SDL_FRect entity_render_pos = {.x = entity_dst_pos->x * ratio_x, .y = entity_dst_pos->y * ratio_y, .w = entity_dst_pos->w * ratio_x, .h = entity_dst_pos->h * ratio_y};
                SDL_RenderCopyExF(renderer, entity.get_entity_texture(), nullptr, &entity_render_pos, 0, nullptr, entity.get_render_flip_from_direction());
                entity.render_health_bar(renderer, ratio_x, ratio_y);
            }
        }
        float x = static_cast<float>(events.get_cursor_pos()->x);
        float y = static_cast<float>(events.get_cursor_pos()->y);
        dst_pos = {.x = x + 20, .y = y + 20 , .w = 0, .h = 64};
        Constants::Anim cursor_anim = static_cast<Constants::Anim>(player.get_selected_entity_type() * Constants::NUM_STATE_PER_ENTITY + 3);
        dst_pos.w = dst_pos.h * dynamic_cast<AnimationEntity*>(Constants::get_animation(cursor_anim).get())->get_ratio();
        RenderUtils::render_animation_entity(renderer, cursor_anim, &dst_pos);

        SDL_RenderPresent(renderer);

        // FPS
        SDL_framerateDelay(&fps_manager);
    }

    SDL_DestroyTexture(background);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDLNet_TCP_DelSocket(socket_set, socket);
    SDLNet_TCP_Close(socket);
    SDLNet_Quit();
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}

void send_new_entity_to_server(TCPsocket socket, Entity::EntityType type, Entity::EntityDirection direction, int row_num) {
    std::string message = Constants::MESSAGE_NEW_ENTITY + Constants::WORD_DELIMITER + std::to_string(type) + Constants::WORD_DELIMITER + std::to_string(direction) + Constants::WORD_DELIMITER + std::to_string(row_num);
    SDLNet_TCP_Send(socket, message.c_str(), message.size());
}

int handle_state_from_server(Player& player, Player& enemy_player, std::vector<std::vector<std::string>> splitted_messages, int splitted_message_index) {
    splitted_message_index++;

    while (splitted_message_index < splitted_messages.size()) {
        std::vector<std::string> splitted_message = splitted_messages[splitted_message_index];

        if (splitted_message[0] == Constants::MESSAGE_STATE_END) {
            return splitted_message_index;
        }

        if (splitted_message[0] == Constants::MESSAGE_ENTITY) {
            unsigned long entity_id = std::stoul(splitted_message[1]);
            Entity::EntityType entity_type = static_cast<Entity::EntityType>(std::stoi(splitted_message[2]));
            Entity::EntityDirection entity_direction = static_cast<Entity::EntityDirection>(std::stoi(splitted_message[3]));
            int row_num = std::stoi(splitted_message[4]);
            float pos_x = std::stof(splitted_message[5]);
            int max_health = std::stoi(splitted_message[6]);
            int health = std::stoi(splitted_message[7]);

            if (player.get_direction() == entity_direction) {
                std::vector<Entity>& entities_map = player.get_entities_map()[row_num];
                for (Entity& entity : entities_map) {
                    if (entity.get_id() == entity_id) {
                        entity.set_max_health(max_health);
                        entity.set_health(health);
                        entity.set_pos_x(pos_x);
                    }
                }
            } else {
                std::vector<Entity>& entities_map = enemy_player.get_entities_map()[row_num];
                for (Entity& entity : entities_map) {
                    if (entity.get_id() == entity_id) {
                        entity.set_max_health(max_health);
                        entity.set_health(health);
                        entity.set_pos_x(pos_x);
                    }
                }
            }
        }

        splitted_message_index++;
    }

    return splitted_message_index;
}


void handle_server_actions(SDL_Window* window, SDLNet_SocketSet socket_set, TCPsocket socket, Player& player, Player& enemy_player) {
    int sockets_ready_for_reading = SDLNet_CheckSockets(socket_set, 0);
    if (sockets_ready_for_reading == -1) {
        ErrorUtils::display_last_net_error_and_quit(window);
    }

    if (sockets_ready_for_reading > 0) {
        std::vector<std::string> splitted_messages;

        // Read from server
        char data[Constants::MAX_MESSAGE_SIZE] = { 0 };
        SDLNet_TCP_Recv(socket, data, Constants::MAX_MESSAGE_SIZE);
        std::string received_message(data);

        size_t start;
        size_t end = 0;

        // Split by message
        while ((start = received_message.find_first_not_of(Constants::MESSAGE_DELIMITER, end)) != std::string::npos) {
            end = received_message.find(Constants::MESSAGE_DELIMITER, start);
            splitted_messages.push_back(received_message.substr(start, end - start));
        }

        // Split by word
        std::vector<std::vector<std::string>> splitted_messages_words;
        for (std::string& message : splitted_messages) {
            std::vector<std::string> splitted_words;

            end = 0;
            while ((start = message.find_first_not_of(Constants::WORD_DELIMITER, end)) != std::string::npos)
            {
                end = message.find(Constants::WORD_DELIMITER, start);
                splitted_words.push_back(message.substr(start, end - start));
            }

            splitted_messages_words.push_back(splitted_words);
        }

        for (int i = 0; i < splitted_messages_words.size(); i++) {
            std::vector<std::string> splitted_message = splitted_messages_words[i];

            if (splitted_message[0] == Constants::MESSAGE_STATE_BEGIN) {
                i = handle_state_from_server(player, enemy_player, splitted_messages_words, i);
            } else if (splitted_message[0] == Constants::MESSAGE_NEW_ENTITY) {
                Entity::EntityType entity_type = static_cast<Entity::EntityType>(std::stoi(splitted_message[1]));
                Entity::EntityDirection entity_direction = static_cast<Entity::EntityDirection>(std::stoi(splitted_message[2]));
                int row_num = std::stoi(splitted_message[3]);
                unsigned long id = std::stoul(splitted_message[4]);

                Entity new_entity(entity_type, entity_direction, row_num, id);
                if (entity_direction == player.get_direction()) {
                    player.get_entities_map()[row_num].push_back(new_entity);
                } else {
                    enemy_player.get_entities_map()[row_num].push_back(new_entity);
                }
            } else if (splitted_message[0] == Constants::MESSAGE_START) {
                Entity::EntityDirection player_direction = static_cast<Entity::EntityDirection>(std::stoi(splitted_message[1]));
                if (player_direction == Entity::LEFT_TO_RIGHT) {
                    player.set_direction(Entity::LEFT_TO_RIGHT);
                    enemy_player.set_direction(Entity::RIGHT_TO_LEFT);
                } else if (player_direction == Entity::RIGHT_TO_LEFT) {
                    player.set_direction(Entity::RIGHT_TO_LEFT);
                    enemy_player.set_direction(Entity::LEFT_TO_RIGHT);
                }
            }
        }
    }
}

void pop_back_utf8(std::string& utf8_str) {
    if (utf8_str.empty())
        return;

    char* cp = utf8_str.data() + utf8_str.size();
    while (--cp >= utf8_str.data() && ((*cp & 0b10000000) && !(*cp & 0b01000000))) {}
    if (cp >= utf8_str.data()) {
        utf8_str.resize(cp - utf8_str.data());
    }
}
