//
// Created by Gazi on 4/27/2024.
//

#ifndef PLAYER_H
#define PLAYER_H
#include "Entity.h"


class Player {
private:
    int _initial_health;
    int _health;
    int _money;
    Entity::EntityType _selected_entity_type;

public:
    Player(int initial_health, int initial_money);

    int get_health();
    void decrease_health(int damage);
    bool is_dead();
    void increase_money(int money_to_add);
    int get_money();
    Entity::EntityType get_selected_entity_type();
    void set_selected_entity_type(Entity::EntityType selected_entity_type);
};



#endif //PLAYER_H
