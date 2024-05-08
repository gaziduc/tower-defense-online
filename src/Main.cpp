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

#include <sstream>

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
#include "network/NetworkUtils.h"
#include "server/ServerConstants.h"

void send_new_entity_to_server(TCPsocket socket, Entity::EntityType type, Entity::EntityDirection direction, int row_num);
void send_time_message_to_server(TCPsocket socket, Uint32 time);
Uint32 handle_server_actions(SDL_Window* window, SDLNet_SocketSet socket_set, TCPsocket socket, Player& player, Player& enemy_player, int latency);
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

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

    SDL_Window *window = SDL_CreateWindow(Constants::PROJECT_NAME.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        ErrorUtils::display_last_sdl_error_and_quit(nullptr);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        ErrorUtils::display_last_sdl_error_and_quit(window);
    }

    int img_flags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_WEBP;
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

    Mix_AllocateChannels(32);

    if (SDLNet_Init() == -1) {
        ErrorUtils::display_last_net_error_and_quit(window);
    }


    Mix_Music* music = Mix_LoadMUS("resources/audio/glorious_morning.mp3");
    if (music == nullptr) {
        ErrorUtils::display_last_mix_error_and_quit(window);
    }

    Mix_PlayMusic(music, -1);

    // Load resources
    Constants::load_animations(window, renderer);
    SDL_Texture *title_bg = IMG_LoadTexture(renderer, "resources/images/backgrounds/title.jpg");
    if (title_bg == nullptr) {
        ErrorUtils::display_last_img_error_and_quit(window);
    }
    SDL_Texture *background = IMG_LoadTexture(renderer, "resources/images/backgrounds/0.png");
    if (background == nullptr) {
        ErrorUtils::display_last_img_error_and_quit(window);
    }

    Constants::load_fonts(window);
    Constants::load_chunks(window);

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

            std::vector<std::string> result;
            std::stringstream ss(input);
            std::string item;

            while (getline(ss, item, ':')) {
                result.push_back(item);
            }


            IPaddress ip;
            if (SDLNet_ResolveHost(&ip, result[0].c_str(), std::stoi(result[1])) == -1) {
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
        SDL_RenderCopy(renderer, title_bg, nullptr, nullptr);
        SDL_FRect dst_pos = {.x = 0, .y = 350 * ratio_y, .w = Constants::VIEWPORT_WIDTH * ratio_x, .h = 400 * ratio_y};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
        SDL_RenderFillRectF(renderer, &dst_pos);

        std::string enter_ip = "Enter the server IPv4 address or hostname, and the server port.";
        std::string example = "Example: 192.168.1.86:7777";
        dst_pos = {.x = Constants::SDL_POS_X_CENTERED, .y = 400, .w = 0, .h = 0};
        RenderUtils::render_text(window, renderer, enter_ip, &dst_pos, {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE}, ratio_x, ratio_y);
        dst_pos = {.x = Constants::SDL_POS_X_CENTERED, .y = 500, .w = 0, .h = 0};
        RenderUtils::render_text(window, renderer, example, &dst_pos, {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE}, ratio_x, ratio_y);
        dst_pos = {.x = Constants::SDL_POS_X_CENTERED, .y = 600, .w = 0, .h = 0};
        RenderUtils::render_text_prompt(window, renderer, input, &dst_pos, {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE}, ratio_x, ratio_y);
        SDL_RenderPresent(renderer);

        SDL_framerateDelay(&fps_manager);
    }

    SDL_StopTextInput();

    Player player(Constants::INITIAL_PLAYER_HEALTH, Constants::INITIAL_PLAYER_MONEY);
    Player enemy_player(Constants::INITIAL_PLAYER_HEALTH, Constants::INITIAL_PLAYER_MONEY);

    Uint32 send_ticks = SDL_GetTicks();
    send_time_message_to_server(socket, send_ticks);

    Uint32 recv_server_ticks = 0;
    while (recv_server_ticks == 0) {
        recv_server_ticks = handle_server_actions(window, socket_set, socket, player, enemy_player, 0);
    }

    Uint32 recv_ticks = SDL_GetTicks();
    Uint32 latency = (recv_ticks - send_ticks) / 2;


    bool game_started = false;
    while (!game_started) {
        events.update_events(window);
        if (events.is_quit()) {
            exit(0);
        }

        handle_server_actions(window, socket_set, socket, player, enemy_player, latency);
        if (player.get_direction() != Entity::UNDEFINED && enemy_player.get_direction() != Entity::UNDEFINED) {
            game_started = true;
        }

        SDL_Point window_size;
        SDL_GetWindowSize(window, &window_size.x, &window_size.y);
        float ratio_x = static_cast<float>(window_size.x) / Constants::VIEWPORT_WIDTH;
        float ratio_y = static_cast<float>(window_size.y) / Constants::VIEWPORT_HEIGHT;

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, title_bg, nullptr, nullptr);
        SDL_FRect dst_pos = {.x = 0, .y = 400 * ratio_y, .w = Constants::VIEWPORT_WIDTH * ratio_x, .h = 300 * ratio_y};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
        SDL_RenderFillRectF(renderer, &dst_pos);

        std::string waiting_text = "Waiting for another player...";
        dst_pos = {.x = Constants::SDL_POS_X_CENTERED, .y = 500, .w = 0, .h = 0};
        RenderUtils::render_text(window, renderer, waiting_text, &dst_pos, {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE}, ratio_x, ratio_y);
        SDL_RenderPresent(renderer);


        SDL_framerateDelay(&fps_manager);
    }

    std::vector<std::pair<TCPsocket, Player>> dummy_vect;

    while (player.get_health() > 0 && enemy_player.get_health() > 0) {
        // Handle event
        events.update_events(window);
        if (events.is_quit()) {
            break;
        }

        handle_server_actions(window, socket_set, socket, player, enemy_player, latency);

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
                    Constants::play_drop_entity_chunk();
                } else {
                    Constants::play_error_chunk();
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
        Game::process_game(player, enemy_player, false, dummy_vect);

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
            SDL_SetRenderDrawColor(renderer, 128, 128, 128,64);
            SDL_FRect dst_pos = { .x = 0, .y = row_num * Constants::ROW_HEIGHT * ratio_y, .w = Constants::VIEWPORT_WIDTH * ratio_x, .h = Constants::ROW_HEIGHT * ratio_y };
            SDL_RenderFillRectF(renderer, &dst_pos);

            dst_pos.x = player.get_direction() == Entity::LEFT_TO_RIGHT ? 0 : (Constants::VIEWPORT_WIDTH - 80) * ratio_x;
            dst_pos.w = 80 * ratio_x;
            RenderUtils::render_animation_entity_ex(renderer, Constants::ARROW, &dst_pos, 90, nullptr, player.get_direction() == Entity::LEFT_TO_RIGHT ? SDL_FLIP_NONE : SDL_FLIP_VERTICAL);
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

    while (true) {
        events.update_events(window);
        if (events.is_quit()) {
            exit(0);
        }

        SDL_Point window_size;
        SDL_GetWindowSize(window, &window_size.x, &window_size.y);
        float ratio_x = static_cast<float>(window_size.x) / Constants::VIEWPORT_WIDTH;
        float ratio_y = static_cast<float>(window_size.y) / Constants::VIEWPORT_HEIGHT;


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, title_bg, nullptr, nullptr);

        SDL_FRect dst_pos = {.x = 0, .y = 450 * ratio_y, .w = Constants::VIEWPORT_WIDTH * ratio_x, .h = 200 * ratio_y};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
        SDL_RenderFillRectF(renderer, &dst_pos);

        dst_pos = {.x = Constants::SDL_POS_X_CENTERED, .y = 500, .w = 0, .h = 0};
        std::string end_str(player.get_health() <= 0 ? "You lost!" : "You won!");
        RenderUtils::render_text(window, renderer, end_str, &dst_pos, {.r = 255, .g = 255, .b = 255, .a = SDL_ALPHA_OPAQUE}, ratio_x, ratio_y);

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
    char message[4] = { 0 };
    message[0] = Constants::MESSAGE_NEW_ENTITY;
    message[1] = type;
    message[2] = direction;
    message[3] = row_num;
    SDLNet_TCP_Send(socket, message, 4);
}


