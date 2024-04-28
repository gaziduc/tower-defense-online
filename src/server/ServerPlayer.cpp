//
// Created by Gazi on 4/28/2024.
//

#include "ServerPlayer.h"

ServerPlayer::ServerPlayer(int initial_health, int initial_money, Entity::EntityDirection direction) {
    _health = initial_health;
    _money = initial_money;
    _direction = direction;
}

std::map<unsigned, std::vector<ServerEntity>>& ServerPlayer::get_entities_map() {
    return _entities_map;
}