//
// Created by Gazi on 4/27/2024.
//
#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <SDL_image.h>
#include <SDL_net.h>
#include <vector>

#include "../Colors.h"
#include "../Constants.h"
#include "../ErrorUtils.h"
#include "../IPv4.h"
#include "../LogUtils.h"
#include "../Player.h"
#include "ServerConstants.h"
#include "../framerate/SDL2_framerate.h"
#include "../game/Game.h"
#include "../network/NetworkUtils.h"

int get_port_from_string(const std::string& port_string);
void send_to_clients(const std::vector<std::pair<TCPsocket, Player>>& client_list, char* message, int message_len);

int main(int argc, char *argv[]) {
    Colors::enable_colors_ifn();
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::cout << Colors::yellow() << "Welcome to " << Constants::PROJECT_NAME_SERVER << "!" << Colors::reset() << std::endl;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        ErrorUtils::display_last_net_error_and_quit(nullptr);
    }

    SDL_Window *window = SDL_CreateWindow(Constants::PROJECT_NAME_SERVER.c_str(), 0, 0, 0, 0, SDL_WINDOW_HIDDEN);
    if (window == nullptr) {
        ErrorUtils::display_last_sdl_error_and_quit(nullptr);
    }

    int img_flags = IMG_INIT_PNG | IMG_INIT_WEBP;
    int img_initted_flags = IMG_Init(img_flags);
    if ((img_initted_flags & img_flags) != img_flags) {
        ErrorUtils::display_last_img_error_and_quit(nullptr);
    }

    if (SDLNet_Init() == -1) {
        ErrorUtils::display_last_net_error_and_quit(nullptr);
    }

    std::cout << "Loading resources..." << std::endl;

    // Load resources
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    Constants::load_animations(nullptr, renderer);

    int port = -1;
    if (argc > 1) {
        std::string port_string(argv[1]);
        port = get_port_from_string(port_string);
    }


    while (port == -1) {
        std::cout << "Enter server port: ";
        std::string line;
        std::getline(std::cin, line);

        port = get_port_from_string(line);
    }

    IPaddress server_address;
    if (SDLNet_ResolveHost(&server_address, nullptr, port) == -1) {
        std::cout << "Error: " << SDLNet_GetError() << std::endl;
        return 1;
    }

    TCPsocket server_socket = SDLNet_TCP_Open(&server_address);
    if (server_socket == nullptr) {
        std::cout << "Error: " << SDLNet_GetError() << std::endl;
        return 1;
    }

    IPaddress local_ips[ServerConstants::MAX_LOCAL_IPS];
    const int num_local_ips = SDLNet_GetLocalAddresses(local_ips, ServerConstants::MAX_LOCAL_IPS);

    std::cout << "Clients on your local network can join you with:" << std::endl;

    for (int local_ip_index = 0; local_ip_index < num_local_ips; local_ip_index++) {
        IPaddress curr_local_ip = local_ips[local_ip_index];
        if (curr_local_ip.host != 0) {
            IPv4 curr_ipv4(&curr_local_ip);
            std::cout << Colors::yellow() << curr_ipv4.to_string() << Colors::reset() << ':' << Colors::green() << port << Colors::reset() << " (" << SDLNet_ResolveIP(&curr_local_ip) << ')' << std::endl;
        }
    }

    std::vector<std::pair<TCPsocket, Player>> client_list;
    SDLNet_SocketSet socket_set = SDLNet_AllocSocketSet(ServerConstants::MAX_CLIENTS);

    unsigned long next_entity_id = 0;

    FPSmanager fps_manager;
    SDL_initFramerate(&fps_manager);
    SDL_setFramerate(&fps_manager, 60);

    std::cout << "Server is now listening..." << std::endl;

    while (true) {
        TCPsocket potential_new_client_socket = SDLNet_TCP_Accept(server_socket);

        // Accept new connections
        if (potential_new_client_socket != nullptr) {
            IPaddress* new_client_ip_address = SDLNet_TCP_GetPeerAddress(potential_new_client_socket);
            IPv4 curr_ipv4(new_client_ip_address);

            std::cout << "User at " << Colors::yellow() << curr_ipv4.to_string() << Colors::reset() << ':' << Colors::green() << new_client_ip_address->port << Colors::reset() << " (" << SDLNet_ResolveIP(new_client_ip_address) << ") joined the server!" << std::endl;

            Player new_player(Constants::INITIAL_PLAYER_HEALTH, Constants::INITIAL_PLAYER_MONEY);
            new_player.set_direction(client_list.empty() ? Entity::LEFT_TO_RIGHT : Entity::RIGHT_TO_LEFT);

            client_list.push_back(std::make_pair<>(potential_new_client_socket, new_player));
            SDLNet_TCP_AddSocket(socket_set, potential_new_client_socket);

            if (client_list.size() == 2) {
                char message_client_0[2] = { 0 };
                message_client_0[0] = Constants::MESSAGE_START;
                message_client_0[1] = Entity::EntityDirection::LEFT_TO_RIGHT;
                SDLNet_TCP_Send(client_list[0].first, message_client_0, 2);

                char message_client_1[2] = { 0 };
                message_client_1[0] = Constants::MESSAGE_START;
                message_client_1[1] = Entity::EntityDirection::RIGHT_TO_LEFT;
                SDLNet_TCP_Send(client_list[1].first, message_client_1, 2);
            }
        }

        if (!client_list.empty()) {
            int sockets_ready_for_reading = SDLNet_CheckSockets(socket_set, 0);
            if (sockets_ready_for_reading == -1) {
                std::cout << "Error: " << SDLNet_GetError() << std::endl;
                return 1;
            }

            if (client_list.size() == 2) {
                Game::process_game(client_list[0].second, client_list[1].second, true, client_list);
            }

            if (sockets_ready_for_reading > 0) {
                int byte_index = 0;
                char message[ServerConstants::MAX_MESSAGE_SIZE] = { 0 };

                std::vector<std::pair<TCPsocket, Player>>::iterator socket_iter = client_list.begin();
                while (socket_iter != client_list.end()) {
                    TCPsocket curr_socket = socket_iter->first;

                    if (SDLNet_SocketReady(curr_socket) > 0) {
                        char data[ServerConstants::MAX_MESSAGE_SIZE] = { 0 };

                        // If client disconnected
                        int bytes_read = SDLNet_TCP_Recv(curr_socket, data, ServerConstants::MAX_MESSAGE_SIZE);
                        if (bytes_read <= 0) {
                            IPaddress* new_client_ip_address = SDLNet_TCP_GetPeerAddress(curr_socket);
                            IPv4 curr_ipv4(new_client_ip_address);
                            std::cout << "User at " << Colors::yellow() << curr_ipv4.to_string() << Colors::reset() << ':' << Colors::green() << new_client_ip_address->port << Colors::reset() << " (" << SDLNet_ResolveIP(new_client_ip_address) << ") disconnected." << std::endl;

                            // Remove socket and close connection
                            SDLNet_TCP_DelSocket(socket_set, curr_socket);
                            socket_iter = client_list.erase(socket_iter);
                            SDLNet_TCP_Close(curr_socket);

                            continue;
                        }


                        int read_byte_index = 0;
                        while (read_byte_index < bytes_read) {
                            if (data[read_byte_index] == Constants::MESSAGE_NEW_ENTITY) {
                                Player& sender_player = socket_iter->second;

                                Entity::EntityType entity_type = static_cast<Entity::EntityType>(data[read_byte_index + 1]);
                                Entity::EntityDirection entity_direction = static_cast<Entity::EntityDirection>(data[read_byte_index + 2]);
                                int row_num = data[read_byte_index + 3];

                                // create new entity in server
                                Entity new_entity(entity_type, entity_direction, row_num, next_entity_id);
                                next_entity_id++;
                                sender_player.get_entities_map()[row_num].push_back(new_entity);

                                sender_player.increase_money(-Entity::get_cost(entity_type));

                                message[byte_index] = Constants::MESSAGE_NEW_ENTITY;
                                message[byte_index + 1] = entity_type;
                                message[byte_index + 2] = entity_direction;
                                message[byte_index + 3] = row_num;
                                SDLNet_Write32(new_entity.get_id(), message + byte_index + 4);

                                byte_index += 8;
                                read_byte_index += 4;
                            } else if (data[read_byte_index] == Constants::MESSAGE_TIME) {
                                message[byte_index] = Constants::MESSAGE_TIME;
                                SDLNet_Write32(SDL_GetTicks(), message + byte_index + 1);

                                read_byte_index += 5;
                                byte_index += 5;
                            }
                        }
                    }

                    socket_iter++;
                }

                send_to_clients(client_list, message, byte_index);
            }

            // Send state each second
            if (fps_manager.framecount % 15 == 0 && client_list.size() == 2) {
                int byte_index = 0;
                char message[ServerConstants::MAX_MESSAGE_SIZE] = { 0 };

                message[byte_index] = Constants::MESSAGE_STATE_BEGIN;
                byte_index += 3;

                message[byte_index] = client_list[0].second.get_direction();
                SDLNet_Write32(client_list[0].second.get_health(), message + byte_index + 1);
                SDLNet_Write32(client_list[0].second.get_money(), message + byte_index + 5);

                message[byte_index + 9] = client_list[1].second.get_direction();
                SDLNet_Write32(client_list[1].second.get_health(), message + byte_index + 10);
                SDLNet_Write32(client_list[1].second.get_money(), message + byte_index + 14);
                byte_index += 18;

                int num_entities = 0;
                std::vector<std::pair<TCPsocket, Player>>::iterator socket_iter = client_list.begin();
                while (socket_iter != client_list.end()) {
                    Player& player = socket_iter->second;
                    for (int row_index = 0; row_index < Constants::NUM_BATTLE_ROWS; row_index++) {
                        std::vector<Entity>& entities = player.get_entities_map()[row_index];
                        for (Entity& entity : entities) {
                            message[byte_index] = Constants::MESSAGE_ENTITY;
                            message[byte_index + 1] = entity.get_entity_type();
                            message[byte_index + 2] = entity.get_entity_direction();
                            message[byte_index + 3] = row_index;
                            SDLNet_Write32(entity.get_id(), message + byte_index + 4);
                            NetworkUtils::write_float(entity.get_entity_dst_pos()->x, message + byte_index + 8);
                            SDLNet_Write32(entity.get_max_health(), message + byte_index + 12);
                            SDLNet_Write32(entity.get_health(), message + byte_index + 16);
                            message[byte_index + 20] = entity.get_state();

                            byte_index += 21;
                            num_entities++;
                        }
                    }

                    socket_iter++;
                }

                SDLNet_Write16(num_entities, message + 1);
                message[byte_index] = Constants::MESSAGE_STATE_END;
                byte_index++;

                send_to_clients(client_list, message, byte_index);
            }
        }

        SDL_framerateDelay(&fps_manager);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDLNet_FreeSocketSet(socket_set);
    SDLNet_TCP_Close(server_socket);

    SDLNet_Quit();

    return 0;
}


int get_port_from_string(const std::string& port_string) {
    try {
        return std::stoi(port_string);
    } catch (std::invalid_argument const& ex) {
        std::cout << "Error: " << Colors::green() << port_string << Colors::reset() << " is not a valid port number (invalid argument)." << std::endl;
        return -1;
    } catch (std::out_of_range const& ex) {
        std::cout << "Error: " << Colors::green() << port_string << Colors::reset() << " is not a valid port number (out of range)." << std::endl;
        return -1;
    }
}

void send_to_clients(const std::vector<std::pair<TCPsocket, Player>>& client_list, char* message, int message_len) {
    LogUtils::log_message(LogUtils::DEBUG, message);
    for (std::pair<TCPsocket, Player> client : client_list) {
        SDLNet_TCP_Send(client.first, message, message_len);
    }
}
