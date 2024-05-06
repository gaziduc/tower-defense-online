//
// Created by Gazi on 4/27/2024.
//

#include "Player.h"


Player::Player(int initial_health, int initial_money) {
    _initial_health = initial_health;
    _health = initial_health;
    _money = initial_money;
    _selected_entity_type = Entity::SWORD_MAN;
    _direction = Entity::UNDEFINED;
}

int Player::get_health() {
    return _health;
}

void Player::set_health(int health) {
    _health = health;
}

void Player::decrease_health(int damage) {
    _health -= damage;
}

bool Player::is_dead() {
    return _health <= 0;
}

void Player::increase_money(int money_to_add) {
    _money += money_to_add;
}

int Player::get_money() {
    return _money;
}

void Player::set_money(int money) {
    _money = money;
}

Entity::EntityType Player::get_selected_entity_type() {
    return _selected_entity_type;
}

void Player::set_selected_entity_type(Entity::EntityType selected_entity_type) {
    _selected_entity_type = selected_entity_type;
}

std::map<unsigned, std::vector<Entity>>& Player::get_entities_map() {
    return _entities_map;
}

Entity::EntityDirection Player::get_direction() {
    return _direction;
}

void Player::set_direction(Entity::EntityDirection direction) {
    _direction = direction;
}
