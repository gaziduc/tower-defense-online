//
// Created by Gazi on 4/28/2024.
//

#ifndef SERVERPLAYER_H
#define SERVERPLAYER_H
#include <map>
#include <vector>

#include "ServerEntity.h"


class ServerPlayer {
private:
    int _health;
    int _money;
    Entity::EntityDirection _direction;
    std::map<unsigned, std::vector<ServerEntity>> _entities_map;

public:
    ServerPlayer(int initial_health, int initial_money, Entity::EntityDirection direction);
    std::map<unsigned, std::vector<ServerEntity>>& get_entities_map();
};



#endif //SERVERPLAYER_H
