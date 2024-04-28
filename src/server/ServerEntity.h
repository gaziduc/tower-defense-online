//
// Created by Gazi on 4/28/2024.
//

#ifndef SERVERENTITY_H
#define SERVERENTITY_H

#include "../Entity.h"


class ServerEntity {
private:
    Entity::EntityType _entity_type;
    Entity::EntityDirection _entity_direction;
    unsigned _row_num;
    int _health;

public:
    ServerEntity(Entity::EntityType entity_type, Entity::EntityDirection direction, unsigned row_num);

    Entity::EntityType get_entity_type();
    Entity::EntityDirection get_entity_direction();
    unsigned get_row_num();
    int get_health();
};



#endif //SERVERENTITY_H
