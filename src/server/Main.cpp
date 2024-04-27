//
// Created by Gazi on 4/27/2024.
//
#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <SDL_net.h>

#include "Colors.h"
#include "ServerConstants.h"
#include "../IPv4.h"

int main(int argc, char *argv[]) {
    Colors::enable_colors();
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
        try {
            port = std::stoi(port_string);
        } catch (std::invalid_argument const& ex) {
            std::cout << "Error: " << ex.what() << std::endl;
        } catch (std::out_of_range const& ex) {
            std::cout << "Error: " << ex.what() << std::endl;
        }
    }


    while (port == -1) {
        std::cout << "Enter server port: ";
        std::cin >> port;
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

   //  std::vector<std::pair<TCPsocket, Player>> client_list;
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
            // Player new_player(100, ip_and_port);
            // client_list.push_back(std::make_pair<>(potential_new_client_socket, new_player));
            SDLNet_TCP_AddSocket(socket_set, potential_new_client_socket);
        }

        SDL_Delay(1);
    }

    SDLNet_FreeSocketSet(socket_set);
    SDLNet_TCP_Close(server_socket);

    SDLNet_Quit();

    return 0;
}
