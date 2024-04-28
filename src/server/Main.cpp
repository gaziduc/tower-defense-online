//
// Created by Gazi on 4/27/2024.
//
#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <SDL_net.h>
#include <vector>

#include "Colors.h"
#include "ServerConstants.h"
#include "ServerEntity.h"
#include "ServerPlayer.h"
#include "../IPv4.h"
#include "../LogUtils.h"

int get_port_from_string(const std::string& port_string);
void send_to_clients(const std::vector<std::pair<TCPsocket, ServerPlayer>>& client_list, std::string& message);

int main(int argc, char *argv[]) {
    Colors::enable_colors_ifn();
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::cout << Colors::yellow() << "Welcome to Tower Defense Online Server!" << Colors::reset() << std::endl;
    if (SDLNet_Init() == -1) {
        std::cout << "Error: " << SDLNet_GetError() << std::endl;
        return 1;
    }

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

    std::vector<std::pair<TCPsocket, ServerPlayer>> client_list;
    SDLNet_SocketSet socket_set = SDLNet_AllocSocketSet(ServerConstants::MAX_CLIENTS);

    std::cout << "Server is now listening..." << std::endl;

    while (true) {
        TCPsocket potential_new_client_socket = SDLNet_TCP_Accept(server_socket);

        // Accept new connections
        if (potential_new_client_socket != nullptr) {
            IPaddress* new_client_ip_address = SDLNet_TCP_GetPeerAddress(potential_new_client_socket);
            IPv4 curr_ipv4(new_client_ip_address);

            std::cout << "User at " << Colors::yellow() << curr_ipv4.to_string() << Colors::reset() << ':' << Colors::green() << new_client_ip_address->port << Colors::reset() << " (" << SDLNet_ResolveIP(new_client_ip_address) << ") joined the server!" << std::endl;

            std::string ip_and_port = curr_ipv4.to_string() + ':' + std::to_string(new_client_ip_address->port);
            ServerPlayer new_player(ServerConstants::INITIAL_PLAYER_HEALTH, ServerConstants::INITIAL_PLAYER_MONEY, client_list.empty() ? Entity::LEFT_TO_RIGHT : Entity::RIGHT_TO_LEFT);
            client_list.push_back(std::make_pair<>(potential_new_client_socket, new_player));
            SDLNet_TCP_AddSocket(socket_set, potential_new_client_socket);

            if (client_list.size() == 2) {
                std::string client_0_str = ServerConstants::MESSAGE_START + ServerConstants::WORD_DELIMITER + std::to_string(Entity::EntityDirection::LEFT_TO_RIGHT);
                SDLNet_TCP_Send(client_list[0].first, client_0_str.c_str(), client_0_str.size());
                std::string client_1_str = ServerConstants::MESSAGE_START + ServerConstants::WORD_DELIMITER + std::to_string(Entity::EntityDirection::RIGHT_TO_LEFT);
                SDLNet_TCP_Send(client_list[1].first, client_1_str.c_str(), client_1_str.size());
            }
        }

        if (!client_list.empty()) {
            int sockets_ready_for_reading = SDLNet_CheckSockets(socket_set, 0);
            if (sockets_ready_for_reading == -1) {
                std::cout << "Error: " << SDLNet_GetError() << std::endl;
                return 1;
            }

            if (sockets_ready_for_reading > 0) {
                std::vector<std::pair<TCPsocket, ServerPlayer>>::iterator socket_iter = client_list.begin();
                while (socket_iter != client_list.end()) {
                    TCPsocket curr_socket = socket_iter->first;

                    if (SDLNet_SocketReady(curr_socket) > 0) {
                        char data[ServerConstants::MAX_MESSAGE_SIZE] = { 0 };

                        // If client disconnected
                        if (SDLNet_TCP_Recv(curr_socket, data, ServerConstants::MAX_MESSAGE_SIZE) <= 0) {
                            IPaddress* new_client_ip_address = SDLNet_TCP_GetPeerAddress(curr_socket);
                            IPv4 curr_ipv4(new_client_ip_address);
                            std::cout << "User at " << Colors::yellow() << curr_ipv4.to_string() << Colors::reset() << ':' << Colors::green() << new_client_ip_address->port << Colors::reset() << " (" << SDLNet_ResolveIP(new_client_ip_address) << ") disconnected." << std::endl;

                            // Remove socket and close connection
                            SDLNet_TCP_DelSocket(socket_set, curr_socket);
                            socket_iter = client_list.erase(socket_iter);
                            SDLNet_TCP_Close(curr_socket);

                            continue;
                        }

                        std::string received_message(data);
                        LogUtils::log_message(LogUtils::INFO, received_message);

                        std::vector<std::string> splitted_messages;
                        size_t start;
                        size_t end = 0;
                        while ((start = received_message.find_first_not_of(ServerConstants::MESSAGE_DELIMITER, end)) != std::string::npos) {
                            end = received_message.find(ServerConstants::MESSAGE_DELIMITER, start);
                            splitted_messages.push_back(received_message.substr(start, end - start));
                        }

                        std::vector<std::vector<std::string>> splitted_messages_words;
                        for (auto& message : splitted_messages) {
                            std::vector<std::string> splitted_words;
                            end = 0;
                            while ((start = message.find_first_not_of(ServerConstants::WORD_DELIMITER, end)) != std::string::npos) {
                                end = message.find(ServerConstants::WORD_DELIMITER, start);
                                splitted_words.push_back(message.substr(start, end - start));
                            }

                            splitted_messages_words.push_back(splitted_words);
                        }

                        for (std::vector<std::string>& message : splitted_messages_words) {
                            if (message[0] == ServerConstants::MESSAGE_NEW_ENTITY) {
                                ServerPlayer sender_player = socket_iter->second;

                                Entity::EntityType entity_type = static_cast<Entity::EntityType>(std::stoi(message[1]));
                                Entity::EntityDirection entity_direction = static_cast<Entity::EntityDirection>(std::stoi(message[2]));
                                int row_num = std::stoi(message[3]);

                                ServerEntity new_entity(entity_type, entity_direction, row_num);
                                sender_player.get_entities_map()[row_num].push_back(new_entity);

                                send_to_clients(client_list, received_message);
                            }
                        }
                    }

                    socket_iter++;
                }
            }
        }

        SDL_Delay(1);
    }

    SDLNet_FreeSocketSet(socket_set);
    SDLNet_TCP_Close(server_socket);

    SDLNet_Quit();

    return 0;
}


int get_port_from_string(const std::string& port_string) {
    try {
        return std::stoi(port_string);
    } catch (std::invalid_argument const& ex) {
        std::cout << "Error: " << port_string << " is not a valid port number (invalid argument)." << std::endl;
        return -1;
    } catch (std::out_of_range const& ex) {
        std::cout << "Error: " << port_string << " is not a valid port number (out of range)." << std::endl;
        return -1;
    }
}

void send_to_clients(const std::vector<std::pair<TCPsocket, ServerPlayer>>& client_list, std::string& message) {
    for (std::pair<TCPsocket, ServerPlayer> client : client_list) {
        SDLNet_TCP_Send(client.first, message.c_str(), message.size());
    }
}
