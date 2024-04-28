//
// Created by Gazi on 4/28/2024.
//

#include "ServerEntity.h"

ServerEntity::ServerEntity(Entity::EntityType entity_type, Entity::EntityDirection direction, unsigned row_num) {
    _entity_type = entity_type;
    _entity_direction = direction;
    _row_num = row_num;
    _health = 100;
}

Entity::EntityType ServerEntity::get_entity_type() {
    return _entity_type;
}

Entity::EntityDirection ServerEntity::get_entity_direction() {
    return _entity_direction;
}

unsigned ServerEntity::get_row_num() {
    return _row_num;
}

int ServerEntity::get_health() {
    return _health;
}
