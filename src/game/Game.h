//
// Created by Gazi on 5/4/2024.
//

#ifndef GAME_H
#define GAME_H


#include "../Player.h"

class Game {
public:
    static void process_game(Player& player1, Player& player2, bool is_server_side, std::vector<std::pair<TCPsocket, Player>>& sockets);
};



#endif //GAME_H