void send_time_message_to_server(TCPsocket socket, Uint32 time) {
    char message[5] = { 0 };
    message[0] = Constants::MESSAGE_TIME;
    SDLNet_Write32(time, message + 1);
    SDLNet_TCP_Send(socket, message, 5);
}


Uint32 handle_server_actions(SDL_Window* window, SDLNet_SocketSet socket_set, TCPsocket socket, Player& player, Player& enemy_player, int latency_frames) {
    int sockets_ready_for_reading = SDLNet_CheckSockets(socket_set, 0);
    if (sockets_ready_for_reading == -1) {
        ErrorUtils::display_last_net_error_and_quit(window);
    }

    if (sockets_ready_for_reading > 0) {
        std::vector<std::string> splitted_messages;

        // Read from server
        char data[ServerConstants::MAX_MESSAGE_SIZE] = { 0 };
        int received_bytes = SDLNet_TCP_Recv(socket, data, ServerConstants::MAX_MESSAGE_SIZE);

        int byte_index = 0;

        Uint32 server_time = 0;
        while (byte_index < received_bytes) {
            if (data[byte_index] == Constants::MESSAGE_TIME) {
                server_time = SDLNet_Read32(data + byte_index + 1);
                byte_index += 5;
            } else if (data[byte_index] == Constants::MESSAGE_NEW_ENTITY) {
                // read
                Entity::EntityType type = static_cast<Entity::EntityType>(data[byte_index + 1]);
                Entity::EntityDirection direction = static_cast<Entity::EntityDirection>(data[byte_index + 2]);
                int row_num = data[byte_index + 3];
                Uint32 id = SDLNet_Read32(data + byte_index + 4);

                // add
                Entity new_entity(type, direction, row_num, id);

                // Handle latency
                if (latency_frames > 0) {
                    new_entity.move(latency_frames);
                }

                if (direction == player.get_direction()) {
                    player.get_entities_map()[row_num].push_back(new_entity);
                } else {
                    enemy_player.get_entities_map()[row_num].push_back(new_entity);
                }

                byte_index += 8;
            } else if (data[byte_index] == Constants::MESSAGE_STATE_BEGIN) {
                byte_index++;

                int num_entities = SDLNet_Read16(data + byte_index);
                byte_index += 2;

                Entity::EntityDirection direction = static_cast<Entity::EntityDirection>(data[byte_index]);
                if (direction == player.get_direction()) {
                    player.set_health(SDLNet_Read32(data + byte_index + 1));
                    player.set_money(SDLNet_Read32(data + byte_index + 5));

                    enemy_player.set_health(SDLNet_Read32(data + byte_index + 10));
                    enemy_player.set_money(SDLNet_Read32(data + byte_index + 14));
                } else {
                    enemy_player.set_health(SDLNet_Read32(data + byte_index + 1));
                    enemy_player.set_money(SDLNet_Read32(data + byte_index + 5));

                    player.set_health(SDLNet_Read32(data + byte_index + 10));
                    player.set_money(SDLNet_Read32(data + byte_index + 14));
                }

                byte_index += 18;

                for (int i = 0; i < num_entities; i++) {
                    Entity::EntityType type = static_cast<Entity::EntityType>(data[byte_index + 1]);
                    Entity::EntityDirection direction = static_cast<Entity::EntityDirection>(data[byte_index + 2]);
                    int row_num = data[byte_index + 3];
                    Uint32 id = SDLNet_Read32(data + byte_index +  4);
                    float pos_x = NetworkUtils::read_float(data + byte_index +  8);
                    int max_health = SDLNet_Read32(data + byte_index +  12);
                    int health = SDLNet_Read32(data + byte_index + 16);
                    Entity::EntityState state = static_cast<Entity::EntityState>(data[byte_index + 20]);

                    if (player.get_direction() == direction) {
                        std::vector<Entity>& entities_map = player.get_entities_map()[row_num];
                        for (Entity& entity : entities_map) {
                            if (entity.get_id() == id) {
                                entity.set_max_health(max_health);
                                entity.set_health(health);
                                entity.set_pos_x(pos_x);
                                if (state == Entity::RUNNING) {
                                    entity.move(latency_frames);
                                }
                            }
                        }
                    } else {
                        std::vector<Entity>& entities_map = enemy_player.get_entities_map()[row_num];
                        for (Entity& entity : entities_map) {
                            if (entity.get_id() == id) {
                                entity.set_max_health(max_health);
                                entity.set_health(health);
                                entity.set_pos_x(pos_x);
                                if (state == Entity::RUNNING) {
                                    entity.move(latency_frames);
                                }
                            }
                        }
                    }

                    byte_index += 21;
                }

                byte_index++;
            } else if (data[byte_index] == Constants::MESSAGE_START) {
                Entity::EntityDirection player_direction = static_cast<Entity::EntityDirection>(data[byte_index + 1]);
                if (player_direction == Entity::LEFT_TO_RIGHT) {
                    player.set_direction(Entity::LEFT_TO_RIGHT);
                    enemy_player.set_direction(Entity::RIGHT_TO_LEFT);
                } else if (player_direction == Entity::RIGHT_TO_LEFT) {
                    player.set_direction(Entity::RIGHT_TO_LEFT);
                    enemy_player.set_direction(Entity::LEFT_TO_RIGHT);
                }

                byte_index += 2;
            } else if (data[byte_index] == Constants::MESSAGE_ENTITY_DEATH) {
                Entity::EntityDirection direction = static_cast<Entity::EntityDirection>(data[byte_index + 1]);
                unsigned row_num = data[byte_index + 2];
                Uint32 id = SDLNet_Read32(data + byte_index + 3);

                // kill entity if needed, usually an entity is killed by server and clients
                // but sometimes there are de-syncs and you need to kill in clients
                if (player.get_direction() == direction) {
                    std::vector<Entity>& entities_map = player.get_entities_map()[row_num];
                    std::vector<Entity>::iterator entity_it = entities_map.begin();
                    while (entity_it != entities_map.end()) {
                        if (entity_it->get_id() == id) {
                            entity_it = entities_map.erase(entity_it);
                            break;
                        }

                        entity_it++;
                    }
                } else {
                    std::vector<Entity>& entities_map = enemy_player.get_entities_map()[row_num];
                    std::vector<Entity>::iterator entity_it = entities_map.begin();
                    while (entity_it != entities_map.end()) {
                        if (entity_it->get_id() == id) {
                            entity_it = entities_map.erase(entity_it);
                            break;
                        }

                        entity_it++;
                    }
                }

                byte_index += 7;
            }
        }

        return server_time;
    }

    return 0;
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
