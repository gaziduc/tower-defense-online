//
// Created by Gazi on 4/21/2024.
//

#include "Entity.h"

#include <optional>
#include <SDL_net.h>

#include "Constants.h"
#include "LogUtils.h"
#include "Player.h"

Entity::Entity(EntityType type, EntityDirection direction, unsigned row_num, Uint32 entity_id) {
    _id = entity_id;
    _entity_type = type;
    _entity_direction = direction;
    _entity_state = EntityState::RUNNING;
    _animation_frame = 0;
    _row_num = row_num;

    unsigned first_anim_index = static_cast<unsigned>(type) * Constants::NUM_STATE_PER_ENTITY;
    SDL_Point size = Constants::get_animation_size(static_cast<Constants::Anim>(first_anim_index));
    float height_ratio = static_cast<float>(size.x) / Constants::ENTITY_HEIGHT;
    float final_width = static_cast<float>(size.y) / height_ratio;
    _dst_pos.x = direction == EntityDirection::LEFT_TO_RIGHT ? -final_width : Constants::VIEWPORT_WIDTH;
    _dst_pos.y = static_cast<float>(row_num + 0.5) * Constants::ROW_HEIGHT - Constants::ENTITY_HEIGHT / 2;
    _dst_pos.w = final_width;
    _dst_pos.h = Constants::ENTITY_HEIGHT;
    _max_health = get_max_health();
    _health = _max_health;


    for (unsigned i = first_anim_index; i < first_anim_index + Constants::NUM_STATE_PER_ENTITY; i++) {
        _animations.push_back(Constants::get_animation(static_cast<Constants::Anim>(i)));
    }
}

Entity::EntityType Entity::get_entity_type() {
    return _entity_type;
}

int Entity::get_row_num() {
    return _row_num;
}

void Entity::set_entity_state(EntityState state) {
    _entity_state = state;
    _animation_frame = 0;

    int w = 0;
    int h = 0;
    SDL_QueryTexture(get_entity_texture(), nullptr, nullptr, &w, &h);
    float height_ratio = static_cast<float>(h) / Constants::ENTITY_HEIGHT;
    float final_width = static_cast<float>(w) / height_ratio;
    _dst_pos.y = static_cast<float>(_row_num + 0.5) * Constants::ROW_HEIGHT - Constants::ENTITY_HEIGHT / 2;
    _dst_pos.w = final_width;
    _dst_pos.h = Constants::ENTITY_HEIGHT;
}

SDL_Texture* Entity::get_entity_texture() {
    return _animations[_entity_state]->get_texture(_animation_frame);
}

SDL_FRect* Entity::get_entity_dst_pos() {
    return &_dst_pos;
}

void Entity::set_pos_x(float pos_x) {
    _dst_pos.x = pos_x;
}

void Entity::move(int num_times) {
    if (_entity_state != EntityState::RUNNING) {
        set_entity_state(EntityState::RUNNING);
    } else {
        for (int i = 0; i < num_times; i++) {
            _animation_frame++;
            reset_animation_ifn();
        }
    }

    _dst_pos.x += _entity_direction == EntityDirection::LEFT_TO_RIGHT ? 1.5 * static_cast<float>(num_times) : -1.5 * static_cast<float>(num_times);
}

void Entity::set_state(EntityState state) {
    if (_entity_state != state) {
        set_entity_state(state);
    } else {
        _animation_frame++;
        reset_animation_ifn();
    }
}

void Entity::attack(Entity& enemy) {
    set_state(EntityState::ATTACKING);

    if (_animation_frame == 0) {
        enemy.decrease_health(get_attack_damage());
    }
}

void Entity::attack(Player& enemy) {
    set_state(EntityState::ATTACKING);

    if (_animation_frame == 0) {
        enemy.decrease_health(get_attack_damage());
    }
}

void Entity::reset_animation_ifn() {
    if (_animations[_entity_state]->get_num_frames() <= _animation_frame) {
        _animation_frame = 0;
    }
}


Entity::EntityDirection Entity::get_entity_direction() {
    return _entity_direction;
}


SDL_RendererFlip Entity::get_render_flip_from_direction() {
    return _entity_direction == EntityDirection::LEFT_TO_RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
}

float Entity::get_range() {
    switch (_entity_type) {
        case EntityType::GUN_MAN:
            return 220;
        case EntityType::SWORD_MAN:
            return 0;
        default:
            LogUtils::log_message(LogUtils::SEVERE, "Unkwown entity type: " + _entity_type);
            return 0;
    }
}

void Entity::decrease_health(int health_to_decrease) {
    _health -= health_to_decrease;
}

int Entity::get_attack_damage() {
    switch (_entity_type) {
        case EntityType::GUN_MAN:
            return 10;
        case EntityType::SWORD_MAN:
            return 40;
        default:
            LogUtils::log_message(LogUtils::SEVERE, "Unkwown entity type: " + _entity_type);
            return 0;
    }
}

bool Entity::is_dead() {
    return _health <= 0;
}

int Entity::get_cost(Entity::EntityType entity_type) {
    switch (entity_type) {
        case SWORD_MAN:
            return 40;
        case GUN_MAN:
            return 30;
        default:
            LogUtils::log_message(LogUtils::SEVERE, "Unknown entity type: " + entity_type);
            return 0;
    }
}

bool Entity::can_attack_entity(std::optional<Entity>& enemy_entity_first) {
    if (_entity_direction == LEFT_TO_RIGHT) {
        return enemy_entity_first && get_entity_dst_pos()->x + get_entity_dst_pos()->w + get_range() >= enemy_entity_first->get_entity_dst_pos()->x;
    } else if (_entity_direction == RIGHT_TO_LEFT) {
        return enemy_entity_first && get_entity_dst_pos()->x - get_range() <= enemy_entity_first->get_entity_dst_pos()->x + enemy_entity_first->get_entity_dst_pos()->w;
    } else {
        LogUtils::log_message(LogUtils::SEVERE, "Entity with id " + std::to_string(_id) + " has an incorrect direction: " + std::to_string(_entity_direction));
        return false;
    }
}

bool Entity::can_attack_player(std::optional<Entity>& enemy_entity_first) {
    if (_entity_direction == LEFT_TO_RIGHT) {
        return !enemy_entity_first && get_entity_dst_pos()->x + get_entity_dst_pos()->w + get_range() >= 1920;
    } else if (_entity_direction == RIGHT_TO_LEFT) {
        return !enemy_entity_first && get_entity_dst_pos()->x - get_range() <= 0;
    } else {
        LogUtils::log_message(LogUtils::SEVERE, "Entity with id " + std::to_string(_id) + " has an incorrect direction: " + std::to_string(_entity_direction));
        return false;
    }
}


bool Entity::is_beyond_entity(Entity& first_entity) {
    if (_entity_direction == LEFT_TO_RIGHT) {
        return get_entity_dst_pos()->x > first_entity.get_entity_dst_pos()->x;
    } else if (_entity_direction == RIGHT_TO_LEFT) {
        return get_entity_dst_pos()->x < first_entity.get_entity_dst_pos()->x;
    } else {
        LogUtils::log_message(LogUtils::SEVERE, "Entity with id " + std::to_string(_id) + " has an incorrect direction: " + std::to_string(_entity_direction));
        return false;
    }
}

int Entity::get_health() {
    return _health;
}

void Entity::set_health(int health) {
    _health = health;
}

int Entity::get_max_health() {
    return 100;
}

void Entity::set_max_health(int max_health) {
    _max_health = max_health;
}


unsigned long Entity::get_id() {
    return  _id;
}


void Entity::render_health_bar(SDL_Renderer *renderer, float ratio_x, float ratio_y) {
    SDL_FRect dst_rect = {
        .x = (_dst_pos.x + _dst_pos.w / 2 - Constants::HEALTH_BAR_WIDTH / 2) * ratio_x,
        .y = (_dst_pos.y - Constants::HEALTH_BAR_HEIGHT) * ratio_y,
        .w = Constants::HEALTH_BAR_WIDTH * ratio_x,
        .h = Constants::HEALTH_BAR_HEIGHT * ratio_y
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 192);
    SDL_RenderFillRectF(renderer, &dst_rect);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 192);
    dst_rect.w = dst_rect.w * static_cast<float>(_health) / static_cast<float>(_max_health);
    SDL_RenderFillRectF(renderer, &dst_rect);
}

void Entity::send_death_to_client(std::vector<std::pair<TCPsocket, Player>>& sockets) {
    char message[7] = { 0 };
    message[0] = Constants::MESSAGE_ENTITY_DEATH;
    message[1] = get_entity_direction();
    message[2] = get_row_num();
    SDLNet_Write32(get_id(), message + 3);

    for (std::pair<TCPsocket, Player>& socket : sockets) {
        SDLNet_TCP_Send(socket.first, message, 7);
    }
}




